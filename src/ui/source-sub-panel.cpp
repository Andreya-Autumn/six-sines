/*
 * Six Sines
 *
 * A synth with audio rate modulation.
 *
 * Copyright 2024, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work will be
 * released under GPL3.
 *
 * The source code and license are at https://github.com/baconpaul/six-sines
 */

#include "source-sub-panel.h"
#include "patch-data-bindings.h"
#include "ui-constants.h"

namespace baconpaul::six_sines::ui
{
SourceSubPanel::SourceSubPanel(SixSinesEditor &e) : HasEditor(e) { setSelectedIndex(0); };
SourceSubPanel::~SourceSubPanel() {}

void SourceSubPanel::setSelectedIndex(size_t idx)
{
    index = idx;
    repaint();

    removeAllChildren();

    auto &sn = editor.patchCopy.sourceNodes[idx];
    setupDAHDSR(editor, sn);
    setupLFO(editor, sn);

    createComponent(editor, *this, sn.envToRatio.meta.id, envToRatio, envToRatioD);
    envToRatioL = std::make_unique<jcmp::Label>();
    envToRatioL->setText("Env Ratio Mul");
    addAndMakeVisible(*envToRatioL);
    addAndMakeVisible(*envToRatio);

    createComponent(editor, *this, sn.lfoToRatio.meta.id, lfoToRatio, lfoToRatioD);
    addAndMakeVisible(*lfoToRatio);
    lfoToRatioL = std::make_unique<jcmp::Label>();
    lfoToRatioL->setText("Depth");
    addAndMakeVisible(*lfoToRatioL);

    resized();
}

void SourceSubPanel::resized()
{
    auto p = getLocalBounds().reduced(uicMargin, 0);
    auto pn = layoutDAHDSRAt(p.getX(), p.getY());
    positionKnobAndLabel(pn.getRight() + uicMargin, pn.getY(), envToRatio, envToRatioL);

    auto r = layoutLFOAt(pn.getX() + 2 * uicMargin + uicKnobSize, p.getY());
    positionKnobAndLabel(r.getX() + uicMargin, r.getY(), lfoToRatio, lfoToRatioL);
}
} // namespace baconpaul::six_sines::ui