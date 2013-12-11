// ---------------------------------------------------------------------
//
// qtgrader  -  the interactive high speed grading assistant
// Copyright (C) 2012-2013 Wayne Piekarski
// wayne@tinmith.net http://tinmith.net/wayne
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ---------------------------------------------------------------------

#pragma once

#include <stdio.h>
#include <QMessageBox>

// Leave error checking turned on permanently
#define QTGRADER_DEBUG

#ifndef QTGRADER_DEBUG
#define GDEBUG(fmt, ...)
#else
#define GDEBUG(fmt, ...) fprintf (stderr, "DEBUG %s:%d - " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__)
#endif
#define GFATAL(fmt, ...) fprintf (stderr, "FATAL %s:%d - " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__), fflush(stderr), GEXIT(1)

#ifndef QTGRADER_DEBUG
#define GASSERT(cond, fmt, ...)
#else
#define __GASSERT_STR(cond) #cond
#define GASSERT(cond, fmt, ...) if (!(cond)) { GFATAL("Assertion %s failed - " fmt, __GASSERT_STR(cond), ## __VA_ARGS__); }
#endif

void _GEXITDIALOG (QString str, const char *file, int line);
#define GEXITDIALOG(str) _GEXITDIALOG(str, __FILE__, __LINE__)
void _GINFODIALOG (QString str, const char *file, int line);
#define GINFODIALOG(str) _GINFODIALOG(str, __FILE__, __LINE__)

void GEXIT (int result);
