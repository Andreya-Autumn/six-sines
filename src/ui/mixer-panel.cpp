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

#include "mixer-panel.h"
#include "mixer-sub-panel.h"
#include "ifm-editor.h"
#include "ui-constants.h"

namespace baconpaul::fm::ui
{
MixerPanel::MixerPanel(IFMEditor &e) : jcmp::NamedPanel("Mixer"), HasEditor(e)
{
    auto &mn = editor.patchCopy.mixerNodes;
    for (auto i = 0U; i < numOps; ++i)
    {
        createComponent(editor, *this, mn[i].level.meta.id, knobs[i], knobsData[i], i);
        knobs[i]->setDrawLabel(false);
        addAndMakeVisible(*knobs[i]);

        createComponent(editor, *this, mn[i].active.meta.id, power[i], powerData[i], i);
        power[i]->setDrawMode(sst::jucegui::components::ToggleButton::DrawMode::GLYPH);
        power[i]->setGlyph(sst::jucegui::components::GlyphPainter::POWER);
        addAndMakeVisible(*power[i]);

        labels[i] = std::make_unique<jcmp::Label>();
        labels[i]->setText("Op " + std::to_string(i + 1) + " Level");
        addAndMakeVisible(*labels[i]);
    }
}
MixerPanel::~MixerPanel() = default;

void MixerPanel::resized()
{

    auto b = getContentArea().reduced(uicMargin, 0);
    auto x = b.getX();
    auto y = b.getY();
    for (auto i = 0U; i < numOps; ++i)
    {
        positionPowerKnobAndLabel(x, y, power[i], knobs[i], labels[i]);
        y += uicLabeledKnobHeight + uicMargin;
    }
}

void MixerPanel::beginEdit(size_t idx)
{
    editor.hideAllSubPanels();
    editor.mixerSubPanel->setIndex(idx);
    editor.mixerSubPanel->setVisible(true);

    editor.singlePanel->setName("Op " + std::to_string(idx + 1) + " Mix");
}

} // namespace baconpaul::fm::ui