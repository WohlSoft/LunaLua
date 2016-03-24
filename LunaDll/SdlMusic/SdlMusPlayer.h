#ifndef NO_SDL


#ifndef SDL_MUS_PLAYER
#define SDL_MUS_PLAYER

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main
#include <map>
#include <string>
#include <atomic>
#include <vector>
#include <functional>
#include <mutex>

class PGE_PostMixFunc {
public:
	std::function<void(void *udata, uint8_t *stream, int len)> func;
	void *userdata;
	PGE_PostMixFunc();
	PGE_PostMixFunc(std::function<void(void *udata, uint8_t *stream, int len)>& func);
};

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
	static int sampleFormat();
	static int channels();

	static bool MUS_IsPlaying();
    static bool MUS_IsPaused();
    static bool MUS_IsFading();

	static unsigned __int64 sampleCount();
    static unsigned __int64 MUS_sampleCount();
	static void addPostMixFunc(PGE_PostMixFunc* func);
	static bool removePostMixFunc(PGE_PostMixFunc* func);

	
private:
    static Mix_Music *play_mus;
	static int volume;
	static int sRate;
	static int chunkSize;
	static int _sampleFormat;
	static int _channels;

	static bool showMsg;
	static std::string showMsg_for;
	
	static std::atomic<unsigned __int64> sCount;
	static std::atomic<unsigned __int64> musSCount;
	static void postMixCallback(void *udata, Uint8 *stream, int len);
	static std::vector<PGE_PostMixFunc*> postMixFuncSet;
	static std::mutex mtx1;
};


class PGE_Sounds
{
public:
    static void SND_PlaySnd(const char *sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *SND_OpenSnd(const char *sndFile);
    static bool playOverrideForAlias(const std::string& alias, int ch);
    static void setOverrideForAlias(const std::string& alias, Mix_Chunk* chunk);
    static Mix_Chunk *getChunkForAlias(const std::string& alias);
    static void setMuteForAlias(const std::string& alias, bool muted);
    static bool getMuteForAlias(const std::string& alias);
private:
    struct ChunkOverrideSettings {
        Mix_Chunk* chunk;
        bool muted;
    };
	static std::map<std::string, Mix_Chunk* > chunksBuffer;
    static Mix_Chunk *sound;
    static char *current;
    static bool overrideArrayIsUsed;
    static std::map<std::string, ChunkOverrideSettings > overrideSettings;
};

#endif

#endif
