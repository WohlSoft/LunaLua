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
        NPC,
        X_SECTIONS,
        X_LEVELFILE
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
        void clear()
        {
            extra_settings_root.clear();
            setup_root.clear();
            default_global_extra_settings.clear();
            data.clear();
        }
    };

    /**
     * @brief Initialize config pack's directory tree
     * @param config_dir path to config pack's root directory
     */
    void loadConfigPack(const std::string &config_dir);

    void loadStore(EntryType type,
                   ConfigStore &dst,
                   const std::string &file,
                   const std::string &hive_head,
                   const std::string &item_head);

    void setEpisodePath(const std::string &episode_path);
    void setCustomPath(const std::string &custom_path);

    std::string getLocalExtraSettingsFile(EntryType type, uint64_t id);
    std::string getGlobalExtraSettingsFile(EntryType type);

    std::string mergeLocalExtraSettings(EntryType type,
                                        uint64_t id,
                                        const std::string &input,
                                        bool beautify = false);
    std::string mergeGlobalExtraSettings(EntryType type,
                                         const std::string &input,
                                         bool beautify = false);
    std::string mergeExtraSettings(EntryType type,
                                   uint64_t id,
                                   const std::string &input,
                                   bool beautify = false);

    std::string findFile(const std::string &fileName, const std::string &root);

    void loadExtraSettings(nlohmann::json &dst, const std::string &path);

private:
    ConfigStore             m_blocks;
    ConfigStore             m_bgo;
    ConfigStore             m_npc;

    nlohmann::json          m_sections_ex;
    nlohmann::json          m_level_ex;

    bool                    m_is_using;
    std::string             m_cp_root_path;
    std::string             m_episode_path;
    std::string             m_custom_path;
};

#endif // CONFIGMANAGER_H
