#ifndef NO_SDL

#ifndef MusicManager_HHHH
#define MusicManager_HHHH

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "SdlMusPlayer.h"

typedef std::pair<int, std::string> musicFile;
typedef std::pair<int, Mix_Chunk* > chunkFile;

class MusicManager
{
	public:
		enum types{
		Stream=0,
		Chunk
		};
        static void initAudioEngine();
		static void addSound(std::string alias, std::string fileName);
		static void close();
		static void play(std::string alias); //Chunk will be played once, stream will be played with loop
		static void pause();
		static void stop(std::string alias);
		static void setVolume(int _volume);
		static std::string lenght();
		static std::string position();
		
		static void loadCustomSounds(std::string episodePath);
		static void resetSoundsToDefault();
		static void loadSounds(std::string path, std::string root);
		static void loadMusics(std::string path, std::string root);
		static std::string SndRoot();
	private:
		static std::map<std::string, musicFile > registredFiles;
		static std::map<std::string, chunkFile > chunksBuffer;

		static std::string curRoot;//Current rood directory (episode or application dir)

		//Musics
		static std::string musList[74];//List of currently loaded files
		static std::string defaultMusList[74];//List of system default files
		static std::string musAliasesList[74];//List of reserved aliases for sound effects

		//SFX
		static std::string chunksList[91];//List of currently loaded files
		static std::string defaultChunksList[91];//List of system default files
		static std::string chunksAliasesList[91];//List of reserved aliases for sound effects
		static int chunksChannelsList[91];//List of channel reservation by some files (-1 is allowing mixed playback)

		static void initArrays();//Fill chinks and musics list with system default files
		
		//INI Paths
		static std::string defaultSndINI;//Full path to global sounds.ini file
		static std::string defaultMusINI;//Full path to global music.ini file
};
#endif


#endif
