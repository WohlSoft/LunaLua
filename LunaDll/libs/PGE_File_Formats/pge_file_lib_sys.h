#pragma once
#ifndef PGE_FILE_LIB_SYS_H_
#define PGE_FILE_LIB_SYS_H_

/*!
 * \file pge_file_lib_sys.h
 * \brief Contains internally used global haders lists
 *
 */

#ifdef PGE_FILES_QT
#include <QRegExp>
#include <QFileInfo>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QtDebug>
#else
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */
#endif

#endif // PGE_FILE_LIB_SYS_H_
