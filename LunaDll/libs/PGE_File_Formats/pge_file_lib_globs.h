/*! \file pge_file_lib_globs.h
    \brief Contains internal settings and references for PGE File Library

    All defined here macroses are allows to build PGE File Library for both
    Qt and STL libraries set.
*/
#pragma once
#ifndef PGE_FILE_LIB_GLOBS_H_
#define PGE_FILE_LIB_GLOBS_H_

/*! \def PGE_FILES_QT
    \brief If this macro is defined, Qt version of PGE File Library will be built
*/

/*! \def PGE_ENGINE
    \brief If this macro is defined, this library builds as part of PGE Engine
*/

/*! \def PGE_EDITOR
    \brief If this macro is defined, this library builds as part of PGE Editor
*/

/*! \def PGEChar
    \brief A macro which equal to 'char' if PGE File Library built in the STL mode
           and equal to QChar if PGE File Library built in the Qt mode
*/

/*! \def PGESTRING
    \brief A macro which equal to std::string if PGE File Library built in the STL mode
           and equal to QString if PGE File Library built in the Qt mode
*/

/*! \def PGESTRINGList
    \brief A macro which equal to std::vector<std::string> if PGE File Library built in the STL mode
           and equal to QStringList if PGE File Library built in the Qt mode
*/

/*! \def PGELIST
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QList if PGE File Library built in the Qt mode
*/

/*! \def PGEVECTOR
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QVector if PGE File Library built in the Qt mode
*/

#include <cstdint>

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QHash>
#include <QObject>
#include <QUrl>
#include <cmath>
#if defined(PGE_ENGINE)||defined(PGE_EDITOR)
#include <QSize>
#endif
#define PGE_FILES_INHERED public QObject
typedef QString PGESTRING;
typedef int     pge_size_t;
inline PGESTRING PGESTR_Simpl(const PGESTRING &str)
{
    return str.simplified();
}
inline PGESTRING PGESTR_toLower(const PGESTRING &str)
{
    return str.toLower();
}
#define PGEGetChar(chr) chr.toLatin1()
typedef QChar PGEChar;
typedef QStringList PGESTRINGList;
#define PGEVECTOR QVector
#define PGELIST QList
typedef int pge_size_t;
#define PGEPAIR QPair
#define PGEMAP  QMap
#define PGEHASH QHash
typedef QFile PGEFILE;
inline void PGE_SPLITSTRING(PGESTRINGList &dst, PGESTRING &src, PGESTRING sep)
{
    dst = src.split(sep);
}
inline PGESTRING PGE_ReplSTRING(PGESTRING &src, PGESTRING from, PGESTRING to)
{
    return src.replace(from, to);
}
inline PGESTRING PGE_RemSubSTRING(PGESTRING &src, PGESTRING substr)
{
    return src.remove(substr);
}
inline PGESTRING PGE_RemStrRng(PGESTRING &str, int pos, int len)
{
    return str.remove(pos, len);
}
inline PGESTRING PGE_SubStr(PGESTRING &str, int pos, int len = -1)
{
    return str.mid(pos, len);
}
inline bool      IsNULL(const PGESTRING str)
{
    return str.isNull();
}
inline bool      IsEmpty(const PGESTRING &str)
{
    return str.isEmpty();
}
inline bool      IsEmpty(const PGESTRINGList &str)
{
    return str.isEmpty();
}
inline int       toInt(PGESTRING str)
{
    return str.toInt();
}
inline unsigned int toUInt(PGESTRING str)
{
    return str.toUInt();
}
inline long      toLong(PGESTRING str)
{
    return str.toLong();
}
inline unsigned long toULong(PGESTRING str)
{
    return str.toULong();
}
inline float     toFloat(PGESTRING str)
{
    return str.toFloat();
}
inline double    toDouble(PGESTRING str)
{
    return str.toDouble();
}
inline PGESTRING removeSpaces(PGESTRING src)
{
    return src.remove(' ');
}
template<typename T>
PGESTRING fromNum(T num)
{
    return QString::number(num);
}
inline PGESTRING fromBoolToNum(bool num)
{
    return QString::number(static_cast<int>(num));
}
namespace PGE_FileFormats_misc
{
    PGESTRING    url_encode(const PGESTRING &sSrc);
    std::string  base64_encode(uint8_t const *bytes_to_encode, size_t in_len, bool no_padding = false);
    std::string  base64_encode(std::string const &source, bool no_padding = false);
    std::string  base64_decode(std::string const &encoded_string);
    QString      base64_encode(QString &source, bool no_padding = false);
    QString      base64_decode(QString &source);
    QString      base64_encodeW(QString &source, bool no_padding = false);
    QString      base64_decodeW(QString &source);
    QString      base64_encodeA(QString &source, bool no_padding = false);
    QString      base64_decodeA(QString &source);
}
inline PGESTRING PGE_URLENC(const PGESTRING &src)
{
    return PGE_FileFormats_misc::url_encode(src);
}
inline PGESTRING PGE_URLDEC(const PGESTRING &src)
{
    /* Don't call fromPercentEncoding() on Windows with empty string,
     * or crash will happen! */
    if(IsEmpty(src))
        return PGESTRING();
    return QUrl::fromPercentEncoding(src.toUtf8());
}
#define PGE_BASE64ENC(src)   PGE_FileFormats_misc::base64_encode(src)
#define PGE_BASE64ENC_nopad(src)   PGE_FileFormats_misc::base64_encode(src, true)
#define PGE_BASE64DEC(src)   PGE_FileFormats_misc::base64_decode(src)
#define PGE_BASE64ENC_W(src) PGE_FileFormats_misc::base64_encodeW(src)
#define PGE_BASE64DEC_W(src) PGE_FileFormats_misc::base64_decodeW(src)
#define PGE_BASE64ENC_A(src) PGE_FileFormats_misc::base64_encodeA(src)
#define PGE_BASE64DEC_A(src) PGE_FileFormats_misc::base64_decodeA(src)
#else
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <cmath>
#include <unordered_map>
#define PGE_FILES_INGERED
typedef std::string PGESTRING;
typedef std::string::size_type pge_size_t;
inline PGESTRING PGESTR_Simpl(PGESTRING str)
{
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}
inline PGESTRING PGESTR_toLower(PGESTRING str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}
#define PGEGetChar(chr) chr
typedef char PGEChar;
typedef std::vector<std::string> PGESTRINGList;
#define PGEVECTOR std::vector
#define PGELIST std::vector
typedef size_t pge_size_t;
#define PGEPAIR std::pair
#define PGEMAP  std::map
#define PGEHASH std::unordered_map
typedef std::fstream PGEFILE;
namespace PGE_FileFormats_misc
{
    void split(std::vector<std::string> &dest, const std::string &str, const std::string& separator);
    void replaceAll(std::string &str, const std::string &from, const std::string &to);
    void RemoveSub(std::string &sInput, const std::string &sub);
    bool hasEnding(std::string const &fullString, std::string const &ending);
    PGESTRING url_encode(const PGESTRING &sSrc);
    PGESTRING url_decode(const std::string &sSrc);
    std::string base64_encode(unsigned char const *bytes_to_encode, size_t in_len, bool no_padding = false);
    std::string base64_encode(std::string const &source, bool no_padding = false);
    std::string base64_decode(std::string const &encoded_string);
    std::string base64_encodeW(std::string &source, bool no_padding = false);
    std::string base64_decodeW(std::string &source);
    std::string base64_encodeA(std::string &source, bool no_padding = false);
    std::string base64_decodeA(std::string &source);
}
inline void PGE_SPLITSTRING(PGESTRINGList &dst, const PGESTRING &src, const PGESTRING &sep)
{
    dst.clear();
    PGE_FileFormats_misc::split(dst, src, sep);
}
inline PGESTRING PGE_ReplSTRING(PGESTRING src, PGESTRING from, PGESTRING to)
{
    PGE_FileFormats_misc::replaceAll(src, from, to);
    return src;
}

inline PGESTRING PGE_RemSubSTRING(PGESTRING src, PGESTRING substr)
{
    PGE_FileFormats_misc::RemoveSub(src, substr);
    return src;
}
inline PGESTRING PGE_RemStrRng(PGESTRING &str, int pos, int len)
{
    str.erase(static_cast<size_t>(pos), static_cast<size_t>(len));
    return str;
}
inline PGESTRING PGE_SubStr(PGESTRING &str, std::string::size_type pos, std::string::size_type len = std::string::npos)
{
    return str.substr(pos, len);
}
inline bool IsNULL(const PGESTRING str)
{
    return (str.empty());
}
inline bool IsEmpty(const PGESTRING &str)
{
    return str.empty();
}
inline bool IsEmpty(const PGESTRINGList &str)
{
    return str.empty();
}
inline int toInt(PGESTRING str)
{
    return std::atoi(str.c_str());
}
inline unsigned int toUInt(PGESTRING str)
{
    return static_cast<unsigned int>(std::atol(str.c_str()));
}
inline long toLong(PGESTRING str)
{
    return std::atol(str.c_str());
}
inline unsigned long toULong(PGESTRING str)
{
    return static_cast<unsigned long>(std::atoll(str.c_str()));
}
inline float toFloat(PGESTRING str)
{
    return static_cast<float>(std::atof(str.c_str()));
}
inline double toDouble(PGESTRING str)
{
    return std::atof(str.c_str());
}
inline PGESTRING removeSpaces(PGESTRING src)
{
    return PGE_RemSubSTRING(src, " ");
}
template<typename T>
PGESTRING fromNum(T num)
{
    std::ostringstream n;
    n << num;
    return n.str();
}

inline PGESTRING fromBoolToNum(bool num)
{
    std::ostringstream n;
    n << static_cast<int>(num);
    return n.str();
}
#define PGE_URLENC(src) PGE_FileFormats_misc::url_encode(src)
#define PGE_URLDEC(src) PGE_FileFormats_misc::url_decode(src)
#define PGE_BASE64ENC(src)   PGE_FileFormats_misc::base64_encode(src)
#define PGE_BASE64ENC_nopad(src) PGE_FileFormats_misc::base64_encode(src, true)
#define PGE_BASE64DEC(src)   PGE_FileFormats_misc::base64_decode(src)
#define PGE_BASE64ENC_W(src) PGE_FileFormats_misc::base64_encodeW(src)
#define PGE_BASE64DEC_W(src) PGE_FileFormats_misc::base64_decodeW(src)
#define PGE_BASE64ENC_A(src) PGE_FileFormats_misc::base64_encodeA(src)
#define PGE_BASE64DEC_A(src) PGE_FileFormats_misc::base64_decodeA(src)
#endif

inline bool PGE_StartsWith(PGESTRING src, PGESTRING with)
{
#ifdef PGE_FILES_QT
    return src.startsWith(with, Qt::CaseSensitive);
#else
    return !src.compare(0, with.size(), with);
#endif
}

inline bool PGE_DetectSMBXFile(PGESTRING src)
{
    /*
     * First line of SMBX1...64 file must contain number from 0 to 64
     */
    src.push_back('\n');//Append double line feed to end (for case if sent first line only)
    src.push_back('\n');

    if((src.size() < 3))
        return false;//If line too short

    if((src[1] != '\n') && (src[2] != '\n') &&
       (src[1] != '\r') && (src[2] != '\r'))
        return false;//If line contains no line feeds (also possible CRLF)

    if((src[0] < '0') && (src[0] > '9'))
        return false;//If first character is not numeric

    if((src[1] != '\r') && (src[1] != '\n') && (src[1] < '0') && (src[1] > '9'))
        return false;//If second character is not numeric and is not line feed

    PGESTRING number;
    number.push_back(src[0]);

    if((src[1] != '\n') && (src[1] != '\r'))
        number.push_back(src[1]);

    int version = toInt(number);

    if(version > 64)  //Unsupported version number!
        return false;

    return true;
}

/*!
 * Misc I/O classes used by PGE File Library internally
 */
namespace PGE_FileFormats_misc
{
    /*!
     * \brief Provides cross-platform file path calculation for a file names or paths
     */
    class FileInfo
    {
        public:
            /*!
             * \brief Constructor
             */
            FileInfo();
            /*!
             * \brief Constructor with pre-opening of a file
             * \param filepath relative or absolute file path
             */
            FileInfo(PGESTRING filepath);
            /*!
             * \brief Sets file which will be used to calculate file information
             * \param filepath
             */
            void setFile(PGESTRING filepath);
            /*!
             * \brief Returns file extension (last part of filename after last dot)
             * \return file suffix or name extension (last part of filename after last dot)
             */
            PGESTRING suffix();
            /*!
             * \brief Returns full filename without path
             * \return full filename without path
             */
            PGESTRING filename();
            /*!
             * \brief Returns absolute path to file
             * \return absolute path to file
             */
            PGESTRING fullPath();
            /*!
             * \brief Returns base name part (first part of file name before first dot)
             * \return base name part (first part of file name before first dot)
             */
            PGESTRING basename();
            /*!
             * \brief Returns full directory path where actual file is located
             * \return full directory path where actual file is located
             */
            PGESTRING dirpath();
        private:
            /*!
             * \brief Recalculates all internal fields
             */
            void rebuildData();
            /*!
             * \brief Current filename
             */
            PGESTRING filePath;
            /*!
             * \brief Current filename without directory path
             */
            PGESTRING _filename;
            /*!
             * \brief File name suffix (last part of file name after last dot)
             */
            PGESTRING _suffix;
            /*!
             * \brief Base name (first part of file name before first dot)
             */
            PGESTRING _basename;
            /*!
             * \brief Full directory path where file is located
             */
            PGESTRING _dirpath;
    };


    class TextInput
    {
        public:
            /*!
             * \brief Relative positions of carriage
             */
            enum positions
            {
                //! Relative to current position
                current = 0,
                //! Relative to begin of file
                begin,
                //! Relative to end of file
                end
            };

            TextInput();
            virtual ~TextInput();
            virtual PGESTRING read(int64_t len);
            virtual PGESTRING readLine();
            virtual PGESTRING readCVSLine();
            virtual PGESTRING readAll();
            virtual bool eof();
            virtual int64_t tell();
            virtual int seek(int64_t pos, positions relativeTo);
            virtual PGESTRING getFilePath();
            virtual void setFilePath(PGESTRING path);
            virtual long getCurrentLineNumber();
        protected:
            PGESTRING _filePath;
            long  _lineNumber;
    };

    class TextOutput
    {
        public:
            /*!
             * \brief Relative positions of carriage
             */
            enum positions
            {
                //! Relative to current position
                current = 0,
                //! Relative to begin of file
                begin,
                //! Relative to end of file
                end
            };
            enum outputMode
            {
                truncate = 0,
                append,
                overwrite
            };

            TextOutput();
            virtual ~TextOutput();
            virtual int write(PGESTRING buffer);
            virtual int64_t tell();
            virtual int seek(int64_t pos, positions relativeTo);
            virtual PGESTRING getFilePath();
            virtual void setFilePath(PGESTRING path);
            virtual long getCurrentLineNumber();
            TextOutput &operator<<(const PGESTRING &s);
            TextOutput &operator<<(const char *s);
        protected:
            PGESTRING _filePath;
            long  _lineNumber;
    };


    class RawTextInput: public TextInput
    {
        public:
            RawTextInput();
            RawTextInput(PGESTRING *rawString, PGESTRING filepath = "");
            virtual ~RawTextInput();
            bool open(PGESTRING *rawString, PGESTRING filepath = "");
            void close();
            virtual PGESTRING read(int64_t len);
            virtual PGESTRING readLine();
            virtual PGESTRING readCVSLine();
            virtual PGESTRING readAll();
            virtual bool eof();
            virtual int64_t tell();
            virtual int seek(int64_t _pos, positions relativeTo);
        private:
            int64_t _pos;
            PGESTRING *_data;
            bool _isEOF;
    };

    class RawTextOutput: public TextOutput
    {
        public:
            RawTextOutput();
            RawTextOutput(PGESTRING *rawString, outputMode mode = truncate);
            virtual ~RawTextOutput();
            bool open(PGESTRING *rawString, outputMode mode = truncate);
            void close();
            int write(PGESTRING buffer);
            int64_t tell();
            int seek(int64_t _pos, positions relativeTo);
        private:
            long long _pos;
            PGESTRING *_data;
    };



    /*!
     * \brief Provides cross-platform text file reading interface
     */
    class TextFileInput: public TextInput
    {
        public:
            /*!
             * \brief Checks is requested file exist
             * \param filePath Full or relative path to the file
             * \return true if file exists
             */
            static bool exists(PGESTRING filePath);
            /*!
             * \brief Constructor
             */
            TextFileInput();
            /*!
             * \brief Constructor with pre-opening of the file
             * \param filePath Full or relative path to the file
             * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
             */
            TextFileInput(PGESTRING filePath, bool utf8 = false);
            /*!
             * \brief Destructor
             */
            virtual ~TextFileInput();
            /*!
             * \brief Opening of the file
             * \param filePath Full or relative path to the file
             * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
             */
            bool open(PGESTRING filePath, bool utf8 = false);
            /*!
             * \brief Close currently opened file
             */
            void close();
            /*!
             * \brief Reads requested number of characters from a file
             * \param Maximal lenght of characters to read from file
             * \return string contains requested line of characters
             */
            PGESTRING read(int64_t len);
            /*!
             * \brief Reads whole line before line feed character
             * \return string contains gotten line
             */
            PGESTRING readLine();
            /*!
             * \brief Reads whole line before line feed character or before first unquoted comma
             * \return string contains gotten line
             */
            PGESTRING readCVSLine();
            /*!
             * \brief Reads all data from a file at current position of carriage
             * \return
             */
            PGESTRING readAll();
            /*!
             * \brief Is carriage position at end of file
             * \return true if carriage position at end of file
             */
            bool eof();
            /*!
             * \brief Returns current position of carriage relative to begin of file
             * \return current position of carriage relative to begin of file
             */
            int64_t tell();
            /*!
             * \brief Changes position of carriage to specific file position
             * \param pos Target position of carriage
             * \param relativeTo defines relativity of target position of carriage (current position, begin of file or end of file)
             */
            int seek(int64_t pos, positions relativeTo);
        private:
#ifdef PGE_FILES_QT
            //! File handler used in Qt version of PGE file Library
            QFile file;
            //! File input stream used in Qt version of PGE file Library
            QTextStream stream;
#else
            //! File input stream used in STL version of PGE file Library
            FILE *stream;
#endif
    };


    class TextFileOutput: public TextOutput
    {
        public:
            /*!
             * \brief Checks is requested file exist
             * \param filePath Full or relative path to the file
             * \return true if file exists
             */
            static bool exists(PGESTRING filePath);
            /*!
             * \brief Constructor
             */
            TextFileOutput();
            /*!
             * \brief Constructor with pre-opening of the file
             * \param filePath Full or relative path to the file
             * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
             */
            TextFileOutput(PGESTRING filePath, bool utf8 = false, bool forceCRLF = false, outputMode mode = truncate);
            /*!
             * \brief Destructor
             */
            virtual ~TextFileOutput();
            /*!
             * \brief Opening of the file
             * \param filePath Full or relative path to the file
             * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
             */
            bool open(PGESTRING filePath, bool utf8 = false, bool forceCRLF = false, outputMode mode = truncate);
            /*!
             * \brief Close currently opened file
             */
            void close();
            /*!
             * \brief Reads requested number of characters from a file
             * \param Maximal lenght of characters to read from file
             * \return string contains requested line of characters
             */
            int write(PGESTRING buffer);
            /*!
             * \brief Returns current position of carriage relative to begin of file
             * \return current position of carriage relative to begin of file
             */
            int64_t tell();
            /*!
             * \brief Changes position of carriage to specific file position
             * \param pos Target position of carriage
             * \param relativeTo defines relativity of target position of carriage (current position, begin of file or end of file)
             */
            int seek(int64_t pos, positions relativeTo);
        private:
            bool m_forceCRLF;
#ifdef PGE_FILES_QT
            //! File handler used in Qt version of PGE file Library
            QFile file;
            //! File input stream used in Qt version of PGE file Library
            QTextStream stream;
#else
            //! File input stream used in STL version of PGE file Library
            FILE *stream;
#endif
    };

}

#endif // PGE_FILE_LIB_GLOBS_H_
