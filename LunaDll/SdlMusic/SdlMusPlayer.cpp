#include <windows.h>
#include <string>
#include "SdlMusPlayer.h"


Mix_Music *PGE_MusPlayer::play_mus = NULL;

bool PGE_SDL_Manager::isInit=false;
std::string PGE_MusPlayer::currentTrack="";

void PGE_SDL_Manager::initSDL()
{
	if(!isInit)
	{
		SDL_Init(SDL_INIT_AUDIO);
		isInit=true;
		PGE_MusPlayer::setSampleRate(44100);
		PGE_MusPlayer::MUS_changeVolume(75);
	}
}

int PGE_MusPlayer::volume=100;
int PGE_MusPlayer::sRate=44100;
bool PGE_MusPlayer::showMsg=true;
std::string PGE_MusPlayer::showMsg_for="";

void PGE_MusPlayer::MUS_stopMusic()
{
	if(!PGE_SDL_Manager::isInit) return;
    Mix_HaltMusic();
}

void PGE_MusPlayer::MUS_playMusic()
{
	if(!PGE_SDL_Manager::isInit) return;
	if(play_mus)
	{
		if(Mix_PlayingMusic()==0)
		{
			if(Mix_PlayMusic(play_mus, -1)==-1)
			{
				//MessageBoxA(0, std::string(std::string("Mix_PlayMusic:")+std::string(Mix_GetError())).c_str(), "Error", 0);
			}
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


void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    volume = vlm;
    Mix_VolumeMusic(volume);
}

void PGE_MusPlayer::setSampleRate(int sampleRate=44100)
{
    sRate=sampleRate;
    Mix_CloseAudio();
    Mix_OpenAudio(sRate, AUDIO_S16, 2, 4096);
	Mix_AllocateChannels(24);
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
	if(currentTrack==std::string(musFile)) return;

    if(play_mus!=NULL)
    {
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





Mix_Chunk *PGE_Sounds::sound = NULL;
char *PGE_Sounds::current = "";

void PGE_Sounds::SND_PlaySnd(const char *sndFile)
{
	PGE_SDL_Manager::initSDL();
    if(current!=sndFile)
    {
        if(sound) { Mix_FreeChunk(sound); sound=NULL; }

		sound = Mix_LoadWAV( sndFile );
        if(!sound) {
			MessageBoxA(0, std::string(std::string("Mix_LoadWAV: ")
			+std::string(sndFile)+"\n"
			+std::string(Mix_GetError())).c_str(), "Error", 0);
            // handle error
        }
    }

    //qDebug() << QString("Play Sound (SDL2_mixer)");
    if(Mix_PlayChannel( -1, sound, 0 )==-1)
    {
		MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
    }

}

