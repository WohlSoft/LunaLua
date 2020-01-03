#pragma once
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <json/json.hpp>

extern std::string g_ApplicationPath;

class ConfigPackMiniManager
{
public:
    typedef std::vector<std::string> StringList;
    ConfigPackMiniManager();
    ~ConfigPackMiniManager() {}

    enum EntryType
    {
        BLOCKS = 1,
        BGO,
        NPC
    };

    struct ConfigEntry
    {
        uint64_t id;
        std::string extra_settings_filename;
        nlohmann::json default_extra_settings;
    };

    struct ConfigStore
    {
        std::string extra_settings_root;
        std::string setup_root;
        nlohmann::json default_global_extra_settings;
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

    std::string getLocalExtraSettingsFile(EntryType type, uint64_t id);
    std::string getGlobalExtraSettingsFile(EntryType type);

    std::string findFile(const std::string &fileName, const std::string &root);

    void loadExtraSettings(nlohmann::json &dst, const std::string &path);

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
