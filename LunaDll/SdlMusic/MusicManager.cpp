#ifndef NO_SDL

#include "MusicManager.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include <math.h>
#include "../libs/ini-reader/INIReader.h"

std::string MusicManager::chunksAliasesList[91]=
{
	"sound1",
	"sound2",
	"sound3",
	"sound4",
	"sound5",
	"sound6",
	"sound7",
	"sound8",
	"sound9",
	"sound10",
	"sound11",
	"sound12",
	"sound13",
	"sound14",
	"sound15",
	"sound16",
	"sound17",
	"sound18",
	"sound19",
	"sound20",
	"sound21",
	"sound22",
	"sound23",
	"sound24",
	"sound25",
	"sound26",
	"sound27",
	"sound28",
	"sound29",
	"sound30",
	"sound31",
	"sound32",
	"sound33",
	"sound34",
	"sound35",
	"sound36",
	"sound37",
	"sound38",
	"sound39",
	"sound40",
	"sound41",
	"sound42",
	"sound43",
	"sound44",
	"sound45",
	"sound46",
	"sound47",
	"sound48",
	"sound49",
	"sound50",
	"sound51",
	"sound52",
	"sound53",
	"sound54",
	"sound55",
	"sound56",
	"sound57",
	"sound58",
	"sound59",
	"sound60",
	"sound61",
	"sound62",
	"sound63",
	"sound64",
	"sound65",
	"sound66",
	"sound67",
	"sound68",
	"sound69",
	"sound70",
	"sound71",
	"sound72",
	"sound73",
	"sound74",
	"sound75",
	"sound76",
	"sound77",
	"sound78",
	"sound79",
	"sound80",
	"sound81",
	"sound82",
	"sound83",
	"sound84",
	"sound85",
	"sound86",
	"sound87",
	"sound88",
	"sound89",
	"sound90",
	"sound91"
};

std::string MusicManager::defaultChunksList[91]=
{
	"sound\\player-jump.ogg", 
	"sound\\stomped.ogg", 
	"sound\\block-hit.ogg", 
	"sound\\block-smash.ogg", 
	"sound\\player-shrink.ogg", 
	"sound\\player-grow.ogg", 
	"sound\\mushroom.ogg", 
	"sound\\player-died.ogg", 
	"sound\\shell-hit.ogg", 
	"sound\\player-slide.ogg", 
	"sound\\item-dropped.ogg", 
	"sound\\has-item.ogg", 
	"sound\\camera-change.ogg", 
	"sound\\coin.ogg", 
	"sound\\1up.ogg", 
	"sound\\lava.ogg", 
	"sound\\warp.ogg", 
	"sound\\fireball.ogg", 
	"sound\\level-win.ogg", 
	"sound\\boss-beat.ogg", 
	"sound\\dungeon-win.ogg", 
	"sound\\bullet-bill.ogg", 
	"sound\\grab.ogg", 
	"sound\\spring.ogg", 
	"sound\\hammer.ogg", 
	"sound\\slide.ogg", 
	"sound\\newpath.ogg", 
	"sound\\level-select.ogg", 
	"sound\\do.ogg", 
	"sound\\pause.ogg", 
	"sound\\key.ogg", 
	"sound\\pswitch.ogg", 
	"sound\\tail.ogg", 
	"sound\\racoon.ogg", 
	"sound\\boot.ogg", 
	"sound\\smash.ogg", 
	"sound\\thwomp.ogg", 
	"sound\\birdo-spit.ogg", 
	"sound\\birdo-hit.ogg", 
	"sound\\smb2-exit.ogg", 
	"sound\\birdo-beat.ogg", 
	"sound\\npc-fireball.ogg", 
	"sound\\fireworks.ogg", 
	"sound\\bowser-killed.ogg", 
	"sound\\game-beat.ogg", 
	"sound\\door.ogg", 
	"sound\\message.ogg", 
	"sound\\yoshi.ogg", 
	"sound\\yoshi-hurt.ogg", 
	"sound\\yoshi-tongue.ogg", 
	"sound\\yoshi-egg.ogg", 
	"sound\\got-star.ogg", 
	"sound\\zelda-kill.ogg", 
	"sound\\player-died2.ogg", 
	"sound\\yoshi-swallow.ogg", 
	"sound\\ring.ogg", 
	"sound\\dry-bones.ogg", 
	"sound\\smw-checkpoint.ogg", 
	"sound\\dragon-coin.ogg", 
	"sound\\smw-exit.ogg", 
	"sound\\smw-blaarg.ogg", 
	"sound\\wart-bubble.ogg", 
	"sound\\wart-die.ogg", 
	"sound\\sm-block-hit.ogg", 
	"sound\\sm-killed.ogg", 
	"sound\\sm-hurt.ogg", 
	"sound\\sm-glass.ogg", 
	"sound\\sm-boss-hit.ogg", 
	"sound\\sm-cry.ogg", 
	"sound\\sm-explosion.ogg", 
	"sound\\climbing.ogg", 
	"sound\\swim.ogg", 
	"sound\\grab2.ogg", 
	"sound\\smw-saw.ogg", 
	"sound\\smb2-throw.ogg", 
	"sound\\smb2-hit.ogg", 
	"sound\\zelda-stab.ogg", 
	"sound\\zelda-hurt.ogg", 
	"sound\\zelda-heart.ogg", 
	"sound\\zelda-died.ogg", 
	"sound\\zelda-rupee.ogg", 
	"sound\\zelda-fire.ogg", 
	"sound\\zelda-item.ogg", 
	"sound\\zelda-key.ogg", 
	"sound\\zelda-shield.ogg", 
	"sound\\zelda-dash.ogg", 
	"sound\\zelda-fairy.ogg", 
	"sound\\zelda-grass.ogg", 
	"sound\\zelda-hit.ogg", 
	"sound\\zelda-sword-beam.ogg", 
	"sound\\bubble.ogg"
};

int MusicManager::chunksChannelsList[91] =
{
	31,//"sound\\player-jump.ogg",
	-1,//"sound\\stomped.ogg",
	-1,//"sound\\block-hit.ogg",
	-1,//"sound\\block-smash.ogg",
	-1,//"sound\\player-shrink.ogg",
	-1,//"sound\\player-grow.ogg",
	-1,//"sound\\mushroom.ogg",
	-1,//"sound\\player-died.ogg",
	30,//"sound\\shell-hit.ogg",
	29,//"sound\\player-slide.ogg",
	-1,//"sound\\item-dropped.ogg",
	-1,//"sound\\has-item.ogg",
	-1,//"sound\\camera-change.ogg",
	-1,//"sound\\coin.ogg",
	-1,//"sound\\1up.ogg",
	-1,//"sound\\lava.ogg",
	-1,//"sound\\warp.ogg",
	-1,//"sound\\fireball.ogg",
	-1,//"sound\\level-win.ogg",
	-1,//"sound\\boss-beat.ogg",
	-1,//"sound\\dungeon-win.ogg",
	-1,//"sound\\bullet-bill.ogg",
	-1,//"sound\\grab.ogg",
	-1,//"sound\\spring.ogg",
	-1,//"sound\\hammer.ogg",
	-1,//"sound\\slide.ogg",
	-1,//"sound\\newpath.ogg",
	-1,//"sound\\level-select.ogg",
	-1,//"sound\\do.ogg",
	-1,//"sound\\pause.ogg",
	-1,//"sound\\key.ogg",
	28,//"sound\\pswitch.ogg",
	-1,//"sound\\tail.ogg",
	-1,//"sound\\racoon.ogg",
	-1,//"sound\\boot.ogg",
	27,//"sound\\smash.ogg",
	-1,//"sound\\thwomp.ogg",
	-1,//"sound\\birdo-spit.ogg",
	-1,//"sound\\birdo-hit.ogg",
	-1,//"sound\\smb2-exit.ogg",
	-1,//"sound\\birdo-beat.ogg",
	-1,//"sound\\npc-fireball.ogg",
	-1,//"sound\\fireworks.ogg",
	-1,//"sound\\bowser-killed.ogg",
	-1,//"sound\\game-beat.ogg",
	-1,//"sound\\door.ogg",
	-1,//"sound\\message.ogg",
	-1,//"sound\\yoshi.ogg",
	-1,//"sound\\yoshi-hurt.ogg",
	26,//"sound\\yoshi-tongue.ogg",
	-1,//"sound\\yoshi-egg.ogg",
	-1,//"sound\\got-star.ogg",
	-1,//"sound\\zelda-kill.ogg",
	-1,//"sound\\player-died2.ogg",
	-1,//"sound\\yoshi-swallow.ogg",
	-1,//"sound\\ring.ogg",
	-1,//"sound\\dry-bones.ogg",
	-1,//"sound\\smw-checkpoint.ogg",
	-1,//"sound\\dragon-coin.ogg",
	-1,//"sound\\smw-exit.ogg",
	-1,//"sound\\smw-blaarg.ogg",
	-1,//"sound\\wart-bubble.ogg",
	-1,//"sound\\wart-die.ogg",
	-1,//"sound\\sm-block-hit.ogg",
	-1,//"sound\\sm-killed.ogg",
	-1,//"sound\\sm-hurt.ogg",
	-1,//"sound\\sm-glass.ogg",
	-1,//"sound\\sm-boss-hit.ogg",
	-1,//"sound\\sm-cry.ogg",
	-1,//"sound\\sm-explosion.ogg",
	25,//"sound\\climbing.ogg",
	24,//"sound\\swim.ogg",
	-1,//"sound\\grab2.ogg",
	-1,//"sound\\smw-saw.ogg",
	-1,//"sound\\smb2-throw.ogg",
	-1,//"sound\\smb2-hit.ogg",
	-1,//"sound\\zelda-stab.ogg",
	-1,//"sound\\zelda-hurt.ogg",
	-1,//"sound\\zelda-heart.ogg",
	-1,//"sound\\zelda-died.ogg",
	-1,//"sound\\zelda-rupee.ogg",
	-1,//"sound\\zelda-fire.ogg",
	-1,//"sound\\zelda-item.ogg",
	-1,//"sound\\zelda-key.ogg",
	-1,//"sound\\zelda-shield.ogg",
	-1,//"sound\\zelda-dash.ogg",
	-1,//"sound\\zelda-fairy.ogg",
	-1,//"sound\\zelda-grass.ogg",
	-1,//"sound\\zelda-hit.ogg",
	-1,//"sound\\zelda-sword-beam.ogg",
	-1,//"sound\\bubble.ogg"
};

/*
typedef std::pair<int, std::string> musicFile;
*/

std::string MusicManager::chunksList[91];

std::map<std::string, musicFile > MusicManager::registredFiles;
std::map<std::string, chunkFile > MusicManager::chunksBuffer;

std::string MusicManager::defaultSndINI="";
std::string MusicManager::defaultMusINI="";

void MusicManager::addSound(std::string alias, std::string fileName)
{
	bool firstRun = !PGE_SDL_Manager::isInit;
	PGE_SDL_Manager::initSDL();
	if(firstRun)
	{
		initChunks();
		defaultSndINI=PGE_SDL_Manager::appPath+"sounds.ini";
		defaultMusINI=PGE_SDL_Manager::appPath+"music.ini";
		loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
		loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
	}
	
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
		if(chunksAliasesList[i]==alias)
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
		//Wroting another path to file
		file.second=PGE_SDL_Manager::appPath+chunksList[chanID];
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

void MusicManager::loadSounds(std::string path, std::string root)
{
	if( !file_existsX(path) ) return;

	INIReader hitBoxFile( path.c_str() );
	if (hitBoxFile.ParseError() < 0)
	{
		MessageBoxA(0, std::string(path+"\n\nError of read INI file").c_str(), "Error", 0);
		return;
	}

	for(int i=0; i<91; i++)
	{
		bool valid=false;
		std::string head = "sound-"+i2str(i+1);
		std::string fileName;

		fileName = hitBoxFile.Get(head, "file", "");
		if(fileName.size()==0) continue;
		replaceSubStr(fileName, "\"", "");
		replaceSubStr(fileName, "\\\\",  "\\");
		replaceSubStr(fileName, "/",  "\\");

		if( file_existsX(root+fileName) )
		{
			chunksList[i] = fileName.c_str();
		}
	}
}

void MusicManager::loadMusics(std::string path, std::string root)
{

}

void MusicManager::loadCustomSounds(std::string episodePath)
{
	initChunks();
	loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
	loadSounds(episodePath+"\\sounds.ini", episodePath);

	for(int i=0; i<91; i++)
		addSound(chunksAliasesList[i], "dummy");
}


void MusicManager::resetSoundsToDefault()
{
	initChunks();
	loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
	for(int i=0; i<91; i++)
		addSound(chunksAliasesList[i], "dummy");
}


void MusicManager::initChunks()
{
	for(int i=0; i<91; i++)
		chunksList[i]=defaultChunksList[i];
}

#endif
