#pragma once

#include <stdio.h>
#include <QMessageBox>

#ifdef NDEBUG
#define GDEBUG(fmt, ...)
#else
#define GDEBUG(fmt, ...) fprintf (stderr, "DEBUG %s:%d - " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__)
#endif
#define GFATAL(fmt, ...) fprintf (stderr, "FATAL %s:%d - " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__), fflush(stderr), GEXIT(1)

#ifdef NDEBUG
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
