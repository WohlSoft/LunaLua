#ifndef NO_SDL

#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "SdlMusPlayer.h"
#include "MusicManager.h"

/***********************************PGE_SDL_Manager********************************************/
bool PGE_SDL_Manager::isInit=false;
std::string PGE_SDL_Manager::appPath="";

void PGE_SDL_Manager::initSDL()
{
    if(!isInit)
    {
        SDL_Init(SDL_INIT_AUDIO);
        isInit=true;
        PGE_MusPlayer::setSampleRate(44100);
        PGE_MusPlayer::MUS_changeVolume(80);
        //std::wstring smbxPath = gAppPathWCHAR;
        //smbxPath = smbxPath.append(L"\\");
        appPath = gAppPathUTF8;
        appPath.append("\\");
    }
}



/***********************************PGE_MusPlayer********************************************/
Mix_Music *PGE_MusPlayer::play_mus = NULL;
std::string PGE_MusPlayer::currentTrack="";
int PGE_MusPlayer::sRate=44100;
bool PGE_MusPlayer::showMsg=true;
std::string PGE_MusPlayer::showMsg_for="";
std::atomic<unsigned __int64> PGE_MusPlayer::sCount = 0;
std::atomic<unsigned __int64> PGE_MusPlayer::musSCount = 0;

Mix_Music *PGE_MusPlayer::currentMusic()
{
    return PGE_MusPlayer::play_mus;
}

void PGE_MusPlayer::MUS_playMusic()
{
    if(!PGE_SDL_Manager::isInit) return;
    if(play_mus)
    {
        if (Mix_PlayingMusic() == 0)
        {
            // Reset music sample count
            musSCount.store(0);

            Mix_PlayMusic(play_mus, -1);
        }
        else
        if(Mix_PausedMusic()==1)
        {
            Mix_ResumeMusic();
        }
    }
    else
    {
        //MessageBoxA(0, std::string(std::string("Play nothing:")+std::string(Mix_GetError())).c_str(), "Error", 0);
    }
}

void  PGE_MusPlayer::MUS_playMusicFadeIn(int ms)
{
    if(!PGE_SDL_Manager::isInit) return;

    if(play_mus)
    {
        if(Mix_PausedMusic()==0)
        {
            // Reset music sample count
            musSCount.store(0);

            if(Mix_FadingMusic()!=MIX_FADING_IN)
                if(Mix_FadeInMusic(play_mus, -1, ms)==-1)
                {
                    MessageBoxA(0, std::string(std::string("Mix_FadeInMusic:")+std::string(Mix_GetError())).c_str(), "Error", 0);
                }
        }
        else
            Mix_ResumeMusic();

    }
    else
    {
        MessageBoxA(0, std::string(std::string("Play nothing:")+std::string(Mix_GetError())).c_str(), "Error", 0);
    }
}

void PGE_MusPlayer::MUS_pauseMusic()
{
    if(!PGE_SDL_Manager::isInit) return;
    Mix_PauseMusic();
}

void PGE_MusPlayer::MUS_stopMusic()
{
    if(!PGE_SDL_Manager::isInit) return;
    Mix_HaltMusic();
}

void PGE_MusPlayer::MUS_stopMusicFadeOut(int ms)
{
    if(!PGE_SDL_Manager::isInit) return;
    if(Mix_FadingMusic()!=MIX_FADING_OUT)
        Mix_FadeOutMusic(ms);
}

std::string PGE_MusPlayer::MUS_MusicTitle()
{
    if (!PGE_SDL_Manager::isInit) return "";
    if (play_mus)
        return std::string(Mix_GetMusicTitle(play_mus));
    else
        return std::string("");
}

std::string PGE_MusPlayer::MUS_MusicTitleTag()
{
    if (!PGE_SDL_Manager::isInit) return "";
    if (play_mus)
        return std::string(Mix_GetMusicTitleTag(play_mus));
    else
        return std::string("");
}

std::string PGE_MusPlayer::MUS_MusicArtistTag()
{
    if (!PGE_SDL_Manager::isInit) return "";
    if (play_mus)
        return std::string(Mix_GetMusicArtistTag(play_mus));
    else
        return std::string("");
}

std::string PGE_MusPlayer::MUS_MusicAlbumTag()
{
    if (!PGE_SDL_Manager::isInit) return "";
    if (play_mus)
        return std::string(Mix_GetMusicAlbumTag(play_mus));
    else
        return std::string("");
}

std::string PGE_MusPlayer::MUS_MusicCopyrightTag()
{
    if (!PGE_SDL_Manager::isInit) return "";
    if (play_mus)
        return std::string(Mix_GetMusicCopyrightTag(play_mus));
    else
        return std::string("");
}


void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    Mix_VolumeMusic(vlm);
}

bool PGE_MusPlayer::MUS_IsPlaying()
{
    return (Mix_PlayingMusic()==1);
}
bool PGE_MusPlayer::MUS_IsPaused()
{
    return (Mix_PausedMusic()==1);
}
bool PGE_MusPlayer::MUS_IsFading()
{
    return (Mix_FadingMusic()==1);
}

void PGE_MusPlayer::setSampleRate(int sampleRate=44100)
{
    sRate=sampleRate;
    Mix_CloseAudio();
    Mix_OpenAudio(sRate, AUDIO_S16, 2, 2048);
    Mix_AllocateChannels(32);

    // Reset music sample count
    sCount.store(0);
    musSCount.store(0);
    Mix_SetPostMix(postMixCallback, NULL);
}

int PGE_MusPlayer::sampleRate()
{
    return sRate;
}

int PGE_MusPlayer::MUS_currentVolume()
{
    return Mix_GetVolumeMusicStream(NULL);
}


void PGE_MusPlayer::MUS_openFile(const char *musFile)
{
    PGE_SDL_Manager::initSDL();
    if(currentTrack==std::string(musFile))
    {
        if(Mix_PlayingMusic()==1)
            return;
    }

    if(play_mus!=NULL)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(play_mus);
        play_mus=NULL;
    }

    play_mus = Mix_LoadMUS( musFile );

    if(!play_mus)
    {
        if(showMsg_for!=musFile)
            showMsg=true;
        if(showMsg)
        {
            MessageBoxA(0, std::string(std::string("Mix_LoadMUS: ")
            +std::string(musFile)+"\n"
            +std::string(Mix_GetError())).c_str(), "Error", 0);
            showMsg_for = std::string(musFile);
            showMsg=false;
        }
    }
    else
    {
        currentTrack = std::string(musFile);
        showMsg=true;
    }
}

void PGE_MusPlayer::postMixCallback(void *udata, Uint8 *stream, int len)
{
    // This post mix callback has a simple purpose: count audio samples.
    sCount += len/4;

    // (Approximate) sample count for only when music is playing
    if ((Mix_PlayingMusic() == 1) && (Mix_PausedMusic() == 0))
    {
        musSCount += len/4;
    }
}

unsigned __int64 PGE_MusPlayer::sampleCount()
{
    return sCount;
}

unsigned __int64 PGE_MusPlayer::MUS_sampleCount()
{
    return musSCount;
}

/***********************************PGE_Sounds********************************************/

Mix_Chunk *PGE_Sounds::sound = NULL;
char *PGE_Sounds::current = "";

std::map<std::string, Mix_Chunk* > PGE_Sounds::chunksBuffer;
bool PGE_Sounds::overrideArrayIsUsed=false;
std::map<std::string, PGE_Sounds::ChunkOverrideSettings > PGE_Sounds::overrideSettings;
uint32_t PGE_Sounds::memUsage = 0;

uint32_t PGE_Sounds::GetMemUsage()
{
    return PGE_Sounds::memUsage;
}

Mix_Chunk *PGE_Sounds::SND_OpenSnd(const char *sndFile)
{
    PGE_SDL_Manager::initSDL();
    std::string filePath = sndFile;
    std::map<std::string, Mix_Chunk* >::iterator it = chunksBuffer.find(filePath);
    Mix_Chunk* tmpChunk = NULL;
    if(it == chunksBuffer.end())
    {
        tmpChunk = Mix_LoadWAV( sndFile );
        if(!tmpChunk) {
            MessageBoxA(0, std::string(std::string("OpenSFX: Mix_LoadWAV: ")
            +std::string(sndFile)+"\n"
            +std::string(Mix_GetError())).c_str(), "Error", 0);
        }

        PGE_Sounds::memUsage += tmpChunk->alen;
        chunksBuffer[filePath] = tmpChunk;
    }
    else
    {
        tmpChunk = chunksBuffer[filePath];
    }

    return tmpChunk;
}

void PGE_Sounds::SND_PlaySnd(const char *sndFile)
{
    PGE_SDL_Manager::initSDL();
    std::string filePath = sndFile;
    std::map<std::string, Mix_Chunk* >::iterator it = chunksBuffer.find(filePath);
    if(it == chunksBuffer.end())
    {
        sound = Mix_LoadWAV( sndFile );
        if(!sound) {
            MessageBoxA(0, std::string(std::string("Mix_LoadWAV: ")
            +std::string(sndFile)+"\n"
            +std::string(Mix_GetError())).c_str(), "Error", 0);
        }

        PGE_Sounds::memUsage += sound->alen;
        chunksBuffer[filePath] = sound;
        if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
        {
            if (std::string(Mix_GetError()) != "No free channels available")//Don't show overflow messagebox
            MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
        }
    }
    else
    {
        if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
        {
            if (std::string(Mix_GetError()) != "No free channels available")//Don't show overflow messagebox
            MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
        }
    }
}

void PGE_Sounds::clearSoundBuffer()
{
    Mix_HaltChannel(-1);
    overrideSettings.clear();
    overrideArrayIsUsed=false;
    for (std::map<std::string, Mix_Chunk* >::iterator it=chunksBuffer.begin(); it!=chunksBuffer.end(); ++it)
    {
        PGE_Sounds::memUsage -= it->second->alen;
        Mix_FreeChunk(it->second);
    }
    chunksBuffer.clear();
}

void PGE_Sounds::setOverrideForAlias(const std::string& alias, Mix_Chunk* chunk)
{
    ChunkOverrideSettings settings = { nullptr, false };
    if(overrideArrayIsUsed)
    {
        auto it = overrideSettings.find(alias);
        if (it != overrideSettings.end())
        {
            settings = it->second;
        }
    }
    settings.chunk = chunk;
    overrideSettings[alias] = settings;
    overrideArrayIsUsed=true;
}

Mix_Chunk *PGE_Sounds::getChunkForAlias(const std::string& alias)
{
    if (overrideArrayIsUsed)
    {
        auto it = overrideSettings.find(alias);
        if (it != overrideSettings.end() && it->second.chunk != nullptr)
        {
            return it->second.chunk;
        }
    }
    return MusicManager::getChunkForAlias(alias);
}

bool PGE_Sounds::playOverrideForAlias(const std::string& alias, int ch)
{
    if(!overrideArrayIsUsed)
        return false;//Don't wait if overriding array is empty

    auto it = overrideSettings.find(alias);
    if (it != overrideSettings.end())
    {
        if (it->second.muted) return true;
        if (it->second.chunk == nullptr) return false;

        if (ch != -1)
            Mix_HaltChannel(ch);
        if (Mix_PlayChannelTimed(ch, it->second.chunk, 0, -1) == -1)
        {
            if (std::string(Mix_GetError()) != "No free channels available")//Don't show overflow messagebox
                MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ") + std::string(Mix_GetError())).c_str(), "Error", 0);
        }
        return true;
    }
    return false;
}

void PGE_Sounds::setMuteForAlias(const std::string& alias, bool muted)
{
    ChunkOverrideSettings settings = { nullptr, false };
    if (overrideArrayIsUsed)
    {
        auto it = overrideSettings.find(alias);
        if (it != overrideSettings.end())
        {
            settings = it->second;
        }
    }
    settings.muted = muted;
    overrideSettings[alias] = settings;
    overrideArrayIsUsed = true;
}

bool PGE_Sounds::getMuteForAlias(const std::string& alias)
{
    if(!overrideArrayIsUsed)
        return false;
    auto it = overrideSettings.find(alias);
    if (it != overrideSettings.end())
    {
        return it->second.muted;
    }
    return false;
}

#endif
