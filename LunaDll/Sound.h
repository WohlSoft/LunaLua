#pragma once
#include <string>

namespace SMBXSound {
	void PlaySFX(int soundindex);

	void SetMusic(int musicnum, int section);
	void SetMusicPath(int section, std::wstring path);
	void PlayMusic(int section);
}