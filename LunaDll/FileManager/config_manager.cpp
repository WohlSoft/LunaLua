#include "config_manager.h"
#include <IniProcessor/ini_processing.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <algorithm>
#include <fmt/fmt_format.h>
#include <SDL2/SDL.h>
#include "../Misc/ResourceFileMapper.h"
#include "../GlobalFuncs.h"
#ifdef UNIT_TEST
#include <stdio.h>
#endif

std::string g_ApplicationPath = "./";

static void addSlashToTail(std::string &str)
{
    if(str.empty())
        return;

    if((str.back() != '/') && (str.back() != '\\'))
        str.push_back('/');
}

static void removeDoubleSlash(std::string &dir)
{
    std::string dirN;
    dirN.reserve(dir.size());
    char c_prev = 0;
    for(char c : dir)
    {
        if(c != c_prev)
            dirN.push_back(c);
        c_prev = (c == '/') ? c : 0;
    }
    dir = dirN;
}

static std::string dump_file(const std::string &path)
{
    SDL_RWops *f = SDL_RWFromFile(path.c_str(), "r");
    std::string out;
    if(!f)
        return std::string();

    Sint64 f_size = SDL_RWsize(f);

    if(f_size < 0)
    {
        SDL_RWclose(f);
        return std::string();
    }

    out.resize(static_cast<size_t>(f_size));

    if(SDL_RWread(f, &out[0], 1, static_cast<size_t>(f_size)) != static_cast<size_t>(f_size))
    {
        SDL_RWclose(f);
        return std::string();
    }

    SDL_RWclose(f);
    return out;
}

ConfigPackMiniManager::ConfigPackMiniManager() :
    m_cp_files(),
    m_episode_files(),
    m_custom_files(),
    m_blocks(),
    m_bgo(),
    m_npc(),
    m_sections_ex_fileinfo(),
    m_level_ex_fileinfo(),
    m_sections_ex(),
    m_level_ex(),
    m_is_using(false),
    m_cp_root_path(),
    m_episode_path(),
    m_custom_path()
{}

void ConfigPackMiniManager::loadConfigPack(const std::string &config_dir)
{
    // Note: Don't need to clear m_blocks/m_bgo/m_npc/m_sections_ex/m_level_ex normally because loadStore will now clear out anything old

    if (config_dir.empty())
    {
        m_blocks.clear();
        m_bgo.clear();
        m_npc.clear();
        m_sections_ex.clear();
        m_level_ex.clear();
        return;
    }

    DirMan confDir(config_dir);

    if (!confDir.exists())
    {
        m_blocks.clear();
        m_bgo.clear();
        m_npc.clear();
        m_sections_ex.clear();
        m_level_ex.clear();
        return;
    }

    m_cp_root_path = confDir.absolutePath() + "/";

    // Get config pack files
    m_cp_files = gCachedFileMetadata.listResourceFilesFromDir(Str2WStr(m_cp_root_path));

    // Get episode files
    m_episode_files = gCachedFileMetadata.listResourceFilesFromDir(Str2WStr(m_episode_path));

    // Get custom files
    m_custom_files = gCachedFileMetadata.listResourceFilesFromDir(Str2WStr(m_custom_path));

    ResourceFileInfo sections_file = getGlobalExtraSettingsFile(X_SECTIONS);
    if (sections_file != m_sections_ex_fileinfo)
    {
        if (sections_file.done)
        {
            m_sections_ex_fileinfo = sections_file;
            loadExtraSettings(m_sections_ex, WStr2Str(sections_file.path));
        }
        else
        {
            m_sections_ex_fileinfo = ResourceFileInfo();
            m_sections_ex.clear();
        }
    }

    ResourceFileInfo levelfile_file = getGlobalExtraSettingsFile(X_LEVELFILE);
    if (levelfile_file != m_level_ex_fileinfo)
    {
        if (levelfile_file.done)
        {
            m_level_ex_fileinfo = levelfile_file;
            loadExtraSettings(m_level_ex, WStr2Str(levelfile_file.path));
        }
        else
        {
            m_level_ex_fileinfo = ResourceFileInfo();
            m_level_ex.clear();
        }
    }

    loadStore(BLOCKS, m_blocks, m_cp_root_path + "lvl_blocks.ini", "blocks-main", "block");
    loadStore(BGO, m_bgo, m_cp_root_path + "lvl_bgo.ini", "background-main", "background");
    loadStore(NPC, m_npc, m_cp_root_path + "lvl_npc.ini", "npc-main", "npc");
}

void ConfigPackMiniManager::loadStore(EntryType type,
                                      ConfigPackMiniManager::ConfigStore &dst,
                                      const std::string &file,
                                      const std::string &hive_head,
                                      const std::string &item_head)
{
    IniProcessing store_set(file);
    size_t total;

#ifdef UNIT_TEST
    printf("Loading store %s:\n", file.c_str());
#endif

    store_set.beginGroup(hive_head);
    store_set.read("total", total, 0);
    store_set.read("config-dir", dst.setup_root, "items");
    store_set.read("extra-settings", dst.extra_settings_root, dst.setup_root);
    store_set.endGroup();

#ifdef UNIT_TEST
    printf("-- Total: %zu\n", total);
    printf("-- Config dir: %s\n", dst.setup_root.c_str());
    printf("-- Extra settings root: %s\n", dst.extra_settings_root.c_str());
    fflush(stdout);
#endif

    dst.setup_root = m_cp_root_path + dst.setup_root;
    addSlashToTail(dst.setup_root);
    removeDoubleSlash(dst.setup_root);

    dst.extra_settings_root = m_cp_root_path + dst.extra_settings_root;
    addSlashToTail(dst.extra_settings_root);
    removeDoubleSlash(dst.extra_settings_root);

    // Get file list for setup_root
    dst.setup_files = gCachedFileMetadata.listResourceFilesFromDir(Str2WStr(dst.setup_root));

    // Get file list for extra_settings_root
    if (dst.setup_root == dst.extra_settings_root)
    {
        // If the directory is the same, just copy the map
        dst.extra_settings_files = dst.setup_files;
    }
    else
    {
        dst.extra_settings_files = gCachedFileMetadata.listResourceFilesFromDir(Str2WStr(dst.extra_settings_root));
    }

    // Load global seta settings for this
    ResourceFileInfo global_layout_file = getGlobalExtraSettingsFile(type);
    if (global_layout_file != dst.default_global_extra_settings_fileinfo)
    {
        dst.default_global_extra_settings_fileinfo = global_layout_file;
        if (global_layout_file.done)
        {
            loadExtraSettings(dst.default_global_extra_settings, WStr2Str(global_layout_file.path));
        }
        else
        {
            dst.default_global_extra_settings.clear();
        }
    }

    dst.data.resize(total);
    for(size_t it = 1; it <= total; it++)
    {
        HandleEventsWhileLoading();

        std::string fname = fmt::format("{0}-{1}.ini", item_head, it);
        std::wstring wfname = Str2WStr(fname);

        // idx 0=setup, 1=episode, 2=custom
        ResourceFileInfo iniFiles[3];

        auto&& setup_files_it = dst.setup_files.find(wfname);
        if (setup_files_it != dst.setup_files.end())
        {
            iniFiles[0] = setup_files_it->second;
        }

        auto&& episode_files_it = m_episode_files.find(wfname);
        if (episode_files_it != m_episode_files.end())
        {
            iniFiles[1] = episode_files_it->second;
        }

        auto&& custom_files_it = m_custom_files.find(wfname);
        if (custom_files_it != m_custom_files.end())
        {
            iniFiles[2] = custom_files_it->second;
        }

        // If the file information for the ini files has changed, read the ini files
        ConfigEntry& e = dst.data[it-1];
        if ((e.id != it) ||
            (e.ini_fileinfo[0] != iniFiles[0]) ||
            (e.ini_fileinfo[1] != iniFiles[1]) ||
            (e.ini_fileinfo[2] != iniFiles[2]))
        {
            e.id = it;
            e.ini_fileinfo[0] = iniFiles[0];
            e.ini_fileinfo[1] = iniFiles[1];
            e.ini_fileinfo[2] = iniFiles[2];
            e.extra_settings_filename = std::string();

            for (int fileIdx = 0; fileIdx < 3; fileIdx++)
            {
                ResourceFileInfo& file = iniFiles[fileIdx];
                if (!file.done) continue;
                IniProcessing item_set(WStr2Str(file.path));

                if (!item_set.beginGroup(item_head))
                    item_set.beginGroup("General");

                item_set.read("extra-settings", e.extra_settings_filename, e.extra_settings_filename);
#ifdef UNIT_TEST
                if (!e.extra_settings_filename.empty())
                {
                    printf("-- Extra settins filename %s found in %s\n", e.extra_settings_filename.c_str(), f.c_str());
                    fflush(stdout);
                }
#endif
                item_set.endGroup();
            }
        }

        if(!e.extra_settings_filename.empty())
        {
            ResourceFileInfo file;
            if (e.extra_settings_filename.find_first_of("/\\") != std::string::npos)
            {
                // If the filename for the JSON contains a slash, we can't rely on our pre-obtained directory listings
                file = findFileInSubfolder(e.extra_settings_filename, dst.extra_settings_root);
            }
            else
            {
                // Otherwise, we can use our directory listing data
                file = findFile(e.extra_settings_filename, dst.extra_settings_files);
            }
            // If the file to load extra settings has changed, load it
            if (file != e.extra_settings_fileinfo)
            {
#ifdef UNIT_TEST
                printf("-- Trying to figure out [%s]\n", path.c_str());
                fflush(stdout);
#endif
                e.extra_settings_fileinfo = file;
                e.default_extra_settings.clear();
                if (file.done)
                {
                    loadExtraSettings(e.default_extra_settings, WStr2Str(file.path));
                }
            }
        }
        else
        {
            // If there is no file, make sure data is cleared
            e.extra_settings_fileinfo = ResourceFileInfo();
            e.default_extra_settings.clear();
        }
    }
}

void ConfigPackMiniManager::setEpisodePath(const std::string &episode_path)
{
    m_episode_path = episode_path;
    addSlashToTail(m_episode_path);
    removeDoubleSlash(m_episode_path);
#ifdef UNIT_TEST
    printf("Episode path: %s\n", m_episode_path.c_str());
    fflush(stdout);
#endif
}

void ConfigPackMiniManager::setCustomPath(const std::string &custom_path)
{
    m_custom_path = custom_path;
    addSlashToTail(m_custom_path);
    removeDoubleSlash(m_custom_path);
#ifdef UNIT_TEST
    printf("Custom path: %s\n", m_custom_path.c_str());
    fflush(stdout);
#endif
}

ResourceFileInfo ConfigPackMiniManager::getGlobalExtraSettingsFile(ConfigPackMiniManager::EntryType type)
{
    switch (type)
    {
    case BLOCKS:
    {
        return findFile("global_block.json", m_cp_files);
    }

    case BGO:
    {
        return findFile("global_bgo.json", m_cp_files);
    }

    case NPC:
    {
        return findFile("global_npc.json", m_cp_files);
    }

    case X_SECTIONS:
    {
        return findFile("lvl_section.json", m_cp_files);
    }

    case X_LEVELFILE:
    {
        return findFile("lvl_settings.json", m_cp_files);
    }

    default:
        return ResourceFileInfo();
    }
}


static void mergeJsonSettingsCB(nlohmann::json &dst, nlohmann::json &src)
{
    if(!src.is_object())
        return;
    for(auto it = src.begin(); it != src.end(); it++)
    {
        if(it->is_object())
        {
            mergeJsonSettingsCB(dst[it.key()], *it);
            continue;
        }
        dst[it.key()] = *it;
    }
}


static std::string mergeJsonSettings(const nlohmann::json &dst, const std::string &src, bool beautify = false)
{
    nlohmann::json res = dst;

    try {
        nlohmann::json j_src = nlohmann::json::parse(src);
        mergeJsonSettingsCB(res, j_src);
    } catch(...) {
        if(beautify)
            return res.dump(4, ' ');
        else
            return res.dump();
    }

    if(beautify)
        return res.dump(4, ' ');
    else
        return res.dump();
}

static std::string mergeJsonSettingsLG(const nlohmann::json &global,
                                       const nlohmann::json &local,
                                       const std::string &src, bool beautify = false)
{
    nlohmann::json res;
    res["local"] = local;
    res["global"] = global;

    try {
        nlohmann::json j_src = nlohmann::json::parse(src);
        if(j_src.find("local") != j_src.end())
        {
            mergeJsonSettingsCB(res["local"], j_src["local"]);
        }
        else
        {
            nlohmann::json j_src_c = j_src;
            if(j_src_c.find("global") != j_src_c.end())
                j_src_c.erase("global");
            mergeJsonSettingsCB(res["local"], j_src_c);
        }
        if(j_src.find("global") != j_src.end())
            mergeJsonSettingsCB(res["global"], j_src["global"]);
    } catch(...) {
        if(beautify)
            return res.dump(4, ' ');
        else
            return res.dump();
    }

    if(beautify)
        return res.dump(4, ' ');
    else
        return res.dump();
}


std::string ConfigPackMiniManager::mergeGlobalExtraSettings(ConfigPackMiniManager::EntryType type,
                                                            const std::string &input,
                                                            bool beautify)
{
    switch (type)
    {
    case BLOCKS:
        return mergeJsonSettings(m_blocks.default_global_extra_settings, input, beautify);
    case BGO:
        return mergeJsonSettings(m_bgo.default_global_extra_settings, input, beautify);
    case NPC:
        return mergeJsonSettings(m_npc.default_global_extra_settings, input, beautify);
    default:
        return std::string();
    }
}

std::string ConfigPackMiniManager::mergeExtraSettings(ConfigPackMiniManager::EntryType type,
                                                      size_t id,
                                                      const std::string &input,
                                                      bool beautify)
{
    switch (type)
    {
    case BLOCKS:
    {
        if ((id >= 1) && (id <= m_blocks.data.size()))
        {
            return mergeJsonSettingsLG(m_blocks.default_global_extra_settings,
                m_blocks.data[id - 1].default_extra_settings,
                input,
                beautify);
        }
        return std::string(); // Should this be 'input'?
    }

    case BGO:
    {
        if ((id >= 1) && (id <= m_bgo.data.size()))
        {
            return mergeJsonSettingsLG(m_bgo.default_global_extra_settings,
                m_bgo.data[id - 1].default_extra_settings,
                input,
                beautify);
        }
        return std::string(); // Should this be 'input'?
    }

    case NPC:
    {
        if ((id >= 1) && (id <= m_npc.data.size()))
        {
            return mergeJsonSettingsLG(m_npc.default_global_extra_settings,
                m_npc.data[id - 1].default_extra_settings,
                input,
                beautify);
        }
        return std::string(); // Should this be 'input'?
    }

    case X_SECTIONS:
    {
        return mergeJsonSettings(m_sections_ex, input, beautify);
    }

    case X_LEVELFILE:
    {
        return mergeJsonSettings(m_level_ex, input, beautify);
    }

    default:
        return std::string();
    }
}

ResourceFileInfo ConfigPackMiniManager::findFile(const std::string &filename, const ResourceFileMap &root_files)
{
    std::wstring wFilename = Str2WStr(filename);

    auto&& custom_it = m_custom_files.find(wFilename);
    if (custom_it != m_custom_files.end())
        return custom_it->second;

    auto&& episode_it = m_episode_files.find(wFilename);
    if (episode_it != m_episode_files.end())
        return episode_it->second;

    auto&& cp_it = root_files.find(wFilename);
    if (cp_it != root_files.end())
        return cp_it->second;

    return ResourceFileInfo();
}

ResourceFileInfo ConfigPackMiniManager::findFileInSubfolder(const std::string &filename, const std::string &root_path)
{
    ResourceFileInfo info;

    info = gCachedFileMetadata.getResourceFileInfo(Str2WStr(m_custom_path + filename));
    if (info.done)
        return info;

    info = gCachedFileMetadata.getResourceFileInfo(Str2WStr(m_episode_path + filename));
    if (info.done)
        return info;

    info = gCachedFileMetadata.getResourceFileInfo(Str2WStr(root_path + filename));
    return info;
}

static void append_type_entry(nlohmann::json &typetree,
                              nlohmann::json path,
                              const std::string &type,
                              const std::string &value)
{
    path.push_back(value);
    if(typetree.find(type) == typetree.end())
        typetree[type] = nlohmann::json::array();
    typetree[type].push_back(path);
}

static void make_list_defaults_numeric_list(nlohmann::json& dst, nlohmann::json& src) {
    for (auto& kvp : src.items())
    {
        nlohmann::json entry = nlohmann::json();
        entry["k"] = kvp.key();
        if (kvp.value().is_object() || kvp.value().is_array()) {
            nlohmann::json&& childObj = nlohmann::json();
            make_list_defaults_numeric_list(childObj, kvp.value());
            entry["v"] = childObj;
        }
        else {
            entry["v"] = kvp.value();
        }
        dst.push_back(entry);
    }
}

static void read_layout_branches(nlohmann::json &typetree,
                                 nlohmann::json &dst,
                                 nlohmann::json &src,
                                 nlohmann::json path_arr)
{
    for(auto it = src.begin(); it != src.end(); it++)
    {
        nlohmann::json &entry = *it;
        std::string name;
        std::string control;

        if(entry.find("control") == entry.end())
            control = "invalid";
        else
            control = entry["control"].get<std::string>();

        if(entry.find("name") == entry.end())
            name = control;
        else
            name = entry["name"].get<std::string>();

        if(SDL_strncasecmp(control.c_str(), "group", 6) == 0)
        {
            if(entry.find("children") == entry.end())
                continue; // Invalid entry: missing a required key
            nlohmann::json path_arr_next = path_arr;
            if(name != "..")
            {
                // Create a new named branch and store all children inside
                path_arr_next.push_back(name);
                read_layout_branches(typetree, dst[name], entry["children"], path_arr_next);
            }
            else
            {
                // Store all children in a current branch
                read_layout_branches(typetree, dst, entry["children"], path_arr_next);
            }
        }
        else if(SDL_strncasecmp(control.c_str(), "spinbox", 8) == 0)
        {
            std::string type = "int";
            if(entry.find("type") != entry.end())
                type = entry["type"].get<std::string>();

            if(type == "int")
            {
                int v = 0;
                if(entry.find("value-default") != entry.end())
                    v = entry["value-default"];
                if(entry.find("value-min") != entry.end() &&
                   entry.find("value-max") != entry.end())
                {
                    int min = entry["value-min"];
                    int max = entry["value-max"];
                    if(v < min)
                        v = min;
                    if(v > max)
                        v = max;
                }
                dst[name] = v;
            }
            else if(type == "double")
            {
                double v = 0;
                if(entry.find("value-default") != entry.end())
                    v = entry["value-default"];
                if(entry.find("value-min") != entry.end() &&
                   entry.find("value-max") != entry.end())
                {
                    double min = entry["value-min"];
                    double max = entry["value-max"];
                    if(v < min)
                        v = min;
                    if(v > max)
                        v = max;
                }
                dst[name] = v;
            }
            else
            {
                dst[name] = "<invalid value format>";
                continue; // Invalid type name
            }
        }
        else if(SDL_strncasecmp(control.c_str(), "checkbox", 9) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key
            bool v = entry["value-default"];
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "color", 6) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key
            std::string v = entry["value-default"];
            dst[name] = v;
            append_type_entry(typetree, path_arr, "color", name);
        }
        else if(SDL_strncasecmp(control.c_str(), "combobox", 9) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key
            int v = entry["value-default"];
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "flagbox", 8) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key
            unsigned long v = entry["value-default"];
            dst[name] = v;
            append_type_entry(typetree, path_arr, "flags", name);
        }
        else if(SDL_strncasecmp(control.c_str(), "description", 12) == 0)
        {
            continue; // This entry doesn't ships a valie
        }
        else if(SDL_strncasecmp(control.c_str(), "lineedit", 9) == 0)
        {
            std::string v;
            if(entry.find("value-default") != entry.end())
                v = entry["value-default"].get<std::string>();
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "multilineedit", 14) == 0)
        {
            std::string v;
            if(entry.find("value-default") != entry.end())
                v = entry["value-default"].get<std::string>();
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "file", 5) == 0 ||
                SDL_strncasecmp(control.c_str(), "musicfile", 10) == 0 ||
                SDL_strncasecmp(control.c_str(), "soundfile", 10) == 0 ||
                SDL_strncasecmp(control.c_str(), "levelfile", 10) == 0)
        {
            std::string v;
            if(entry.find("value-default") != entry.end())
                v = entry["value-default"].get<std::string>();
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "itemselect", 11) == 0)
        {
            int v = 0;
            if(entry.find("type") == entry.end())
                continue; // invalid entry: missing required key
            if(entry.find("value-default") != entry.end())
                v = entry["value-default"];
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "sizebox", 8) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key

            std::string type = "int";
            if(entry.find("type") != entry.end())
                type = entry["type"].get<std::string>();

            nlohmann::json v = entry["value-default"];
            if(v.find("w") != v.end() && v.find("h") != v.end())
            {
                if(type == "int")
                {
                    int w = v["w"];
                    int h = v["h"];
                    dst[name]["w"] = w;
                    dst[name]["h"] = h;
                }
                else if(type == "double")
                {
                    double w = v["w"];
                    double h = v["h"];
                    dst[name]["w"] = w;
                    dst[name]["h"] = h;
                }
                else
                {
                    dst[name] = "<invalid value format>";
                    continue; // Invalid type name
                }

                append_type_entry(typetree, path_arr, "size", name);
            }
            else
            {
                dst[name] = "<invalid value format>";
            }
        }
        else if(SDL_strncasecmp(control.c_str(), "pointbox", 9) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key

            std::string type = "int";
            if(entry.find("type") != entry.end())
                type = entry["type"].get<std::string>();

            nlohmann::json v = entry["value-default"];
            if(v.find("x") != v.end() && v.find("y") != v.end())
            {
                if(type == "int")
                {
                    int x = v["x"];
                    int y = v["y"];
                    dst[name]["x"] = x;
                    dst[name]["y"] = y;
                }
                else if(type == "double")
                {
                    double x = v["x"];
                    double y = v["y"];
                    dst[name]["x"] = x;
                    dst[name]["y"] = y;
                }
                else
                {
                    dst[name] = "<invalid value format>";
                    continue; // Invalid type name
                }

                append_type_entry(typetree, path_arr, "point", name);
            }
            else
            {
                dst[name] = "<invalid value format>";
            }
        }
        else if(SDL_strncasecmp(control.c_str(), "rectbox", 8) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key

            std::string type = "int";
            if(entry.find("type") != entry.end())
                type = entry["type"].get<std::string>();

            nlohmann::json v = entry["value-default"];
            if(v.find("x") != v.end() && v.find("y") != v.end() &&
               v.find("w") != v.end() && v.find("w") != v.end())
            {
                if(type == "int")
                {
                    int x = v["x"];
                    int y = v["y"];
                    int w = v["w"];
                    int h = v["h"];
                    dst[name]["x"] = x;
                    dst[name]["y"] = y;
                    dst[name]["w"] = w;
                    dst[name]["h"] = h;
                }
                else if(type == "double")
                {
                    double x = v["x"];
                    double y = v["y"];
                    double w = v["w"];
                    double h = v["h"];
                    dst[name]["x"] = x;
                    dst[name]["y"] = y;
                    dst[name]["w"] = w;
                    dst[name]["h"] = h;
                }
                else
                {
                    dst[name] = "<invalid value format>";
                    continue; // Invalid type name
                }

                append_type_entry(typetree, path_arr, "rect", name);
            }
            else
            {
                dst[name] = "<invalid value format>";
            }
        }
        else if (SDL_strncasecmp(control.c_str(), "listGroup", 9) == 0)
        {
            if (entry.find("children") == entry.end())
                continue; // invalid entry: missing required key

            append_type_entry(typetree, path_arr, "list", name);

            nlohmann::json path_arr_next = path_arr;
            nlohmann::json fields = nlohmann::json();

            // Store the default values for the children
            path_arr_next.push_back(name);
            read_layout_branches(typetree, fields, entry["children"], path_arr_next);

            nlohmann::json fieldsList = nlohmann::json();
            // Prevent a pairs call in the lua code by getting a numeric list here
            make_list_defaults_numeric_list(fieldsList, fields);
            dst[name]["defaults"] = fieldsList;
            dst[name]["count"] = 0;
            dst[name]["items"] = nlohmann::json::object(); // TODO: In a future version, allow for adding some list elements by default.
        }
        else
        {
            dst[name] = "<unknown entry type>";
        }
    }
}

void ConfigPackMiniManager::loadExtraSettings(nlohmann::json &dst, const std::string &path)
{
#ifdef UNIT_TEST
    printf("-- Loading extra settings file %s\n", path.c_str());
    fflush(stdout);
#endif
    dst.clear();

    std::string src = dump_file(path);
    if(!src.empty())
    {
        try
        {
            nlohmann::json layout = nlohmann::json::parse(src);
            auto items = layout.find("layout");
            if(items->is_array())
            {
                if(dst.find("__type") == dst.end())
                    dst["__type"] = nlohmann::json();
                nlohmann::json path_arr = nlohmann::json::array();
                read_layout_branches(dst["__type"], dst, *items, path_arr);
            }

#ifdef UNIT_TEST
            printf("-- Parsed content of length %zu, %s\n", src.size(), dst.is_object() ? "is object" : "odd crap");
            std::string out_tree = dst.dump(4, ' ');
            printf("-- Generated a tree:%s\n", out_tree.c_str());
            fflush(stdout);
#endif
        }
        catch(const nlohmann::json::parse_error &e)
        {
            (void)e;
#ifdef UNIT_TEST
            printf("-- ERROR: fail to parse: %s\n", e.what());
            fflush(stdout);
#endif
        }
        catch(const nlohmann::json::exception &e)
        {
            (void)e;
#ifdef UNIT_TEST
            printf("-- ERROR: exception: %s\n", e.what());
            fflush(stdout);
#endif
        }

    }
#ifdef UNIT_TEST
    else
    {
        printf("-- failed to read: blank output\n");
        fflush(stdout);
    }
#endif
}
