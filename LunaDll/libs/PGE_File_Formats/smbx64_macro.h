#pragma once
#ifndef SMBX64_MACRO_H
#define SMBX64_MACRO_H

/*! \file smbx64_macro.h
 *  \brief Contains helper macroses for making SMBX64 Standard format based parsers
 */

/*
  Small set of macroses used to organize SMBX64 specific file read functions.
*/

//(you must create and open PGE_FileFormats_misc::TextInput &in; !!!)
#define SMBX64_FileBegin() unsigned int file_format = 0;   /*File format number*/\
                           PGESTRING line;      /*Current Line data*/

//Jump to next line
#define nextLine() line = in.readCVSLine();

//Version comparison
#define ge(v) file_format>=v
#define gt(v) file_format>v
#define le(v) file_format<=v
#define lt(v) file_format<v

#endif // SMBX64_MACRO_H

