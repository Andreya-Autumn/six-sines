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

#include <sstream>
#include <fstream>
#include "sst/plugininfra/paths.h"
#include "preset-manager.h"

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(sixsines_patches);

namespace baconpaul::six_sines::ui
{
PresetManager::PresetManager()
{
    try
    {
        auto userPath = sst::plugininfra::paths::bestDocumentsFolderPathFor("SixSines");
        fs::create_directories(userPath);
        userPatchesPath = userPath / "Patches";
        fs::create_directories(userPatchesPath);
    }
    catch (fs::filesystem_error &e)
    {
        SXSNLOG("Unable to create user dir " << e.what());
    }

    try
    {
        auto fs = cmrc::sixsines_patches::get_filesystem();
        for (const auto &d : fs.iterate_directory(factoryPath))
        {
            SXSNLOG(d.filename());
            if (d.is_directory())
            {
                std::set<std::string> ents;
                for (const auto &p :
                     fs.iterate_directory(std::string() + factoryPath + "/" + d.filename()))
                {
                    ents.insert(p.filename());
                    SXSNLOG("   " << p.filename());
                }
                factoryPatchNames[d.filename()] = ents;
            }
        }
    }
    catch (const std::exception &)
    {
    }
}

void PresetManager::rescanUserPresets() {}
void PresetManager::saveUserPreset(const fs::path &category, const fs::path &name, Patch &patch)
{
    try
    {
        auto dir = userPatchPath / category;
        fs::create_directories(dir);
        auto pt = (dir / name).replace_extension(".sxsnp");
        saveUserPresetDirect(pt, patch);
    }
    catch (fs::filesystem_error &e)
    {
        SXSNLOG(e.what());
    }
}

void PresetManager::saveUserPresetDirect(const fs::path &pt, Patch &patch)
{
    std::ofstream ofs(pt);
    if (ofs.is_open())
    {
        ofs << patch.toState();
    }
    ofs.close();
}

void PresetManager::loadUserPresetDirect(const fs::path &p, Patch &patch)
{
    std::ifstream t(p);
    if (!t.is_open())
        return;
    std::stringstream buffer;
    buffer << t.rdbuf();

    patch.fromState(buffer.str());
}

void PresetManager::loadFactoryPreset(const std::string &cat, const std::string &pat, Patch &patch)
{
    try
    {
        auto fs = cmrc::sixsines_patches::get_filesystem();
        auto f = fs.open(std::string() + factoryPath + "/" + cat + "/" + pat);
        auto pb = std::string(f.begin(), f.end());
        patch.fromState(pb);
    }
    catch (const std::exception &e)
    {
        SXSNLOG(e.what());
    }
}

} // namespace baconpaul::six_sines::ui