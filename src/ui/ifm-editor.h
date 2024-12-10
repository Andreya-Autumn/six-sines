/*
 * BaconPaul's FM Atrocity
 *
 * A mess, with FM.
 *
 * Copyright 2024, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work can also be
 * released under GPL3. You know the drill.
 */

#ifndef BACONPAUL_FMTHING_UI_IFM_EDITOR_H
#define BACONPAUL_FMTHING_UI_IFM_EDITOR_H

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/data/Continuous.h>

#include "synth/synth.h"

namespace jcmp = sst::jucegui::components;
namespace jdat = sst::jucegui::data;

namespace baconpaul::fm::ui
{
struct IFMEditor : jcmp::WindowPanel
{
    Patch patchCopy;
    Synth::audioToUIQueue_t &audioToUI;
    Synth::uiToAudioQueue_T &uiToAudio;
    std::function<void()> flushOperator;
    IFMEditor(Synth::audioToUIQueue_t &atou, Synth::uiToAudioQueue_T &utoa,
              std::function<void()> flushOperator);
    virtual ~IFMEditor();

    void resized() override;

    void idle();
    std::unique_ptr<juce::Timer> idleTimer;

    std::unique_ptr<jcmp::NamedPanel> matrixPanel, mixerPanel, singlePanel, sourcesPanel, mainPanel;
};
} // namespace baconpaul::fm::ui
#endif // IFM_EDITOR_H
