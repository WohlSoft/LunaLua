#include "config_manager.h"

#include <IniProcessor/ini_processing.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <algorithm>
#include <fmt/fmt_format.h>
#include <SDL2/SDL.h>
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
    m_is_using(false)
{}

void ConfigPackMiniManager::loadConfigPack(const std::string &config_dir)
{
    if(config_dir.empty())
        return;

    DirMan confDir(config_dir);

    if(!confDir.exists())
        return;

    if(!Files::fileExists(confDir.absolutePath() + "/main.ini"))
        return;

    m_cp_root_path = confDir.absolutePath() + "/";

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

    std::string global_layout_path = getGlobalExtraSettingsFile(type);
    if(!global_layout_path.empty())
        loadExtraSettings(dst.default_global_extra_settings, global_layout_path);

    for(size_t it = 1; it <= total; it++)
    {
        std::vector<std::string> files_to_read;
        std::string fname;

        fname = fmt::format("{0}{1}-{2}.ini", dst.setup_root, item_head, it);
        removeDoubleSlash(fname);
        if(Files::fileExists(fname))
            files_to_read.push_back(fname);

        fname = fmt::format("{0}{1}-{2}.ini", m_episode_path, item_head, it);
        removeDoubleSlash(fname);
        if(Files::fileExists(fname))
            files_to_read.push_back(fname);

        fname = fmt::format("{0}{1}-{2}.ini", m_custom_path, item_head, it);
        removeDoubleSlash(fname);
        if(Files::fileExists(fname))
            files_to_read.push_back(fname);

        ConfigEntry e;
        e.id = it;
        e.extra_settings_filename = std::string();

        for(const std::string &f : files_to_read)
        {
            IniProcessing item_set(f);

            if(!item_set.beginGroup(item_head))
                item_set.beginGroup("General");

            item_set.read("extra-settings", e.extra_settings_filename, e.extra_settings_filename);
#ifdef UNIT_TEST
            if(!e.extra_settings_filename.empty())
            {
                printf("-- Extra settins filename %s found in %s\n", e.extra_settings_filename.c_str(), f.c_str());
                fflush(stdout);
            }
#endif
            item_set.endGroup();
        }

        if(!e.extra_settings_filename.empty())
        {
            std::string path = findFile(e.extra_settings_filename, dst.extra_settings_root);
#ifdef UNIT_TEST
            printf("-- Trying to figure out [%s]\n", path.c_str());
            fflush(stdout);
#endif
            if(!path.empty())
            {
                loadExtraSettings(e.default_extra_settings, path);
            }
        }

        dst.data.insert({e.id, e});
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

std::string ConfigPackMiniManager::getLocalExtraSettingsFile(ConfigPackMiniManager::EntryType type, uint64_t id)
{
    switch (type)
    {
    case BLOCKS:
    {
        auto it = m_blocks.data.find(id);
        if(it == m_blocks.data.end())
            return std::string();
        return findFile(it->second.extra_settings_filename, m_blocks.extra_settings_root);
    }

    case BGO:
    {
        auto it = m_bgo.data.find(id);
        if(it == m_bgo.data.end())
            return std::string();
        return findFile(it->second.extra_settings_filename, m_bgo.extra_settings_root);
    }

    case NPC:
    {
        auto it = m_npc.data.find(id);
        if(it == m_npc.data.end())
            return std::string();
        return findFile(it->second.extra_settings_filename, m_npc.extra_settings_root);
    }

    default:
        return std::string();
    }
}

std::string ConfigPackMiniManager::getGlobalExtraSettingsFile(ConfigPackMiniManager::EntryType type)
{
    switch (type)
    {
    case BLOCKS:
    {
        std::string f = findFile("global_block.json", m_cp_root_path);
        if (Files::fileExists(f))
            return f;
        return std::string();
    }

    case BGO:
    {
        std::string f = findFile("global_bgo.json", m_cp_root_path);
        if (Files::fileExists(f))
            return f;
        return std::string();
    }

    case NPC:
    {
        std::string f = findFile("global_npc.json", m_cp_root_path);
        if (Files::fileExists(f))
            return f;
        return std::string();
    }

    default:
        return std::string();
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


std::string ConfigPackMiniManager::mergeLocalExtraSettings(ConfigPackMiniManager::EntryType type,
                                                           uint64_t id,
                                                           const std::string &input,
                                                           bool beautify)
{
    switch (type)
    {
    case BLOCKS:
    {
        auto it = m_blocks.data.find(id);
        if(it == m_blocks.data.end())
            return std::string();
        return mergeJsonSettings(it->second.default_extra_settings, input, beautify);
    }

    case BGO:
    {
        auto it = m_bgo.data.find(id);
        if(it == m_bgo.data.end())
            return std::string();
        return mergeJsonSettings(it->second.default_extra_settings, input, beautify);
    }

    case NPC:
    {
        auto it = m_npc.data.find(id);
        if(it == m_npc.data.end())
            return std::string();
        return mergeJsonSettings(it->second.default_extra_settings, input, beautify);
    }

    default:
        return std::string();
    }
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
                                                      uint64_t id,
                                                      const std::string &input,
                                                      bool beautify)
{
    switch (type)
    {
    case BLOCKS:
    {
        auto it = m_blocks.data.find(id);
        if(it == m_blocks.data.end())
            return std::string();
        return mergeJsonSettingsLG(m_blocks.default_global_extra_settings,
                                   it->second.default_extra_settings,
                                   input,
                                   beautify);
    }

    case BGO:
    {
        auto it = m_bgo.data.find(id);
        if(it == m_bgo.data.end())
            return std::string();
        return mergeJsonSettingsLG(m_bgo.default_global_extra_settings,
                                   it->second.default_extra_settings,
                                   input, beautify);
    }

    case NPC:
    {
        auto it = m_npc.data.find(id);
        if(it == m_npc.data.end())
            return std::string();
        return mergeJsonSettingsLG(m_npc.default_global_extra_settings,
                                   it->second.default_extra_settings,
                                   input,
                                   beautify);
    }

    default:
        return std::string();
    }
}

std::string ConfigPackMiniManager::findFile(const std::string &fileName, const std::string &root)
{
    if(Files::fileExists(m_custom_path + fileName))
        return m_custom_path + fileName;

    if(Files::fileExists(m_episode_path + fileName))
        return m_episode_path + fileName;

    if(Files::fileExists(root + fileName))
        return root + fileName;

    return std::string();
}

static void read_layout_branches(nlohmann::json &typetree, nlohmann::json &dst, nlohmann::json &src)
{
    for(auto it = src.begin(); it != src.end(); it++)
    {
        nlohmann::json &entry = *it;
        if(entry.find("control") == entry.end())
            continue; // invalid entry: missing required key
        if(entry.find("name") == entry.end())
            continue; // invalid entry: missing required key

        std::string control = entry["control"];
        std::string name = entry["name"];

        if(SDL_strncasecmp(control.c_str(), "group", 6) == 0)
        {
            if(entry.find("children") == entry.end())
                continue; // Invalid entry: missing a required key
            read_layout_branches(typetree[name], dst[name], entry["children"]);
        }
        else if(SDL_strncasecmp(control.c_str(), "spinbox", 8) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key

            std::string type = "int";
            if(entry.find("type") == entry.end())
                type = entry["type"];

            if(type == "int")
            {
                int v = entry["value-default"];
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
                double v = entry["value-default"];
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
            typetree[name] = "color";
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
            typetree[name] = "flags";
        }
        else if(SDL_strncasecmp(control.c_str(), "lineedit", 9) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key
            std::string v = entry["value-default"];
            dst[name] = v;
        }
        else if(SDL_strncasecmp(control.c_str(), "sizebox", 8) == 0)
        {
            if(entry.find("value-default") == entry.end())
                continue; // invalid entry: missing required key

            std::string type = "int";
            if(entry.find("type") != entry.end())
                type = entry["type"];

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

                typetree[name] = "size";
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
                type = entry["type"];

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

                typetree[name] = "point";
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
                type = entry["type"];

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

                typetree[name] = "rect";
            }
            else
            {
                dst[name] = "<invalid value format>";
            }
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
                read_layout_branches(dst["__type"], dst, *items);
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
#ifdef UNIT_TEST
            printf("-- ERROR: fail to parse: %s\n", e.what());
            fflush(stdout);
#endif
        }
        catch(const nlohmann::json::exception &e)
        {
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
