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
    static Mix_Music* currentMusic();
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
    static int MUS_currentVolume();

    static bool MUS_IsPlaying();
    static bool MUS_IsPaused();
    static bool MUS_IsFading();

    static unsigned __int64 sampleCount();
    static unsigned __int64 MUS_sampleCount();

private:
    static void MUS_StartDeferring();
    static void MUS_StopDeferring();
public:
    class DeferralLock
    {
    private:
        static unsigned __int64 lockCount;
        bool isLocked;
    public:
        // Note: Not thread safe, though that applies to all of PGE_MusPlayer really, but making special note anyway
        DeferralLock(bool startLocked = false);
        ~DeferralLock();
        void Lock();
        void Unlock();
    };
private:
    static Mix_Music *play_mus;
    static bool deferringMusic;
    static bool musicGotDeferred;
    static int musicDeferredFadeIn;

    static int sRate;
    static bool showMsg;
    static std::string showMsg_for;

    static std::atomic<unsigned __int64> sCount;
    static std::atomic<unsigned __int64> musSCount;
    static void postMixCallback(void *udata, Uint8 *stream, int len);
};


class PGE_Sounds
{
private:
    static std::string lastError;
public:
    static const char* SND_getLastError() { return lastError.c_str(); }
    static bool SND_PlaySnd(const char *sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *SND_OpenSnd(const char *sndFile);
    static bool playOverrideForAlias(const std::string& alias, int ch);
    static void setOverrideForAlias(const std::string& alias, Mix_Chunk* chunk);
    static Mix_Chunk *getChunkForAlias(const std::string& alias);
    static void setMuteForAlias(const std::string& alias, bool muted);
    static bool getMuteForAlias(const std::string& alias);
public:
    static uint32_t GetMemUsage();
private:
    struct ChunkOverrideSettings {
        Mix_Chunk* chunk;
        bool muted;
    };
    static std::map<std::string, Mix_Chunk* > chunksBuffer;
    static char *current;
    static bool overrideArrayIsUsed;
    static std::map<std::string, ChunkOverrideSettings > overrideSettings;
public:
    static uint32_t memUsage;
};

#endif

#endif
