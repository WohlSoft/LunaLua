#include "LuaProxyAudio.h"

#include "../../SdlMusic/MusicManager.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../LuaProxy.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../MciEmulator/mciEmulator.h"

#include "../../SMBXInternal/Level.h"

void LuaProxy::Audio::MusicOpen(const char *filename)
{
#ifndef NO_SDL
    std::string full_paths = getSfxPath(std::string(filename));
    PGE_MusPlayer::MUS_openFile(full_paths.c_str());
#endif
}


void LuaProxy::Audio::MusicPlay()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusic();
#endif
}


void LuaProxy::Audio::MusicPlayFadeIn(int ms)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusicFadeIn(ms);
#endif
}


void LuaProxy::Audio::MusicStop()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_stopMusic();
#endif
}


void LuaProxy::Audio::MusicStopFadeOut(int ms)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_stopMusicFadeOut(ms);
#endif
}


void LuaProxy::Audio::MusicVolume(int vlm)
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_changeVolume(vlm);
#endif
}


void LuaProxy::Audio::MusicPause()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_pauseMusic();
#endif
}


void LuaProxy::Audio::MusicResume()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_playMusic();
#endif
}


bool LuaProxy::Audio::MusicIsPlaying()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPlaying();
#else
    return false;
#endif
}


bool LuaProxy::Audio::MusicIsPaused()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPaused();
#endif
}


bool LuaProxy::Audio::MusicIsFading()
{
#ifndef NO_SDL
	return PGE_MusPlayer::MUS_IsFading();
#endif
}

std::string LuaProxy::Audio::MusicTitle()
{
#ifndef NO_SDL
	return PGE_MusPlayer::MUS_MusicTitle();
#endif
}

std::string LuaProxy::Audio::MusicTitleTag()
{
#ifndef NO_SDL
	return PGE_MusPlayer::MUS_MusicTitleTag();
#endif
}

std::string LuaProxy::Audio::MusicArtistTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicArtistTag();
#endif
}

std::string LuaProxy::Audio::MusicAlbumTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicAlbumTag();
#endif
}

std::string LuaProxy::Audio::MusicCopyrightTag()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_MusicCopyrightTag();
#endif
}


void LuaProxy::Audio::seizeStream(int section)
{
	MusicManager::setSeized(section, true);

}

void LuaProxy::Audio::releaseStream(int section)
{
	MusicManager::setSeized(section, false);
}



Mix_Chunk* LuaProxy::Audio::newMix_Chunk()
{
	return NULL;
}



void LuaProxy::Audio::clearSFXBuffer()
{
    PGE_Sounds::clearSoundBuffer();
}


void LuaProxy::Audio::playSFX(const char *filename)
{
#ifndef NO_SDL
    std::string full_paths = getSfxPath(std::string(filename));
    PGE_Sounds::SND_PlaySnd(full_paths.c_str());
#else
    ::LuaProxy::playSFX(filename);
#endif
}


Mix_Chunk *LuaProxy::Audio::SfxOpen(const char *filename)
{
    std::string full_paths = getSfxPath(std::string(filename));
    return PGE_Sounds::SND_OpenSnd(full_paths.c_str());
}


int LuaProxy::Audio::SfxPlayCh(int channel, Mix_Chunk *chunk, int loops)
{
    return Mix_PlayChannel(channel, chunk, loops);
}


int LuaProxy::Audio::SfxPlayChTimed(int channel, Mix_Chunk *chunk, int loops, int ticks)
{
    return Mix_PlayChannelTimed(channel, chunk, loops, ticks);
}


int LuaProxy::Audio::SfxFadeInCh(int channel, Mix_Chunk *chunk, int loops, int ms)
{
    return Mix_FadeInChannel(channel, chunk, loops, ms);
}


int LuaProxy::Audio::SfxFadeInChTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks)
{
    return Mix_FadeInChannelTimed(channel, chunk, loops, ms, ticks);
}


void LuaProxy::Audio::SfxPause(int channel)
{
    Mix_Pause(channel);
}


void LuaProxy::Audio::SfxResume(int channel)
{
    Mix_Resume(channel);
}


int LuaProxy::Audio::SfxStop(int channel)
{
    return Mix_HaltChannel(channel);
}


int LuaProxy::Audio::SfxExpire(int channel, int ticks)
{
    return Mix_ExpireChannel(channel, ticks);
}


int LuaProxy::Audio::SfxFadeOut(int channel, int ms)
{
    return Mix_FadeOutChannel(channel, ms);
}


int LuaProxy::Audio::SfxIsPlaying(int channel)
{
    return Mix_Playing(channel);
}

int LuaProxy::Audio::SfxIsPaused(int channel)
{
    return Mix_Paused(channel);
}



string LuaProxy::Audio::getSfxPath(string filename)
{
    std::string full_paths;
    if(!isAbsolutePath(filename))
    {
        std::wstring world_dir = (std::wstring)GM_FULLDIR;
        std::wstring full_path = world_dir.append(::Level::GetName());
        full_path = removeExtension(full_path);
        full_path = full_path.append(L"\\"); // < path into level folder
        full_paths = wstr2str(full_path) + filename;
    }
    else
    {
        full_paths = filename;
    }
    return full_paths;
}


void LuaProxy::Audio::setMciSection(int section)
{
    MusicManager::setCurrentSection(section);
}


void LuaProxy::Audio::resetMciSections()
{
	MusicManager::resetSeizes();
}


int LuaProxy::Audio::SfxVolume(int channel, int vlm)
{
    return Mix_Volume(channel, vlm);
}


int LuaProxy::Audio::SfxIsFading(int channel)
{
    if(channel<0) channel=1;//Anti-crash protection
    return Mix_FadingChannel(channel);
}


int LuaProxy::Audio::SfxSetPanning(int channel, int left, int right)
{
	return Mix_SetPanning(channel, (Uint8)left, (Uint8)right);
}

int LuaProxy::Audio::SfxSetDistance(int channel, int distance)
{
	return Mix_SetDistance(channel, (Uint8)distance);
}

int LuaProxy::Audio::SfxSet3DPosition(int channel, int angle, int distance)
{
	return Mix_SetPosition(channel, (Sint16)angle, (Uint8)distance);
}

int LuaProxy::Audio::SfxReverseStereo(int channel, int flip)
{
	return Mix_SetReverseStereo(channel, flip);
}

double LuaProxy::Audio::AudioClock()
{
#ifndef NO_SDL
	return ((double)PGE_MusPlayer::sampleCount()) / PGE_MusPlayer::sampleRate();
#else
	return 0;
#endif
}

double LuaProxy::Audio::MusicClock()
{
#ifndef NO_SDL
	return ((double)PGE_MusPlayer::MUS_sampleCount()) / PGE_MusPlayer::sampleRate();
#else
	return 0;
#endif
}
