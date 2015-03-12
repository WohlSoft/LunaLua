#ifdef LuaProxyAudio_H //Temporary disabled, WIP
//#ifndef LuaProxyAudio_H
#define LuaProxyAudio_H

#include <SDL2/SDL_mixer.h>
#undef main

namespace LuaProxy
{
	namespace Audio
	{
		//Music
		void MusicOpen(const char *filename);
		void MusicPlay();
		void MusicPlayFadeIn(int ms);
		void MusicStop();
		void MusicStopFadeOut(int ms);
		void MusicVolume(int vlm);
		void MusicPause();
		void MusicResume();
		bool MusicIsPlaying();
		bool MusicIsPaused();
		bool MusicIsFading();
		void seizeStream(int section);
		void releaseStream(int section);

		//SFX
		void clearSFXBuffer();
		void playSFX(const char* filename);
		Mix_Chunk* openSFX(const char* filename);
		int playSFXChannel(int channel, Mix_Chunk* chunk, int loops);
		int playSFXChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
		int FadeInSFXChannel(int channel, Mix_Chunk *chunk, int loops, int ms);
		int FadeInSFXChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks);
		void pauseSFX(int channel);
		void resumeSFX(int channel);
		int stopSFXChannel(int channel);
		int expireSFXChannel(int channel, int ticks);
		int fadeOutSFXChannel(int channel, int ms);
		int sfxIsPlaying(int channel);
		int sfxIsPaused(int channel);
	}
}
#endif