#include <fstream>
#include <iostream>
#include <ios>
#include "../LuaProxy.h"
#include "../../GlobalFuncs.h"
#include "../../SMBXInternal/Level.h"
#include "../../Misc/MiscFuncs.h"

LuaProxy::Data::Data(DataType dataType) :
	m_dataType(dataType),
	m_sectionName("default"),
	m_useSaveSlot(dataType != DATA_GLOBAL)
{
	init();
}

LuaProxy::Data::Data(DataType dataType, bool useSaveSlot) :
	m_dataType(dataType),
	m_sectionName("default"),
	m_useSaveSlot(useSaveSlot)
{
	init();
}

LuaProxy::Data::Data(DataType dataType, const std::string &sectionName) :
	m_dataType(dataType),
	m_sectionName(sectionName),
	m_useSaveSlot(dataType != DATA_GLOBAL)
{
	init();
}


LuaProxy::Data::Data(DataType dataType, const std::string &sectionName, bool useSaveSlot) :
	m_dataType(dataType),
	m_sectionName(sectionName),
	m_useSaveSlot(useSaveSlot)
{
	init();
}


void LuaProxy::Data::init()
{
	std::string path = resolvePathFromSection();
	std::ifstream luaData(path, std::ios::binary | std::ios::in);
	if (!luaData.is_open()){
		std::ofstream luaDataCreate(path, std::ios::binary | std::ios::out);
		if (luaDataCreate.is_open())
			luaDataCreate.close();
	}
	if (luaData.is_open()){
		while (!luaData.eof())
		{
			std::string key;
			std::string value;
			std::getline(luaData, key);
			if (luaData.eof())
				break;
			std::getline(luaData, value);
			replaceSubStr(key, "\\n", "\n");
			replaceSubStr(value, "\\n", "\n");
			m_data[key] = value;
		}
		luaData.close();
	}
}


std::string LuaProxy::Data::resolvePathFromSection()
{
	std::string levelName = utf8_encode(::Level::GetName());
	std::string episodePath = (std::string)GM_FULLDIR;
	std::string smbxPath = utf8_encode(getModulePath());
	std::string returnPath = "";
	switch (m_dataType)
	{
	case LuaProxy::Data::DATA_LEVEL:
		returnPath += episodePath;
		returnPath += removeExtension(levelName);
		returnPath += "\\";
		returnPath += "LuaData_";
		if (m_useSaveSlot)
			returnPath += "Save" + std::to_string(GM_CUR_SAVE_SLOT) + "_";
		returnPath += m_sectionName;
		returnPath += ".txt";
		break;
	case LuaProxy::Data::DATA_WORLD:
		returnPath += episodePath;
		returnPath += "LuaData_";
		if (m_useSaveSlot)
			returnPath += "Save" + std::to_string(GM_CUR_SAVE_SLOT) + "_";
		returnPath += m_sectionName;
		returnPath += ".txt";
		break;
	case LuaProxy::Data::DATA_GLOBAL:
	default:
		returnPath += smbxPath;
		returnPath += "\\worlds\\";
		returnPath += "LuaData_";
		if (m_useSaveSlot)
			returnPath += "Save" + std::to_string(GM_CUR_SAVE_SLOT) + "_";
		returnPath += m_sectionName;
		returnPath += ".txt";
		break;
	}
	return returnPath;
}



void LuaProxy::Data::set(const std::string &key, const std::string &value)
{
	m_data[key] = value;
}


std::string LuaProxy::Data::get(const std::string &key) const
{
	// Using [] isn't allowed in a const function, so one is supposed to use 'find'
	std::map<std::string, std::string>::const_iterator it;
	it = m_data.find(key);
	return it == m_data.end() ? "" : it->second;
}

luabind::object LuaProxy::Data::get(lua_State* L) const
{
	luabind::object allData = luabind::newtable(L);
	for (std::map<std::string, std::string>::const_iterator it = m_data.begin(); it != m_data.end(); ++it) {
		allData[it->first] = it->second;
	}
	return allData;
}


void LuaProxy::Data::save()
{
	save(m_sectionName);
}

void LuaProxy::Data::save(const std::string &sectionName)
{
	m_sectionName = sectionName;
	std::string path = resolvePathFromSection();
	std::ofstream luaData(path, std::ios::binary | std::ios::out);
	if (luaData.is_open()){
		for (std::map<std::string, std::string>::iterator it = m_data.begin(); it != m_data.end(); ++it) {
			std::string key = it->first;
			std::string value = it->second;
			replaceSubStr(key, "\n", "\\n");
			replaceSubStr(value, "\n", "\\n");
			luaData << key << std::endl;
			luaData << value << std::endl;
		}
		luaData.close();
	}
}


LuaProxy::Data::DataType LuaProxy::Data::dataType() const
{
	return m_dataType;
}


void LuaProxy::Data::setDataType(DataType dataType)
{
	m_dataType = dataType;
}


std::string LuaProxy::Data::sectionName() const
{
	return m_sectionName;
}


void LuaProxy::Data::setSectionName(const std::string &sectionName)
{
	m_sectionName = sectionName;
}


bool LuaProxy::Data::useSaveSlot() const
{
	return m_useSaveSlot;
}


void LuaProxy::Data::setUseSaveSlot(bool useSaveSlot)
{
	m_useSaveSlot = useSaveSlot;
}



LuaProxy::Data::~Data()
{}
