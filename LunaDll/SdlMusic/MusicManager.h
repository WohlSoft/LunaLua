
#ifndef MusicManager_HHHH
#define MusicManager_HHHH

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "SdlMusPlayer.h"

typedef std::pair<int, std::string> musicFile;

class MusicManager
{
	public:
		enum types{
		Stream=0,
		Chunk
		};
		static void addSound(std::string alias, std::string fileName);
		static void close();
		static void play(std::string alias); //Chunk will be played once, stream will be played with loop
		static void pause();
		static void stop(std::string alias);
		static void setVolume(int _volume);
		static std::string lenght();
		static std::string position();
	
	private:
		static std::map<std::string, musicFile> registredFiles;
		static std::map<std::string, Mix_Chunk *> chunksBuffer;
		static std::string chunksList[91];
};
#endif
