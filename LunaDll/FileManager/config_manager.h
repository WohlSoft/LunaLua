#pragma once
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

extern std::string g_ApplicationPath;

class ConfigPackMiniManager
{
public:
    typedef std::vector<std::string> StringList;
    ConfigPackMiniManager();
    ~ConfigPackMiniManager() {}

    struct ConfigEntry
    {
        uint64_t id;
        std::string extra_settings_filename;
    };

    struct ConfigStore
    {
        std::string extra_settings_root;
        std::string setup_root;
        std::unordered_map<uint64_t, ConfigEntry> data;
    };

    /**
     * @brief Initialize config pack's directory tree
     * @param config_dir path to config pack's root directory
     */
    void loadConfigPack(const std::string &config_dir);

    void loadStore(ConfigStore &dst,
                   const std::string &file,
                   const std::string &hive_head,
                   const std::string &item_head);

    void setEpisodePath(const std::string &episode_path);
    void setCustomPath(const std::string &custom_path);

private:
    ConfigStore             m_blocks;
    ConfigStore             m_bgo;
    ConfigStore             m_npc;

    bool                    m_is_using;
    std::string             m_cp_root_path;
    std::string             m_episode_path;
    std::string             m_custom_path;
};

#endif // CONFIGMANAGER_H
