#ifndef NO_SDL

#ifndef MusicManager_HHHH
#define MusicManager_HHHH

#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include "SdlMusPlayer.h"

typedef std::pair<int, std::string> musicFile;
typedef std::pair<int, Mix_Chunk* > chunkFile;

struct ChunkEntry 
{
    ChunkEntry();
    ~ChunkEntry();

    void setPath(std::string path);
    bool doLoad();
    bool play();

    int         id;
    Mix_Chunk*  chunk;
    bool        needReload;
    std::string fullPath;
    int         channel;
};

struct MusicEntry 
{
    MusicEntry();
    ~MusicEntry();
    void setPath(std::string path);
    void play();
    enum MusType 
    {
        MUS_WORLD='w',
        MUS_SPECIAL='s',
        MUS_LEVEL='l'
    };
    int     id;
    char    type;
    std::string fullPath;
};

class MusicManager
{
public:
    enum types
    {
        Stream=0,
        Chunk
    };
    static void initAudioEngine();
    static void rebuildSoundCache();
    static void addSound(std::string alias, std::string fileName);
    static void close();
    static void play(std::string alias); //Chunk will be played once, stream will be played with loop
    static void pause();
    static void stop(std::string alias);
    static void setVolume(int _volume);
    static void setVolumeOverride(int _volume);
    static std::string lenght();
    static std::string position();
    
    static void loadCustomSounds(std::string episodePath, std::string levelCustomPath="");
    static void resetSoundsToDefault();
    static void loadSounds(std::string path, std::string root);
    static void loadMusics(std::string path, std::string root, bool is_first_run);
    static std::string SndRoot();

    static void resetSeizes();
    static void setSeized(int section, bool state);
    static void setCurrentSection(int section);

    static Mix_Chunk *getChunkForAlias(const std::string& alias);

private:
    static std::unordered_map<std::string, musicFile > registredFiles;
    static std::unordered_map<std::string, chunkFile > chunksBuffer;

    static std::string curRoot;//Current rood directory (episode or application dir)

    //Musics
    static int max_lvl_music_id; // Size of level music array
    static int max_wld_music_id; // Size of world music array
    static MusicEntry *music_lvl;
    static MusicEntry *music_wld;
    static MusicEntry music_spc[4];
    static const int defaultMusCount = 75; // Total number of world + level + special music in smbx 1.3
    static const int defaultMusCountLvl = 56; // smbx 1.3 special music count
    static const int defaultMusCountWld = 16; // smbx 1.3 special music count
    static const int defaultMusCountSpc = 3;  // smbx 1.3 special music count
    static std::string defaultMusList[defaultMusCount];//List of system default files
    static std::string musAliasesList[defaultMusCount];//List of reserved aliases for sound effects

    static void resizeMusicArrays(int new_max_lvl_music_id, int new_max_wld_music_id); // Change size of music ids
    static void initArraysMusic();//Populate music array based on default values

    //SFX
    static ChunkEntry sounds[91];
    static std::string defaultChunksList[91];//List of system default files
    static std::string chunksAliasesList[91];//List of reserved aliases for sound effects
    static int chunksChannelsList[91];//List of channel reservation by some files (-1 is allowing mixed playback)

    static void initArraysSound();//Populate sound array based on default values
    
    //INI Paths
    static std::string defaultSndINI;//Full path to global sounds.ini file
    static std::string defaultMusINI;//Full path to global music.ini file

    //Music stream seizing
    static bool seizedSections[21];
    static bool pausedNatively;
    static int curSection;
    static int musicVolume;
};
#endif


#endif
