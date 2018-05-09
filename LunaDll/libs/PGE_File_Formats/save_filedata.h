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

/*!
 * \file save_filedata.h
 *
 * \brief Contains structure definitions for a game save data
 *
 */

#pragma once
#ifndef SAVE_FILEDATA_H
#define SAVE_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

//! Game Save specific Visible element entry <array-id, is-vizible>
typedef PGEPAIR<unsigned int, bool > visibleItem;
//! Game Save specific gotten star entry <Level-Filename, Section-ID(SMBX64-Standard, one star per section) or NPC-ArrayID (PGE-X, multiple stars per section)>
typedef PGEPAIR<PGESTRING, int > starOnLevel;

/*!
 * \brief Recent state of each playable character
 */
struct saveCharState
{
    saveCharState() : id(1), state(1), itemID(0), mountType(0), mountID(0), health(1) {}
    //! ID of playable character
    unsigned long id;
    //! Curent ID of playable character's state
    unsigned long state;
    //! Current item ID in the item slot (SMBX64-only)
    unsigned long itemID;
    //! Mounted vehicle type (SMBX64-only)
    unsigned int mountType;
    //! Mounted vehicle ID (SMBX64-only)
    unsigned int mountID;
    //! Recent health level
    unsigned int health;
};

/*!
 * \brief Recent playable character ID per player ID
 */
struct savePlayerState
{
    //! ID of playable character
    int characterID;
};

/*!
 * \brief Game save data structure
 */
struct GamesaveData
{
    //! Helper meta-data
    FileFormatMeta meta;

    int lives;               //!< Number of lives
    unsigned int coins;      //!< Number of coins
    unsigned int points;     //!< Number of points
    unsigned int totalStars; //!< Total stars

    long worldPosX; //!< Last world map position X
    long worldPosY; //!< Last world map position Y

    unsigned long last_hub_warp; //!< Last entered/exited warp Array-ID on the HUB-based episodes.

    unsigned int musicID; //!< Current world music ID
    PGESTRING musicFile;    //!< Current world music file (custom music)

    bool gameCompleted;   //!< Is episode was completed in last time

    PGEVECTOR<saveCharState > characterStates;
    PGELIST<unsigned long > currentCharacter;

    //Visible state of world map items
    PGEVECTOR<visibleItem > visibleLevels;
    PGEVECTOR<visibleItem > visiblePaths;
    PGEVECTOR<visibleItem > visibleScenery;
    PGEVECTOR<starOnLevel > gottenStars;
};

#endif // SAVE_FILEDATA_H
