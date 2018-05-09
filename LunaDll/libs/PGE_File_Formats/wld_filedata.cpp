/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "file_formats.h"
#include "wld_filedata.h"


int FileFormats::smbx64WorldCheckLimits(WorldData &wld)
{
    int errorCode = 0;

    //Tiles limit
    if(wld.tiles.size() > 20000) errorCode |= SMBX64EXC_TILES;
    //Sceneries limit
    if(wld.scenery.size() > 5000) errorCode |= SMBX64EXC_SCENERIES;
    //Paths limit
    if(wld.paths.size() > 2000) errorCode |= SMBX64EXC_PATHS;
    //Levels limit
    if(wld.levels.size() > 400) errorCode |= SMBX64EXC_LEVELS;
    //Music boxes limit
    if(wld.music.size() > 1000) errorCode |= SMBX64EXC_MUSICBOXES;

    return errorCode;
}


WorldTerrainTile FileFormats::CreateWldTile()
{
    return WorldTerrainTile();
}

WorldScenery FileFormats::CreateWldScenery()
{
    return WorldScenery();
}

WorldPathTile FileFormats::CreateWldPath()
{
    return WorldPathTile();
}

WorldLevelTile FileFormats::CreateWldLevel()
{
    return WorldLevelTile();
}

WorldMusicBox FileFormats::CreateWldMusicbox()
{
    return WorldMusicBox();
}

void FileFormats::CreateWorldHeader(WorldData &NewFileData)
{
    NewFileData = WorldData();
}


void FileFormats::CreateWorldData(WorldData &NewFileData)
{
    CreateWorldHeader(NewFileData);

    NewFileData.tiles.clear();
    NewFileData.tile_array_id = 0;
    NewFileData.scenery.clear();
    NewFileData.scene_array_id = 0;
    NewFileData.paths.clear();
    NewFileData.path_array_id = 0;
    NewFileData.levels.clear();
    NewFileData.level_array_id = 0;
    NewFileData.music.clear();
    NewFileData.musicbox_array_id = 0;
    NewFileData.arearects.clear();
    NewFileData.arearect_array_id = 0;
    NewFileData.layers.clear();
    NewFileData.layers_array_id = 0;
}

WorldData FileFormats::CreateWorldData()
{
    WorldData NewFileData;
    CreateWorldData(NewFileData);
    return NewFileData;
}
