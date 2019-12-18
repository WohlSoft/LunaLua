#ifndef NO_SDL

#include "MusicManager.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include <math.h>
#include "../libs/ini-reader/INIReader.h"


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


ChunkEntry MusicManager::sounds[91];
MusicEntry MusicManager::music_lvl[57];
MusicEntry MusicManager::music_wld[16];
MusicEntry MusicManager::music_spc[4];

std::unordered_map<std::string, musicFile > MusicManager::registredFiles;
std::unordered_map<std::string, chunkFile > MusicManager::chunksBuffer;

std::string MusicManager::defaultSndINI="";
std::string MusicManager::defaultMusINI="";

std::string MusicManager::curRoot="";


void MusicManager::initAudioEngine()
{
    bool firstRun = !PGE_SDL_Manager::isInit;
    PGE_SDL_Manager::initSDL();
    if(firstRun)
    {
        initArrays();
        defaultSndINI=PGE_SDL_Manager::appPath+"sounds.ini";
        defaultMusINI=PGE_SDL_Manager::appPath+"music.ini";
        loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
        loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
        rebuildSoundCache();
    }
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
                //Play it!
                sounds[chanID].play();
            }
        }
    } else {
        if (!seizedSections[curSection])
        {
            if(alias=="smusic") {
                music_spc[0].play();
            } else if(alias=="stmusic") {
                music_spc[1].play();
            } else if(alias=="tmusic") {
                music_spc[2].play();
            } else if(alias.substr(0, 6) == "wmusic") {
                std::string musIDs = alias.substr(6);
                int musID = std::atoi(musIDs.c_str()) - 1;
                if(musID>=0 && musID<16)
                {
                    music_wld[musID].play();
                }
            } else if(alias.substr(0, 5) == "music") {
                std::string musIDs = alias.substr(5);
                int musID = std::atoi(musIDs.c_str()) - 1;
                if(musID>=0 && musID<56)
                {
                    music_lvl[musID].play();
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
            pausedNatively = false;
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
    if( !file_existsX(path) ) return;

    INIReader SoundsIni( path );
    if (SoundsIni.ParseError() < 0)
    {
        MessageBoxA(0, std::string(path+"\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

    curRoot=root;
    for(int i=0; i<91; i++)
    {
        std::string head = "sound-"+i2str(i+1);
        std::string fileName;
        std::string reserveChannel;

        fileName = SoundsIni.Get(head, "file", "");
        if(fileName.size()==0) continue;

        reserveChannel = SoundsIni.Get(head, "single-channel", "0");

        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");

        if( file_existsX(root+fileName) )
        {
            sounds[i].setPath(root+fileName.c_str());
            if(reserveChannel=="1")
                sounds[i].channel = 0;
            else
                sounds[i].channel = -1;
        }
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
    if( !file_existsX(path) ) return;

    INIReader MusicIni( path );
    if (MusicIni.ParseError() < 0)
    {
        MessageBoxA(0, std::string(path+"\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

    curRoot=root;
    int i=0;

    //World music
    for(int j=1; (j<=16) && (i<74); i++, j++)
    {
        std::string head = "world-music-"+i2str(j);
        std::string fileName;

        fileName = MusicIni.Get(head, "file", "");
        if(fileName.size()==0) continue;
        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");
        if (file_existsX(root + clearTrackNumber(fileName) ))
        {
            music_wld[j-1].setPath(root+fileName.c_str());
        }
    }

    //Special music
    for(int j=1; (j<=3) && (i<74); i++, j++)
    {
        std::string head = "special-music-"+i2str(j);
        std::string fileName;

        fileName = MusicIni.Get(head, "file", "");
        if(fileName.size()==0) continue;
        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");
        if (file_existsX(root + clearTrackNumber(fileName)))
        {
            music_spc[j-1].setPath(root+fileName.c_str());
        }
    }

    //Level music
    for(int j=1; (j<=56) && (i<74); i++, j++)
    {
        if(j==24) j++;
        std::string head = "level-music-"+i2str(j);
        std::string fileName;

        fileName = MusicIni.Get(head, "file", "");
        if(fileName.size()==0) continue;
        replaceSubStr(fileName, "\"", "");
        replaceSubStr(fileName, "\\\\",  "\\");
        replaceSubStr(fileName, "/",  "\\");
        if (file_existsX(root + clearTrackNumber(fileName)))
        {
            music_lvl[j-1].setPath(root+fileName.c_str());
        }
    }
}

void MusicManager::loadCustomSounds(std::string episodePath, std::string levelCustomPath)
{
    initArrays();
    loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
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

#endif

