#ifndef NO_SDL

#include "MusicManager.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include <math.h>
#include "../libs/ini-reader/INIReader.h"

std::string MusicManager::chunksList[91];
std::string MusicManager::musList[74];

std::unordered_map<std::string, musicFile > MusicManager::registredFiles;
std::unordered_map<std::string, chunkFile > MusicManager::chunksBuffer;

std::string MusicManager::defaultSndINI="";
std::string MusicManager::defaultMusINI="";

std::string MusicManager::curRoot="";

void MusicManager::initAudioEngine()
{
    bool firstRun = !PGE_SDL_Manager::isInit;
    PGE_SDL_Manager::initSDL();
    if(firstRun)
    {
        initArrays();
        defaultSndINI=PGE_SDL_Manager::appPath+"sounds.ini";
        defaultMusINI=PGE_SDL_Manager::appPath+"music.ini";
        loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
        loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
    }
}

void MusicManager::addSound(std::string alias, std::string fileName)
{
    initAudioEngine();
	
	//clear junk
	replaceSubStr(fileName, "\"", "");
	replaceSubStr(fileName, "\\\\",  "\\");
	replaceSubStr(fileName, "/",  "\\");

	bool isChunk=false;
	std::string s(fileName);

	int chanID=0;
	int musID=0;
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
		file.first = Chunk;
		//Wroting another path to file
		file.second = chunksList[chanID];
	}
	else
	{
		bool found=false;
		for(int i=0; i<74; i++)
		{
			if(musAliasesList[i]==alias)
			{
				found=true;
				musID=i;
				break;
			}
		}
		file.first=Stream;
		if(found)
			file.second=musList[musID];
		else
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
			Mix_VolumeChunk(sound, MIX_MAX_VOLUME);
			std::unordered_map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
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
	std::unordered_map<std::string, musicFile>::iterator it = registredFiles.find(alias);
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
				std::unordered_map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
				if(it != chunksBuffer.end())
				{
					if(chunksBuffer[alias].first != -1)
						Mix_HaltChannel(chunksBuffer[alias].first);
					if(Mix_PlayChannelTimed( chunksBuffer[alias].first, chunksBuffer[alias].second, 0, -1 )==-1)
					{
						if(std::string(Mix_GetError())!="No free channels available")//Don't show overflow messagebox
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
	std::unordered_map<std::string, musicFile>::iterator it = registredFiles.find(alias);
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
			std::unordered_map<std::string, chunkFile >::iterator it = chunksBuffer.find(alias);
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

	curRoot=root;

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
			chunksList[i] = root+fileName.c_str();
		}
	}
}

void MusicManager::loadMusics(std::string path, std::string root)
{
		if( !file_existsX(path) ) return;

	INIReader MusicIni( path.c_str() );
	if (MusicIni.ParseError() < 0)
	{
		MessageBoxA(0, std::string(path+"\n\nError of read INI file").c_str(), "Error", 0);
		return;
	}

	curRoot=root;
	int i=0;

	//World music
	for(int j=1; (j<=16) && (i<74); i++, j++)
	{
		bool valid=false;
		std::string head = "world-music-"+i2str(j);
		std::string fileName;

		fileName = MusicIni.Get(head, "file", "");
		if(fileName.size()==0) continue;
		replaceSubStr(fileName, "\"", "");
		replaceSubStr(fileName, "\\\\",  "\\");
		replaceSubStr(fileName, "/",  "\\");
		if( file_existsX(root+fileName) )
		{
			musList[i] = root+fileName.c_str();
		}
	}

	//Special music
	for(int j=1; (j<=3) && (i<74); i++, j++)
	{
		bool valid=false;
		std::string head = "special-music-"+i2str(j);
		std::string fileName;

		fileName = MusicIni.Get(head, "file", "");
		if(fileName.size()==0) continue;
		replaceSubStr(fileName, "\"", "");
		replaceSubStr(fileName, "\\\\",  "\\");
		replaceSubStr(fileName, "/",  "\\");
		if( file_existsX(root+fileName) )
		{
			musList[i] = root+fileName.c_str();
		}
	}

	//Level music
	for(int j=1; (j<=56) && (i<74); i++, j++)
	{
		if(j==24) j++;
		bool valid=false;
		std::string head = "level-music-"+i2str(j);
		std::string fileName;

		fileName = MusicIni.Get(head, "file", "");
		if(fileName.size()==0) continue;
		replaceSubStr(fileName, "\"", "");
		replaceSubStr(fileName, "\\\\",  "\\");
		replaceSubStr(fileName, "/",  "\\");
		if( file_existsX(root+fileName) )
		{
			musList[i] = root+fileName.c_str();
		}
	}
}

void MusicManager::loadCustomSounds(std::string episodePath)
{
	initArrays();
	loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
	loadSounds(episodePath+"\\sounds.ini", episodePath);
	loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
	loadMusics(episodePath+"\\music.ini", episodePath);

	for(int i=0; i<91; i++)
		addSound(chunksAliasesList[i], "dummy");
	for(int i=0; i<74; i++)
		addSound(musAliasesList[i], "dummy");
}


void MusicManager::resetSoundsToDefault()
{
	initArrays();
	loadSounds(defaultSndINI, PGE_SDL_Manager::appPath);
	loadMusics(defaultMusINI, PGE_SDL_Manager::appPath);
	for(int i=0; i<91; i++)
		addSound(chunksAliasesList[i], "dummy");
	for(int i=0; i<74; i++)
		addSound(musAliasesList[i], "dummy");
}


void MusicManager::initArrays()
{
	curRoot = PGE_SDL_Manager::appPath;
	for(int i=0; i<91; i++)
		chunksList[i]=PGE_SDL_Manager::appPath+defaultChunksList[i];
	for(int i=0; i<74; i++)
		musList[i] = PGE_SDL_Manager::appPath+defaultMusList[i];
}

std::string MusicManager::SndRoot()
{
    return curRoot;
}

#endif
