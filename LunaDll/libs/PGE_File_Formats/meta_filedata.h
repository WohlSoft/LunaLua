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
 * \file meta_filedata.h
 * \brief Contains defition of the common meta-data structures and classes
 */
#pragma once
#ifndef META_FILEDATA_H
#define META_FILEDATA_H

#include "pge_file_lib_globs.h"
#include <memory>

/**
 * @brief Common data structure meta-data
 */
struct FileFormatMeta
{
    FileFormatMeta():
        ReadFileValid(true),
        ERROR_linenum(-1),
        RecentFormat(0),
        RecentFormatVersion(0),
        modified(true),
        untitled(true),
        smbx64strict(false)
    {}
    //! Is file parsed correctly, false if some error is occouped
    bool ReadFileValid;
    //! Error messsage
    PGESTRING ERROR_info;
    //! Line data where error was occouped
    PGESTRING ERROR_linedata;
    //! Number of line where error was occouped
    long      ERROR_linenum;
    //! Recently used (open or save) file format
    int RecentFormat;
    //! Recently used format version (for SMBX1...64 files only)
    unsigned int RecentFormatVersion;
    //! Is file was modified since open?
    bool modified;
    //! Is this level made from scratch and was not saved into file?
    bool untitled;
    //! Enable SMBX64 Standard restrictions
    //! (disable unsupported features and warn about limit exiting)
    bool smbx64strict;
    //! Recent file name since file was opened
    PGESTRING filename;
    //! Recent file path where file was located since it was opened
    PGESTRING path;
};

/**
 * @brief Common element meta-data
 */
struct ElementMeta
{
    ElementMeta() :
        array_id(0),
        index(0),
        userdata(nullptr) {}
    //! Array-ID is an unique key value identificates each unique element object.
    //! Re-counts on each file reloading
    unsigned int array_id;
    //! Recent array index where element was saved (used to speed-up settings synchronization)
    unsigned int index;
    //! JSON-like string with a custom properties (without master brackets, like "param":"value,["subparam":value])
    PGESTRING custom_params;
    //! User data pointer, Useful in the editors to have direct pointer to pre-placed elements
    void *userdata;
};

/*!
 * \brief Position bookmark entry structure
 */
struct Bookmark
{
    PGESTRING bookmarkName; //!< Name of bookmark
    double x;               //!< Bookmarked X position of the camera
    double y;               //!< Bookmarked Y position of the camera
};

#ifdef PGE_EDITOR
/*!
 * \brief Contains backup of helpful techincal data used by PGE Editor
 */
class CrashData
{
    public:
        /*!
         * \brief Constructor
         */
        explicit CrashData();
        /*!
         * \brief Copy constructor
         * \param _cd another CrashData object
         */
        CrashData(const CrashData &_cd);
        /*!
         * \brief Copy constructor
         * \param _cd another CrashData object
         */
        CrashData(CrashData &_cd);
        /*!
         * \brief Sets default preferences
         */
        void reset();
        //! Is crash data was used by editor (if false, LVLX writer will not record crash data into file)
        bool used;
        //! Is this level was untitled since crash occopued?
        bool untitled;
        //! Is this level was modified before crash occouped?
        bool modifyed;
        //! Recent file format ID (specific enum in the format structure declaration)
        int fmtID;
        //! Recent file format version
        unsigned int fmtVer;
        //! Full original file path file which was opened before crash occouped
        PGESTRING fullPath;
        //! Full episode path of file which was opened before crash occouped
        PGESTRING path;
        //! Base file name of file which was opened before crash occouped
        PGESTRING filename;
};
#endif

/*!
 * \brief Contains additional helpful meda-data used by PGE Applications
 */
struct MetaData
{
    //! Array of all position bookmarks presented in the opened file
    PGEVECTOR<Bookmark> bookmarks;

    /* For Editor application only*/
#ifdef PGE_EDITOR
    //! Crash backup of Editor's special data
    CrashData crash;
#endif

    //!Helper meta-data
    FileFormatMeta meta;
};

#endif // META_FILEDATA_H
