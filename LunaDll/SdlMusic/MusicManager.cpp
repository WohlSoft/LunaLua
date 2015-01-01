#ifndef NO_SDL

#include "MusicManager.h"
#include "../Globals.h"
#include <math.h>

std::string MusicManager::chunksList[91]=
{
	"sound\\player-jump.mp3", 
	"sound\\stomped.mp3", 
	"sound\\block-hit.mp3", 
	"sound\\block-smash.mp3", 
	"sound\\player-shrink.mp3", 
	"sound\\player-grow.mp3", 
	"sound\\mushroom.mp3", 
	"sound\\player-died.mp3", 
	"sound\\shell-hit.mp3", 
	"sound\\player-slide.mp3", 
	"sound\\item-dropped.mp3", 
	"sound\\has-item.mp3", 
	"sound\\camera-change.mp3", 
	"sound\\coin.mp3", 
	"sound\\1up.mp3", 
	"sound\\lava.mp3", 
	"sound\\warp.mp3", 
	"sound\\fireball.mp3", 
	"sound\\level-win.mp3", 
	"sound\\boss-beat.mp3", 
	"sound\\dungeon-win.mp3", 
	"sound\\bullet-bill.mp3", 
	"sound\\grab.mp3", 
	"sound\\spring.mp3", 
	"sound\\hammer.mp3", 
	"sound\\slide.mp3", 
	"sound\\newpath.mp3", 
	"sound\\level-select.mp3", 
	"sound\\do.mp3", 
	"sound\\pause.mp3", 
	"sound\\key.mp3", 
	"sound\\pswitch.mp3", 
	"sound\\tail.mp3", 
	"sound\\racoon.mp3", 
	"sound\\boot.mp3", 
	"sound\\smash.mp3", 
	"sound\\thwomp.mp3", 
	"sound\\birdo-spit.mp3", 
	"sound\\birdo-hit.mp3", 
	"sound\\smb2-exit.mp3", 
	"sound\\birdo-beat.mp3", 
	"sound\\npc-fireball.mp3", 
	"sound\\fireworks.mp3", 
	"sound\\bowser-killed.mp3", 
	"sound\\game-beat.mp3", 
	"sound\\door.mp3", 
	"sound\\message.mp3", 
	"sound\\yoshi.mp3", 
	"sound\\yoshi-hurt.mp3", 
	"sound\\yoshi-tongue.mp3", 
	"sound\\yoshi-egg.mp3", 
	"sound\\got-star.mp3", 
	"sound\\zelda-kill.mp3", 
	"sound\\player-died2.mp3", 
	"sound\\yoshi-swallow.mp3", 
	"sound\\ring.mp3", 
	"sound\\dry-bones.mp3", 
	"sound\\smw-checkpoint.mp3", 
	"sound\\dragon-coin.mp3", 
	"sound\\smw-exit.mp3", 
	"sound\\smw-blaarg.mp3", 
	"sound\\wart-bubble.mp3", 
	"sound\\wart-die.mp3", 
	"sound\\sm-block-hit.mp3", 
	"sound\\sm-killed.mp3", 
	"sound\\sm-hurt.mp3", 
	"sound\\sm-glass.mp3", 
	"sound\\sm-boss-hit.mp3", 
	"sound\\sm-cry.mp3", 
	"sound\\sm-explosion.mp3", 
	"sound\\climbing.mp3", 
	"sound\\swim.mp3", 
	"sound\\grab2.mp3", 
	"sound\\smw-saw.mp3", 
	"sound\\smb2-throw.mp3", 
	"sound\\smb2-hit.mp3", 
	"sound\\zelda-stab.mp3", 
	"sound\\zelda-hurt.mp3", 
	"sound\\zelda-heart.mp3", 
	"sound\\zelda-died.mp3", 
	"sound\\zelda-rupee.mp3", 
	"sound\\zelda-fire.mp3", 
	"sound\\zelda-item.mp3", 
	"sound\\zelda-key.mp3", 
	"sound\\zelda-shield.mp3", 
	"sound\\zelda-dash.mp3", 
	"sound\\zelda-fairy.mp3", 
	"sound\\zelda-grass.mp3", 
	"sound\\zelda-hit.mp3", 
	"sound\\zelda-sword-beam.mp3", 
	"sound\\bubble.mp3"
};

int MusicManager::chunksChannelsList[91] =
{
	31,//"sound\\player-jump.mp3",
	-1,//"sound\\stomped.mp3",
	-1,//"sound\\block-hit.mp3",
	-1,//"sound\\block-smash.mp3",
	-1,//"sound\\player-shrink.mp3",
	-1,//"sound\\player-grow.mp3",
	-1,//"sound\\mushroom.mp3",
	-1,//"sound\\player-died.mp3",
	30,//"sound\\shell-hit.mp3",
	29,//"sound\\player-slide.mp3",
	-1,//"sound\\item-dropped.mp3",
	-1,//"sound\\has-item.mp3",
	-1,//"sound\\camera-change.mp3",
	-1,//"sound\\coin.mp3",
	-1,//"sound\\1up.mp3",
	-1,//"sound\\lava.mp3",
	-1,//"sound\\warp.mp3",
	-1,//"sound\\fireball.mp3",
	-1,//"sound\\level-win.mp3",
	-1,//"sound\\boss-beat.mp3",
	-1,//"sound\\dungeon-win.mp3",
	-1,//"sound\\bullet-bill.mp3",
	-1,//"sound\\grab.mp3",
	-1,//"sound\\spring.mp3",
	-1,//"sound\\hammer.mp3",
	-1,//"sound\\slide.mp3",
	-1,//"sound\\newpath.mp3",
	-1,//"sound\\level-select.mp3",
	-1,//"sound\\do.mp3",
	-1,//"sound\\pause.mp3",
	-1,//"sound\\key.mp3",
	28,//"sound\\pswitch.mp3",
	-1,//"sound\\tail.mp3",
	-1,//"sound\\racoon.mp3",
	-1,//"sound\\boot.mp3",
	27,//"sound\\smash.mp3",
	-1,//"sound\\thwomp.mp3",
	-1,//"sound\\birdo-spit.mp3",
	-1,//"sound\\birdo-hit.mp3",
	-1,//"sound\\smb2-exit.mp3",
	-1,//"sound\\birdo-beat.mp3",
	-1,//"sound\\npc-fireball.mp3",
	-1,//"sound\\fireworks.mp3",
	-1,//"sound\\bowser-killed.mp3",
	-1,//"sound\\game-beat.mp3",
	-1,//"sound\\door.mp3",
	-1,//"sound\\message.mp3",
	-1,//"sound\\yoshi.mp3",
	-1,//"sound\\yoshi-hurt.mp3",
	26,//"sound\\yoshi-tongue.mp3",
	-1,//"sound\\yoshi-egg.mp3",
	-1,//"sound\\got-star.mp3",
	-1,//"sound\\zelda-kill.mp3",
	-1,//"sound\\player-died2.mp3",
	-1,//"sound\\yoshi-swallow.mp3",
	-1,//"sound\\ring.mp3",
	-1,//"sound\\dry-bones.mp3",
	-1,//"sound\\smw-checkpoint.mp3",
	-1,//"sound\\dragon-coin.mp3",
	-1,//"sound\\smw-exit.mp3",
	-1,//"sound\\smw-blaarg.mp3",
	-1,//"sound\\wart-bubble.mp3",
	-1,//"sound\\wart-die.mp3",
	-1,//"sound\\sm-block-hit.mp3",
	-1,//"sound\\sm-killed.mp3",
	-1,//"sound\\sm-hurt.mp3",
	-1,//"sound\\sm-glass.mp3",
	-1,//"sound\\sm-boss-hit.mp3",
	-1,//"sound\\sm-cry.mp3",
	-1,//"sound\\sm-explosion.mp3",
	25,//"sound\\climbing.mp3",
	24,//"sound\\swim.mp3",
	-1,//"sound\\grab2.mp3",
	-1,//"sound\\smw-saw.mp3",
	-1,//"sound\\smb2-throw.mp3",
	-1,//"sound\\smb2-hit.mp3",
	-1,//"sound\\zelda-stab.mp3",
	-1,//"sound\\zelda-hurt.mp3",
	-1,//"sound\\zelda-heart.mp3",
	-1,//"sound\\zelda-died.mp3",
	-1,//"sound\\zelda-rupee.mp3",
	-1,//"sound\\zelda-fire.mp3",
	-1,//"sound\\zelda-item.mp3",
	-1,//"sound\\zelda-key.mp3",
	-1,//"sound\\zelda-shield.mp3",
	-1,//"sound\\zelda-dash.mp3",
	-1,//"sound\\zelda-fairy.mp3",
	-1,//"sound\\zelda-grass.mp3",
	-1,//"sound\\zelda-hit.mp3",
	-1,//"sound\\zelda-sword-beam.mp3",
	-1,//"sound\\bubble.mp3"
};

/*
typedef std::pair<int, std::string> musicFile;
*/

std::map<std::string, musicFile > MusicManager::registredFiles;
std::map<std::string, chunkFile > MusicManager::chunksBuffer;

void MusicManager::addSound(std::string alias, std::string fileName)
{
	PGE_SDL_Manager::initSDL();
	
	//clear junk
	replaceSubStr(fileName, "\"", "");
	replaceSubStr(fileName, "\\\\",  "\\");
	replaceSubStr(fileName, "/",  "\\");

	bool isChunk=false;
	std::string s(fileName);

	int chanID=0;
	//Check is this an SMBX Sound file
	for(int i=0;i<91;i++)
	{
		std::string t(chunksList[i]);
		if(s.length()<t.length()) continue;
		if(s.compare(s.length()-t.length(), t.length(), t)==0)
		{
			isChunk=true;
			chanID=i;
			break;
		}
	}

	musicFile file;
	if(isChunk)
	{
		file.first=Chunk;
		//replace extension of file with ogg
		s[s.length()-3]='o';
		s[s.length()-2]='g';
		s[s.length()-1]='g';
		file.second=s;
	}
	else
	{
		file.first=Stream;
		file.second=fileName;
	}
			
	registredFiles[alias]=file;

	if(isChunk)
	{//Register SDL Chunk
		Mix_Chunk* sound = Mix_LoadWAV( file.second.c_str() );
		if(!sound)
		{
			MessageBoxA(0, std::string(std::string("Mix_LoadWAV: ")
				+std::string(file.second)+"\n"
				+std::string(Mix_GetError())).c_str(), "Error", 0);
			// handle error
		}
		else
		{
			std::map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
			if(it == chunksBuffer.end())
			{
				chunkFile file;
				file.first  = chunksChannelsList[chanID];//ID of reserved channel for this sample
				file.second = sound;//Pointer to sample
				chunksBuffer[alias] = file;
			}
			else
			{
				Mix_FreeChunk(chunksBuffer[alias].second);
				chunksBuffer[alias].first  = chunksChannelsList[chanID];
				chunksBuffer[alias].second = sound;
			}
		}
	}

}

void MusicManager::close()
{
	PGE_MusPlayer::MUS_stopMusic();
	//for (std::map<std::string, Mix_Chunk *>::iterator it=chunksBuffer.begin(); it!=chunksBuffer.end(); ++it)
	//{
	//	Mix_FreeChunk(it->second);
	//}
	//chunksBuffer.clear();
	//registredFiles.clear();
}

void MusicManager::play(std::string alias) //Chunk will be played once, stream will be played with loop
{
	std::map<std::string, musicFile>::iterator it = registredFiles.find(alias);
		if(it != registredFiles.end())
		{
			musicFile file = registredFiles[alias];
			if(file.first==Stream)
			{
				PGE_MusPlayer::MUS_openFile(file.second.c_str());
				PGE_MusPlayer::MUS_playMusic();
			}
			else
			if(file.first==Chunk)
			{
				std::map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
				if(it != chunksBuffer.end())
				{
					if(Mix_PlayChannel( chunksBuffer[alias].first, chunksBuffer[alias].second, 0 )==-1)
					{
						MessageBoxA(0, std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str(), "Error", 0);
					}
				}
			}
		}

}

void MusicManager::pause()
{
	PGE_MusPlayer::MUS_pauseMusic();
}

void MusicManager::stop(std::string alias)
{
		std::map<std::string, musicFile>::iterator it = registredFiles.find(alias);
		if(it != registredFiles.end())
		{
			musicFile file = registredFiles[alias];
			if(file.first==Stream)
			{
				PGE_MusPlayer::MUS_stopMusic();
			}
			else
			if(file.first==Chunk)
			{
				std::map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
				if(it != chunksBuffer.end())
				{
					if(chunksBuffer[alias].first>=0)
						Mix_HaltChannel(chunksBuffer[alias].first);
				}
			}
		}

}

void MusicManager::setVolume(int _volume)
{
	double piece = ((double)_volume/1000.0);
	int converted = (int)floor((piece*128.0)+0.5);
	PGE_MusPlayer::MUS_changeVolume(converted);
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

#endif
