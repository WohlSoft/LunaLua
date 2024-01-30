#ifndef NO_SDL

#include "MusicManager.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include <math.h>
#include <IniProcessor/ini_processing.h>


ChunkEntry::ChunkEntry()
{
    id=0;
    chunk=NULL;
    needReload=true;
    fullPath = "";
    channel = -1;
}

ChunkEntry::~ChunkEntry()
{
    if (chunk)
    {
        PGE_Sounds::memUsage -= chunk->alen;
        Mix_FreeChunk(chunk);
    }
    chunk=NULL;
}

void ChunkEntry::setPath(std::string path)
{
    if(fullPath!=path)
    {
        needReload=true;
        fullPath=path;
    }
}

bool ChunkEntry::doLoad()
{
    if(needReload)
    {
        if (chunk)
        {
            PGE_Sounds::memUsage -= chunk->alen;
            Mix_FreeChunk(chunk);
        }
        chunk = Mix_LoadWAV( fullPath.c_str() );
        if (chunk)
        {
            PGE_Sounds::memUsage += chunk->alen;
        }
        return (bool)chunk;
    }
    else
    {
        return true;
    }
}

bool ChunkEntry::play()
{
    if (channel != -1)
        Mix_HaltChannel(channel);
    if(!chunk)
        return false;
    return (Mix_PlayChannelTimedVolume(channel, chunk, 0, -1, MIX_MAX_VOLUME) != -1);
}


MusicEntry::MusicEntry()
{
    id = 0;
    type = 0;
    fullPath = "";
}

MusicEntry::~MusicEntry()
{}

void MusicEntry::setPath(std::string path)
{
    fullPath = path;
}

void MusicEntry::play()
{
    PGE_MusPlayer::MUS_openFile(fullPath.c_str());
    PGE_MusPlayer::MUS_playMusic();
}

CustomSoundEntry::CustomSoundEntry()
{
    fullPath = "";
    chunk = NULL;
}

CustomSoundEntry::~CustomSoundEntry()
{}


ChunkEntry MusicManager::sounds[91];
MusicEntry MusicManager::music_lvl[57];
MusicEntry MusicManager::music_wld[16];
MusicEntry MusicManager::music_spc[4];

std::unordered_map<std::string, musicFile > MusicManager::registredFiles;
std::unordered_map<std::string, chunkFile > MusicManager::chunksBuffer;

std::string MusicManager::defaultSndINI="";
std::string MusicManager::defaultMusINI="";

std::string MusicManager::curRoot="";

bool MusicManager::setToChangeMusicAlias = false;
std::string MusicManager::curMusicAlias = "";
int MusicManager::currentMusicID = -1;

std::string MusicManager::curSfxAlias = "";
int MusicManager::currentSfxID = 0;
int MusicManager::sfxTimer = 0;

int MusicManager::fullCustomSFXCount = 0;
CustomSoundEntry* MusicManager::custom_sfxs = NULL;


void MusicManager::initAudioEngine()
{
    bool firstRun = !PGE_SDL_Manager::isInit;
    PGE_SDL_Manager::initSDL();
    if(firstRun)
    {
        initArrays();
        defaultSndINI=PGE_SDL_Manager::appPath+"sounds.ini";
        defaultMusINI=PGE_SDL_Manager::appPath+"music.ini";
        loadSounds(defaultSndINI, PGE_SDL_Manager::appPath + "sound\\");
        loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
        rebuildSoundCache();
    }
}

void MusicManager::setMusicAlias(std::string alias)
{
    curMusicAlias = alias;
}

void MusicManager::setSfxAlias(std::string alias)
{
    curSfxAlias = alias;
}


void MusicManager::rebuildSoundCache()
{
    // Reinit reserved channels list
    int numberOfReservedChannels = 0;

    // For sounds which are failing to load
    std::string failedSounds;
    int countOfFailedSounds = 0;
    constexpr static int MaxFailedSoundToDisplay = 15;

    for(int i=0; i<91; i++)
    {
        if(sounds[i].channel != -1)
        {
            sounds[i].channel = numberOfReservedChannels++;
        }
        if(sounds[i].needReload)
        {
            if(!sounds[i].doLoad())
            {
                countOfFailedSounds++;
                if(countOfFailedSounds > MaxFailedSoundToDisplay)
                    continue;
                failedSounds += " " + sounds[i].fullPath + "\n";
            }
        }
    }
    Mix_AllocateChannels(numberOfReservedChannels + 32);
    Mix_ReserveChannels(numberOfReservedChannels);

    if (countOfFailedSounds > MaxFailedSoundToDisplay)
    {
        failedSounds += "And " + std::to_string(countOfFailedSounds - MaxFailedSoundToDisplay) + " more sounds...\n";
    }

    if(!failedSounds.empty())
    {
        std::string errorMsg = "Some audio files failed to load:\n" + failedSounds;
        MessageBoxA(0, errorMsg.c_str(), "Errors while loading sound files", MB_OK | MB_ICONWARNING);
    }
}


//!
//! \brief This function accepts MCI-alias and file path. Function used to poke initialization of sound engine and bind custom musics sent by SMBX Engine.
//! \param alias MCI-Alias sent by SMBX Engine
//! \param fileName File path sent by SMBX Engine
//!
void MusicManager::addSound(std::string alias, std::string fileName)
{
    initAudioEngine();

    //Load custom music
    if(alias=="music24") {
        //clear junk
        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");
        music_lvl[23].setPath(fileName);
    }
}



void MusicManager::close()
{
    PGE_MusPlayer::MUS_stopMusic();
}


bool MusicManager::seizedSections[21] =
                { false, false, false, false, false, false, false,
                  false, false, false, false, false, false, false,
                  false, false, false, false, false, false, false};
bool MusicManager::pausedNatively=false;
int MusicManager::curSection=0;
// Music volume overrider. -1 to use default behavior, 0~128 - enforce specific music volume
int MusicManager::musicVolume = -1;

//Music stream seizing
void MusicManager::resetSeizes()
{
    for (int i = 0; i<21; i++)
        seizedSections[i] = false;
}

void MusicManager::setSeized(int section, bool state)
{
    if (section>=21) return;
    if (section<-1) return;

    if (section == -1)
    {
        for (int i = 0; i < 21;i++)
            seizedSections[i] = state;
    }
    else
        seizedSections[section] = state;
}

void MusicManager::setCurrentSection(int section)
{
    if (section>21) return;
    if (section<0) return;

    curSection = section;
}

void MusicManager::play(std::string alias) //Chunk will be played once, stream will be played with loop
{
    bool isChunk = alias.substr(0, 5) == "sound";
    if (isChunk)
    {
        std::string chanIDs = alias.substr(5);
        int chanID = std::atoi(chanIDs.c_str()) - 1;
        //Detect out-of-bounds chanID
        if((chanID >= 0)&&(chanID <91))
        {
            if(!PGE_Sounds::playOverrideForAlias(alias, sounds[chanID].channel))
            {
                bool cancelled = createSFXStartLuaEvent(chanID, sounds[chanID].fullPath);
                if(!cancelled)
                {
                    //Play it!
                    setSfxAlias(alias);
                    currentSfxID = chanID;
                    sfxTimer = 2;
                    sounds[chanID].play();
                }
            }
            else
            {
                setSfxAlias(alias);
                currentSfxID = chanID;
                sfxTimer = 2;
            }
        }
    } else {
        if (!seizedSections[curSection])
        {
            if(!setToChangeMusicAlias)
            {
                setMusicAlias(alias);
            }
            if(alias=="smusic") {
                currentMusicID = 1;
                if(!PGE_MusPlayer::playOverrideForMusicAlias(alias))
                {
                    music_spc[0].play();
                }
            } else if(alias=="stmusic") {
                currentMusicID = 2;
                if(!PGE_MusPlayer::playOverrideForMusicAlias(alias))
                {
                    music_spc[1].play();
                }
            } else if(alias=="tmusic") {
                currentMusicID = 3;
                if(!PGE_MusPlayer::playOverrideForMusicAlias(alias))
                {
                    music_spc[2].play();
                }
            } else if(alias.substr(0, 6) == "wmusic") {
                std::string musIDs = alias.substr(6);
                int musID = std::atoi(musIDs.c_str()) - 1;
                currentMusicID = musID;
                if(musID>=0 && musID<16)
                {
                    if(!PGE_MusPlayer::playOverrideForMusicAlias(alias))
                    {
                        music_wld[musID].play();
                    }
                }
            } else if(alias.substr(0, 5) == "music") {
                std::string musIDs = alias.substr(5);
                int musID = std::atoi(musIDs.c_str()) - 1;
                currentMusicID = musID;
                if(musID>=0 && musID<57)
                {
                    if(!PGE_MusPlayer::playOverrideForMusicAlias(alias))
                    {
                        music_lvl[musID].play();
                    }
                }
            }
            pausedNatively = false;
        }
        else if(pausedNatively)
        {
            PGE_MusPlayer::MUS_playMusic();
            pausedNatively = false;
        }
    }
}


void MusicManager::pause()
{
    if(!PGE_MusPlayer::MUS_IsPaused())
    {//Pause if it was NOT paused
        PGE_MusPlayer::MUS_pauseMusic();
        pausedNatively = true;
    }
}

void MusicManager::stop(std::string alias)
{
    bool isChunk = alias.substr(0, 5) == "sound";
    if (isChunk)
    {
        std::string chanIDs = alias.substr(5);
        int chanID = std::atoi(chanIDs.c_str()) - 1;
        //Detect out-of-bounds chanID
        if((chanID < 0) || (chanID >= 91))
        {
            chanID = 0;
        } else {
            //Mute it!
            if(sounds[chanID].channel>0)
                Mix_HaltChannel(sounds[chanID].channel);
        }
    } else {
        if(!seizedSections[curSection])
        {
            PGE_MusPlayer::MUS_stopMusic();
            currentMusicID = 0;
            pausedNatively = false;
        }
    }
}

std::string MusicManager::getCurrentMusic()
{
    bool isSpecial = (curMusicAlias == "smusic" || curMusicAlias == "stmusic" || curMusicAlias == "tmusic");
    bool isLevel = (curMusicAlias.substr(0, 5) == "music");
    bool isOverworld = (curMusicAlias.substr(0, 6) == "wmusic");

    if(isSpecial)
    {
        return music_spc[currentMusicID].fullPath;
    }
    else if(isOverworld)
    {
        return music_wld[currentMusicID].fullPath;
    }
    else if(isLevel)
    {
        return music_lvl[currentMusicID].fullPath;
    }
    else
    {
        return "";
    }
}

int MusicManager::getCurrentSfx()
{
    return currentSfxID;
}

void MusicManager::update()
{
    bool isChunk = curSfxAlias.substr(0, 5) == "sound";
    if(isChunk && currentSfxID > 0)
    {
        if(sfxTimer > 0)
        {
            sfxTimer--;
        }
        if(sfxTimer == 0)
        {
            currentSfxID = 0;
            curSfxAlias = "";
        }
    }
    if(setToChangeMusicAlias)
    {
        if (!seizedSections[curSection])
        {
            int substrateCount = 0;
            std::string chanIDs = "";
            int chanID = 0;

            bool isSpecial = (curMusicAlias == "smusic" || curMusicAlias == "stmusic" || curMusicAlias == "tmusic");
            bool isLevel = (curMusicAlias.substr(0, 5) == "music");
            bool isOverworld = (curMusicAlias.substr(0, 6) == "wmusic");

            if(isLevel)
            {
                // get the music ID
                substrateCount = 5;
                chanIDs = curMusicAlias.substr(substrateCount);
                chanID = std::atoi(chanIDs.c_str()) - 1;
            }
            else if(isOverworld)
            {
                // get the world ID
                substrateCount = 6;

                chanIDs = curMusicAlias.substr(substrateCount);
                chanID = std::atoi(chanIDs.c_str()) - 1;
            }
            else
            {
                // restart the music regardless of section
                stop(curMusicAlias);
                play(curMusicAlias);
                setToChangeMusicAlias = false;
            }
            
            if(!isSpecial)
            {
                // restart the music that is currently being played
                if(chanID == currentMusicID)
                {
                    stop(curMusicAlias);
                    play(curMusicAlias);
                    setToChangeMusicAlias = false;
                }
            }
        }
    }
}

void MusicManager::setVolume(int _volume)
{
    if (!seizedSections[curSection])
    {
        if(MusicManager::musicVolume < 0) // Use built-in music volume behavior
        {
            double piece = ((double)_volume / 1000.0);
            int converted = (int)floor((piece*128.0) + 0.5);
            PGE_MusPlayer::MUS_changeVolume(converted);
        }
        else
        {
            PGE_MusPlayer::MUS_changeVolume(MusicManager::musicVolume);
        }
    }
}

void MusicManager::setVolumeOverride(int _volume)
{
    MusicManager::musicVolume = _volume;
}


std::string MusicManager::lenght()
{
    return "52:12:11:12";
}

std::string MusicManager::position()
{
    std::string t="00:04:12:45";
    return t;
}

void MusicManager::loadSounds(std::string path, std::string root)
{
    if(!file_existsX(path))
        return;

    IniProcessing soundsList(path);
    if(!soundsList.isOpened())
    {
        MessageBoxA(0, std::string(path + "\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

    curRoot = root;
    for(int i = 0; i < 91; i++)
    {
        HandleEventsWhileLoading();

        std::string head = "sound-"+i2str(i+1);
        std::string fileName;
        int reserveChannel;

        if(!soundsList.beginGroup(head))
            continue;
        soundsList.read("file", fileName, "");
        
        if(fileName.size() == 0)
        {
            soundsList.endGroup();
            continue;
        }

        soundsList.read("single-channel", reserveChannel, 0);

        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");

        // If no extension...
        size_t findLastSlash = fileName.find_last_of("/\\");
        size_t findLastDot = fileName.find_last_of(".", findLastSlash);

        // Append missing extension
        if (findLastDot == std::wstring::npos)
        {
            static const char* extensionOptions[] = { ".ogg", ".mp3", ".wav", ".voc", ".flac", ".spc" };
            for (int j=0; j < (sizeof(extensionOptions) / sizeof(extensionOptions[0])); j++)
            {
                std::string possibleName = fileName + extensionOptions[j];
                if (file_existsX(root + possibleName))
                {
                    fileName = possibleName;
                    break;
                }
            }
        }

        if(file_existsX(root + fileName))
        {
            sounds[i].setPath(root + fileName.c_str());
            if(reserveChannel != 0)
                sounds[i].channel = 0;
            else
                sounds[i].channel = -1;
        }

        soundsList.endGroup();
    }
}


static std::string clearTrackNumber(std::string in)
{
    unsigned int i = 0;
    for (; i < in.size(); i++)
    {
        if (in[i] == '|')
            break;
    }
    if (i == in.size()) return in;//Not found
    in.resize(i, ' ');
    return in;
}

void MusicManager::loadMusics(std::string path, std::string root)
{
    if(!file_existsX(path))
        return;

    IniProcessing musicList(path);
    if (!musicList.isOpened())
    {
        MessageBoxA(0, std::string(path + "\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

    curRoot = root;
    int i = 0;

    //World music
    for(int j = 1; (j <= 16) && (i < 74); i++, j++)
    {
        std::string head = "world-music-" + i2str(j);
        std::string fileName;

        if(!musicList.beginGroup(head))
            continue; // Group doesn't exist
        musicList.read("file", fileName, "");
        musicList.endGroup();

        if(fileName.size() == 0)
            continue;

        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");

        if (file_existsX(root + clearTrackNumber(fileName) ))
        {
            music_wld[j-1].setPath(root + fileName);
        }
    }

    //Special music
    for(int j = 1; (j <= 3) && (i < 74); i++, j++)
    {
        std::string head = "special-music-"+i2str(j);
        std::string fileName;

        if(!musicList.beginGroup(head))
            continue; // Group doesn't exist
        musicList.read("file", fileName, "");
        musicList.endGroup();

        if(fileName.size() == 0)
            continue;

        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");

        if (file_existsX(root + clearTrackNumber(fileName)))
        {
            music_spc[j-1].setPath(root + fileName);
        }
    }

    //Level music
    for(int j=1; (j<=56) && (i<74); i++, j++)
    {
        if(j==24) j++;
        std::string head = "level-music-"+i2str(j);
        std::string fileName;

        if(!musicList.beginGroup(head))
            continue; // Group doesn't exist
        musicList.read("file", fileName, "");
        musicList.endGroup();

        if(fileName.size() == 0)
            continue;

        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");

        if (file_existsX(root + clearTrackNumber(fileName)))
        {
            music_lvl[j-1].setPath(root + fileName);
        }
    }
}

void MusicManager::loadCustomSounds(std::string episodePath, std::string levelCustomPath)
{
    initArrays();
    loadSounds(defaultSndINI, PGE_SDL_Manager::appPath + "sound\\");
    loadSounds(episodePath+"\\sounds.ini", episodePath);
    if(!levelCustomPath.empty())
        loadSounds(levelCustomPath+"\\sounds.ini", levelCustomPath);
    loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
    loadMusics(episodePath+"\\music.ini", episodePath);
    if(!levelCustomPath.empty())
        loadMusics(levelCustomPath+"\\music.ini", levelCustomPath);
    rebuildSoundCache();
}


void MusicManager::resetSoundsToDefault()
{
    initArrays();
    loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
    loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
    rebuildSoundCache();
}


void MusicManager::initArrays()
{
    curRoot = PGE_SDL_Manager::appPath;
    for(int i=0; i<91; i++)
    {
        sounds[i].id=i+1;
        sounds[i].setPath(PGE_SDL_Manager::appPath+defaultChunksList[i]);
        sounds[i].channel=chunksChannelsList[i];
    }
    for(int i=0, j=0, k=MusicEntry::MUS_WORLD; i<74; i++, j++)
    {
        switch(k)
        {
        case MusicEntry::MUS_WORLD:
            music_wld[j].type=k;
            music_wld[j].id=j+1;
            music_wld[j].setPath(PGE_SDL_Manager::appPath+defaultMusList[i]);
            if(j>=15)
            {
                j=0; j++;
            }
            break;
        case MusicEntry::MUS_SPECIAL:
            music_spc[j].type=k;
            music_spc[j].id=j+1;
            music_spc[j].setPath(PGE_SDL_Manager::appPath+defaultMusList[i]);
            if(j>=3)
            {
                j=0; j++;
            }
            break;
        case MusicEntry::MUS_LEVEL:
            music_spc[j].type=k;
            music_spc[j].id=j+1;
            music_spc[j].setPath(PGE_SDL_Manager::appPath+defaultMusList[i]);
            break;
        }
    }
}

std::string MusicManager::SndRoot()
{
    return curRoot;
}

Mix_Chunk *MusicManager::getChunkForAlias(const std::string& alias)
{
    bool isChunk = alias.substr(0, 5) == "sound";
    if (isChunk)
    {
        std::string chanIDs = alias.substr(5);
        int chanID = std::atoi(chanIDs.c_str()) - 1;
        //Detect out-of-bounds chanID
        if((chanID >= 0)&&(chanID <91))
        {
            return sounds[chanID].chunk;
        }
    }
    return nullptr;
}

std::string MusicManager::getMusicForAlias(const std::string& alias, int type)
{
    if(alias != "")
    {
        bool isMusic = false;

        int startingPoint = 0;
        int startAt = 0;
        int endAt = 0;
        int betweenCount = 0;
        int foundID = 0;

        std::string typeMusic = "";
        int specialIdx = 0;

        if(type == 3) // level music, from Lua
        {
            startingPoint = 5;
            isMusic = alias.substr(0, startingPoint) == "music";
            typeMusic = "music";
        }
        else if(type == 2) // world music, from Lua
        {
            startingPoint = 6;
            isMusic = alias.substr(0, startingPoint) == "wmusic";
            typeMusic = "wmusic";
        }
        else if(type == 1) // special music, from Lua
        {
            if(alias == "smusic")
            {
                startingPoint = 6;
                isMusic = true;
                typeMusic = "smusic";
            }
            if(alias == "stmusic")
            {
                startingPoint = 7;
                isMusic = true;
                typeMusic = "stmusic";
            }
            if(alias == "tmusic")
            {
                startingPoint = 6;
                isMusic = true;
                typeMusic = "tmusic";
            }
        }
        
        bool isSpecial = (typeMusic == "smusic" || typeMusic == "stmusic" || typeMusic == "tmusic");

        if (isMusic)
        {
            if(typeMusic == "music")
            {
                startAt = 19;
                endAt = 75;
            }
            else if(typeMusic == "wmusic")
            {
                startAt = 0;
                endAt = 15;
            }
            
            if(typeMusic == "smusic")
            {
                startAt = 16;
                specialIdx = 0;
            }
            else if(typeMusic == "stmusic")
            {
                startAt = 17;
                specialIdx = 1;
            }
            else if(typeMusic == "tmusic")
            {
                startAt = 18;
                specialIdx = 2;
            }
            
            if(typeMusic != "" && !isSpecial)
            {
                for(int i = startAt; i < endAt; i++)
                {
                    betweenCount = betweenCount + 1;
                    std::size_t exactChanID = musAliasesList[i].find(alias);
                    if(exactChanID!=std::string::npos)
                    {
                        foundID = i;
                        break;
                    }
                    else
                    {
                        foundID = -1;
                        betweenCount = -1;
                        break;
                    }
                }
            }

            //Detect out-of-bounds chanID
            if(betweenCount > 0 && typeMusic == "music" && !isSpecial)
            {
                return music_lvl[betweenCount - 1].fullPath;
            }
            else if(betweenCount > 0 && isSpecial)
            {
                return music_spc[specialIdx].fullPath;
            }
            else if(betweenCount > 0 && typeMusic == "wmusic" && !isSpecial)
            {
                return music_wld[betweenCount - 1].fullPath;
            }
        }
        return "";
    }
    else
    {
        return "";
    }
}

#endif

