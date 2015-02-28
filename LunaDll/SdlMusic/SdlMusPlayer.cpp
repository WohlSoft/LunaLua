#ifndef NO_SDL

#include "../GlobalFuncs.h"
#include "SdlMusPlayer.h"

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

		std::wstring smbxPath = getModulePath();
		smbxPath = smbxPath.append(L"\\");
		appPath = wstr2str(smbxPath);
	}
}



/***********************************PGE_MusPlayer********************************************/
Mix_Music *PGE_MusPlayer::play_mus = NULL;
std::string PGE_MusPlayer::currentTrack="";
int PGE_MusPlayer::volume=100;
int PGE_MusPlayer::sRate=44100;
bool PGE_MusPlayer::showMsg=true;
std::string PGE_MusPlayer::showMsg_for="";

void PGE_MusPlayer::MUS_playMusic()
{
	if(!PGE_SDL_Manager::isInit) return;
	if(play_mus)
	{
		if(Mix_PlayingMusic()==0)
		{
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
			if(Mix_FadingMusic()!=MIX_FADING_IN)
				if(Mix_FadeInMusic(play_mus, -1, ms)==-1)
				{
					if(std::string(Mix_GetError())!="No free channels available")//Don't show overflow messagebox
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


void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    volume = vlm;
    Mix_VolumeMusic(volume);
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
    Mix_OpenAudio(sRate, AUDIO_S16, 2, 4096);
	Mix_AllocateChannels(32);
}

int PGE_MusPlayer::sampleRate()
{
    return sRate;
}

int PGE_MusPlayer::currentVolume()
{
    return volume;
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



/***********************************PGE_Sounds********************************************/

Mix_Chunk *PGE_Sounds::sound = NULL;
char *PGE_Sounds::current = "";

std::map<std::string, Mix_Chunk* > PGE_Sounds::chunksBuffer;

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

		chunksBuffer[filePath] = sound;
		if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
		{
			MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
		}
	}
	else
	{
		if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
		{
			MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
		}
	}
}

void PGE_Sounds::clearSoundBuffer()
{
	for (std::map<std::string, Mix_Chunk* >::iterator it=chunksBuffer.begin(); it!=chunksBuffer.end(); ++it)
	{
		Mix_FreeChunk(it->second);
	}
	chunksBuffer.clear();
}

#endif
