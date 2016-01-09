#ifndef NO_SDL


#ifndef SDL_MUS_PLAYER
#define SDL_MUS_PLAYER

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main
#include <map>
#include <string>
#include <atomic>

class PGE_SDL_Manager
{
public:
	static bool isInit;
	static void initSDL();
	static std::string appPath;
};


class PGE_MusPlayer
{
public:
	static std::string currentTrack;
    static void MUS_playMusic();
	static void MUS_playMusicFadeIn(int ms);
	static void MUS_pauseMusic();
	static void MUS_stopMusic();
	static void MUS_stopMusicFadeOut(int ms);
	static std::string MUS_MusicTitle();
	static std::string MUS_MusicTitleTag();
    static std::string MUS_MusicArtistTag();
	static std::string MUS_MusicAlbumTag();
    static std::string MUS_MusicCopyrightTag();

    static void MUS_changeVolume(int vlm);
    static void MUS_openFile(const char *musFile);
    static void setSampleRate(int sampleRate);
    static int sampleRate();
    static int currentVolume();

	static bool MUS_IsPlaying();
    static bool MUS_IsPaused();
    static bool MUS_IsFading();

	static unsigned __int64 sampleCount();
    static unsigned __int64 MUS_sampleCount();
private:
    static Mix_Music *play_mus;
    static int volume;
    static int sRate;
	static bool showMsg;
	static std::string showMsg_for;
	
	static std::atomic<unsigned __int64> sCount;
	static std::atomic<unsigned __int64> musSCount;
	static void postMixCallback(void *udata, Uint8 *stream, int len);
};


class PGE_Sounds
{
public:
    static void SND_PlaySnd(const char *sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *SND_OpenSnd(const char *sndFile);
private:
	static std::map<std::string, Mix_Chunk* > chunksBuffer;
    static Mix_Chunk *sound;
    static char *current;
};

#endif

#endif
