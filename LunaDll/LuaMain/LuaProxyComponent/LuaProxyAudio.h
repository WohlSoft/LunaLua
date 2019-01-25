//#ifdef LuaProxyAudio_H //Temporary disabled, WIP
#ifndef LuaProxyAudio_H
#define LuaProxyAudio_H

#include <string>
#include <memory>
struct Mix_Chunk;
class MciEmulator;

namespace LuaProxy
{
    namespace Audio
    {
        //Music
        void MusicOpen(const std::string& filename);
        void MusicPlay();
        void MusicPlayFadeIn(int ms);
        void MusicStop();
        void MusicStopFadeOut(int ms);
        int  MusicVolume(void);
        void MusicVolume(int vlm);
        void MusicPause();
        void MusicResume();
        bool MusicIsPlaying();
        bool MusicIsPaused();
        bool MusicIsFading();
        std::string MusicTitle();
        std::string MusicTitleTag();
        std::string MusicArtistTag();
        std::string MusicAlbumTag();
        std::string MusicCopyrightTag();
        void    MusicSetPosition(double seconds);
        double  MusicGetPosition();
        double  MusicGetLoopStart();
        double  MusicGetLoopEnd();
        double  MusicGetLoopLength();
        void seizeStream(int section);
        void releaseStream(int section);
        void changeMusic(int section, const std::string &filename);
        void changeMusic(int section, int musicId);
        void changeMusic(int section, const std::string &filename, int fadeInDelayMs);
        void changeMusic(int section, int musicId, int fadeInDelayMs);
        void musicFadeOut(int section, int fadeInDelayMs);

        //SFX
        Mix_Chunk* newMix_Chunk();
        void clearSFXBuffer();
        void playSFX(const std::string& filename);
        Mix_Chunk* SfxOpen(const std::string& filename);
        int SfxPlayCh(int channel, Mix_Chunk* chunk, int loops);
        int SfxPlayChVol(int channel, Mix_Chunk* chunk, int loops, int volume);
        int SfxPlayChTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
        int SfxPlayChTimedVol(int channel, Mix_Chunk *chunk, int loops, int ticks, int volume);
        int SfxFadeInCh(int channel, Mix_Chunk *chunk, int loops, int ms);
        int SfxFadeInChVol(int channel, Mix_Chunk *chunk, int loops, int ms, int volume);
        int SfxFadeInChTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks);
        int SfxFadeInChTimedVol(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks, int volume);
        void SfxPause(int channel);
        void SfxResume(int channel);
        int SfxStop(int channel);
        int SfxExpire(int channel, int ticks);
        int SfxFadeOut(int channel, int ms);
        int SfxIsPlaying(int channel);
        int SfxIsPaused(int channel);
        int SfxIsFading(int channel);
        int SfxVolume(int channel, int vlm);
        int SfxSetPanning(int channel, int left, int right);
        int SfxSetDistance(int channel, int distance);
        int SfxSet3DPosition(int channel, int angle, int distance);
        int SfxReverseStereo(int channel, int flip);

        std::string getSfxPath(const std::string& path);
        void setMciSection(int section);
        void resetMciSections();

        double AudioClock();
        double MusicClock();

        void __setOverrideForAlias(const std::string& alias, Mix_Chunk* chunk);
        Mix_Chunk* __getChunkForAlias(const std::string& alias);
        void __setMuteForAlias(const std::string& alias, bool muted);
        bool __getMuteForAlias(const std::string& alias);

        class PlayingSfxInstance {
        private:
            int mChannel;
            bool mFinished;
        public:
            PlayingSfxInstance(int channel);
            ~PlayingSfxInstance();

            // Callback when the channel is finished
            void OnChannelFinished();

            void Pause();
            void Resume();
            void Stop();
            void Expire(int ms);
            void FadeOut(int ms);
            bool IsPlaying();
            bool IsPaused();
            bool IsFading();
            void Volume(int vlm);
            void SetPanning(uint8_t left, uint8_t right);
            void SetDistance(uint8_t distance);
            void Set3DPosition(int16_t angle, uint8_t distance);
            void SetReverseStereo(bool flip);
        };

        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxPlayObj(Mix_Chunk &chunk, int loops);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxPlayObjVol(Mix_Chunk &chunk, int loops, int volume);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxPlayObjTimed(Mix_Chunk &chunk, int loops, int ticks);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxPlayObjTimedVol(Mix_Chunk &chunk, int loops, int ticks, int volume);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxFadeInObj(Mix_Chunk &chunk, int loops, int ms);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxFadeInObjVol(Mix_Chunk &chunk, int loops, int ms, int volume);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxFadeInObjTimed(Mix_Chunk &chunk, int loops, int ms, int ticks);
        std::shared_ptr<LuaProxy::Audio::PlayingSfxInstance> SfxFadeInObjTimedVol(Mix_Chunk &chunk, int loops, int ms, int ticks, int volume);
    }
}
#endif
