#----------------------------------------------------------------------------
#
# Platformer Game Engine by Wohlstand, a free platform for game making
# Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

CONFIG  += C++11

!contains(DEFINES, PGE_FILES_QT): SOURCES +=  $$PWD/ConvertUTF_PGEFF.c

SOURCES +=  $$PWD/file_formats.cpp \
            $$PWD/file_rw_lvl.cpp \
            $$PWD/file_rw_lvl_38a.cpp \
            $$PWD/file_rw_lvlx.cpp \
            $$PWD/file_rw_meta.cpp \
            $$PWD/file_rw_npc_txt.cpp \
            $$PWD/file_rw_sav.cpp \
            $$PWD/file_rw_wld.cpp \
            $$PWD/file_rw_wldx.cpp \
            $$PWD/file_rw_smbx64_cnf.cpp \
            $$PWD/file_rwopen.cpp \
            $$PWD/file_strlist.cpp \
            $$PWD/lvl_filedata.cpp \
            $$PWD/npc_filedata.cpp \
            $$PWD/pge_x.cpp \
            $$PWD/save_filedata.cpp \
            $$PWD/smbx64.cpp \
            $$PWD/smbx64_cnf_filedata.cpp \
            $$PWD/wld_filedata.cpp \
            $$PWD/pge_file_lib_globs.cpp \
            $$PWD/file_rw_savx.cpp \
            $$PWD/file_rw_lvl_38a_old.cpp \
            $$PWD/file_rw_wld_38a.cpp

HEADERS +=  $$PWD/file_formats.h \
            $$PWD/file_strlist.h \
            $$PWD/lvl_filedata.h \
            $$PWD/meta_filedata.h \
            $$PWD/npc_filedata.h \
            $$PWD/pge_x.h \
            $$PWD/pge_x_macro.h \
            $$PWD/save_filedata.h \
            $$PWD/smbx64.h \
            $$PWD/smbx64_macro.h \
            $$PWD/wld_filedata.h \
            $$PWD/pge_file_lib_globs.h \
            $$PWD/pge_file_lib_sys.h \
            $$PWD/smbx64_cnf_filedata.h \
            $$PWD/CSVReader.h \
            $$PWD/CSVReaderPGE.h \
            $$PWD/CSVUtils.h \
            $$PWD/pge_ff_units.h \
            $$PWD/smbx38a_private.h
