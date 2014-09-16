#pragma once

namespace ScienceBattle
{
	void ScienceCode();
	void ScienceInitCode();
}

namespace SAJSnowbordin
{
	void SnowbordinCode();
	void SnowbordinInitCode();
}

// <-- ADD NEW LEVELS TO THIS --> //
enum LvlEnum {
	Invalid,
	DllTestLvl,
	QraestoliaCaverns,
	TheFloorIsLava,
	CurtainTortoise,
	AbstractAssault,
	DemosBrain,
	EuroShellRandD,
	Calleoca,
	Science,
	Snowbordin,
	ThouStartsANewVideo,
};

// Add function declarations for specific level functions here (unless the level doesn't need its own)
void dlltestlvlCode();
void QraestoliaCavernsCode();
void TheFloorisLavaCode();
void CurtainTortoiseCode();
void AbstractAssaultCode();
void DemosBrainCode();
void EuroShellRandDCode();
void KilArmoryCode();
void CalleocaCode();
void CalleocaInitCode();
void ScienceBattle::ScienceCode();
void ScienceBattle::ScienceInitCode();
void SAJSnowbordin::SnowbordinCode();
void SAJSnowbordin::SnowbordinInitCode();