/*
 * Six Sines
 *
 * A synth with audio rate modulation.
 *
 * Copyright 2024-2025, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work will be
 * released under GPL3.
 *
 * The source code and license are at https://github.com/baconpaul/six-sines
 */

#ifndef BACONPAUL_SIX_SINES_UI_PATCH_DATA_BINDINGS_H
#define BACONPAUL_SIX_SINES_UI_PATCH_DATA_BINDINGS_H

#include <cstdint>
#include "sst/jucegui/data/Continuous.h"
#include "sst/jucegui/data/Discrete.h"
#include "sst/jucegui/components/Knob.h"
#include "sst/jucegui/components/DraggableTextEditableValue.h"

#include "six-sines-editor.h"

namespace baconpaul::six_sines::ui
{
struct PatchContinuous : jdat::Continuous
{
    SixSinesEditor &editor;
    uint32_t pid;
    Param *p{nullptr};
    PatchContinuous(SixSinesEditor &e, uint32_t id) : editor(e), pid(id)
    {
        if (e.patchCopy.paramMap.find(id) == e.patchCopy.paramMap.end())
        {
            SXSNLOG("You were unable to find param " << id
                                                     << " - its probably not in patch::params()");
            assert(false);
            std::terminate();
        }

        p = e.patchCopy.paramMap.at(id);
    }
    ~PatchContinuous() override = default;

    std::string getLabel() const override
    {
        auto r = p->meta.name;
        return r;
    }
    float getValue() const override { return p->value; }
    std::string getValueAsStringFor(float f) const override
    {
        if (tsPowerPartner && tsPowerPartner->getValue())
        {
            auto r = p->meta.valueToString(
                f, sst::basic_blocks::params::ParamMetaData::FeatureState().withTemposync(true));
            if (r.has_value())
                return *r;
        }
        auto r = p->meta.valueToString(f);
        if (r.has_value())
            return *r;
        return "error";
    }
    void setValueAsString(const std::string &s) override
    {
        std::string em;
        auto v = p->meta.valueFromString(s, em);
        if (v.has_value())
        {
            setValueFromGUI(*v);
        }
    }
    void setValueFromGUI(const float &f) override
    {
        if (p->value == p->meta.minVal && f != p->value)
        {
            if (onPullFromMin)
                onPullFromMin();
        }
        p->value = f;
        editor.uiToAudio.push({Synth::UIToAudioMsg::Action::SET_PARAM, pid, f});
        editor.flushOperator();
        editor.updateTooltip(this);
    }
    void setValueFromModel(const float &f) override { p->value = f; }
    float getDefaultValue() const override { return p->meta.defaultVal; }
    bool isBipolar() const override { return p->meta.isBipolar(); }
    float getMin() const override { return p->meta.minVal; }
    float getMax() const override { return p->meta.maxVal; }

    jdat::Discrete *tsPowerPartner{nullptr};
    void setTemposyncPowerPartner(jdat::Discrete *d) { tsPowerPartner = d; }

    std::function<void()> onPullFromMin{nullptr};
};

struct PatchDiscrete : jdat::Discrete
{
    SixSinesEditor &editor;
    uint32_t pid;
    Param *p{nullptr};
    std::function<void()> onGuiSetValue{nullptr};

    PatchDiscrete(SixSinesEditor &e, uint32_t id) : editor(e), pid(id)
    {
        if (e.patchCopy.paramMap.find(id) == e.patchCopy.paramMap.end())
        {
            SXSNLOG("You were unable to find param " << id
                                                     << " - its probably not in patch::params()");
            assert(false);
            std::terminate();
        }
        p = e.patchCopy.paramMap.at(id);
    }
    ~PatchDiscrete() override = default;

    std::string getLabel() const override
    {
        auto r = p->meta.name;
        return r;
    }
    int getValue() const override { return static_cast<int>(std::round(p->value)); }
    std::string getValueAsStringFor(int i) const override
    {
        auto res = p->meta.valueToString(i);
        if (res.has_value())
            return *res;
        return "error";
    }
    void setValueFromGUI(const int &f) override
    {
        p->value = f;
        editor.uiToAudio.push({Synth::UIToAudioMsg::Action::SET_PARAM, pid, static_cast<float>(f)});
        editor.flushOperator();

        if (onGuiSetValue)
            onGuiSetValue();
    }
    void setValueFromModel(const int &f) override { p->value = f; }
    int getDefaultValue() const override
    {
        return static_cast<int>(std::round(p->meta.defaultVal));
    }
    int getMin() const override { return static_cast<int>(std::round(p->meta.minVal)); }
    int getMax() const override { return static_cast<int>(std::round(p->meta.maxVal)); }
};

template <typename P, typename T, typename Q, typename... Args>
void createComponent(SixSinesEditor &e, P &panel, const Param &parm, std::unique_ptr<T> &cm,
                     std::unique_ptr<Q> &pc, Args... args)
{
    auto id = parm.meta.id;
    pc = std::make_unique<Q>(e, id);
    cm = std::make_unique<T>();

    if constexpr (std::is_same_v<Q, PatchContinuous> &&
                  !std::is_same_v<T, jcmp::DraggableTextEditableValue>) // hack
    {
        cm->onPopupMenu = [&e, ptr = cm.get()](auto &mods)
        {
            e.hideTooltip();
            e.popupMenuForContinuous(ptr);
        };
    }
    cm->onBeginEdit = [&e, &cm, &pc, args..., id, &panel]()
    {
        e.uiToAudio.push({Synth::UIToAudioMsg::Action::BEGIN_EDIT, id});
        if (std::is_same_v<Q, PatchContinuous>)
        {
            e.showTooltipOn(cm.get());
            e.updateTooltip(pc.get());
        }

        panel.beginEdit(args...);
    };
    cm->onEndEdit = [&e, id, &panel]()
    {
        e.uiToAudio.push({Synth::UIToAudioMsg::Action::END_EDIT, id});
        if (std::is_same_v<Q, PatchContinuous>)
        {
            e.hideTooltip();
        }
    };
    cm->setSource(pc.get());

    e.componentByID[id] = juce::Component::SafePointer<juce::Component>(cm.get());
}

} // namespace baconpaul::six_sines::ui
#endif // PATCH_CONTINUOUS_H
