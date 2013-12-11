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

#include "database.h"
#include "debug.h"

class MainWindow;
class GradeWindow;
class Pages;

#define DB_VERSION "DB v2.0"

class Global
{
public:
  static void initDatabase(size_t numStudents, size_t numQuestions, size_t numPagesPerStudent)
  {
    GASSERT(_db == NULL, "Cannot initialize database twice");
    _numPagesPerStudent = numPagesPerStudent;
    _db = new Database(numStudents, numQuestions);
  }
  static void initDatabase(QString filename);
  static void save(QString filename);
  static void generatePDFs(QString dirname);

  static void initPages(QString path)
  {
    GASSERT(_pages == NULL, "Cannot initialize pages twice");
    _pages = new Pages(path);
  }

  static Database* db() { return getDatabase(); }
  static Database* getDatabase() { GASSERT(_db, "DB not initialized"); return _db; }
  static Pages* getPages() { GASSERT(_pages, "Pages not initialized"); return _pages; }
  static QString& getPath() { return getPages()->getPath(); }

  static size_t getNumPages() { return getPages()->size(); }
  static size_t getNumStudents() { return getDatabase()->getNumStudents(); }
  static size_t getNumQuestions() { return getDatabase()->getNumQuestions(); }
  static size_t getNumPagesPerStudent()
  {
    GASSERT(getNumPages() % getNumStudents() == 0, "%zu pages do not align with %zu students", getNumPages(), getNumStudents());
    return (getNumPages() / getNumStudents());
  }
  static QPixmap getQPixmap(size_t page)
  {
    return getPages()->getQPixmap(page);
  }

  static void setGradeWindow(GradeWindow *in) { GASSERT(!_gw, "GradeWindow is already set"); _gw = in; }
  static GradeWindow* getGradeWindow() { GASSERT(_gw, "GradeWindow is not set"); return _gw; }
  static GradeWindow* gw() { return getGradeWindow(); }

  static void setMainWindow(MainWindow *in) { GASSERT(!_mw, "MainWindow is already set"); _mw = in; }
  static MainWindow* getMainWindow() { GASSERT(_mw, "MainWindow is not set"); return _mw; }
  static MainWindow* mw() { return getMainWindow(); }

  static void setStatusLabel(QLabel *in) { GASSERT(!_statusLabel, "Status label is already set"); _statusLabel = in; }
  static QLabel* getStatusLabel() { GASSERT(_statusLabel, "Status label is not set"); return _statusLabel; }

  static void setQApplication(QApplication *in) { GASSERT(!_qapp, "QApplication is already set"); _qapp = in; }
  static QApplication* getQApplication() { GASSERT(_qapp, "QApplication is not set"); return _qapp; }

private:
  static QApplication* _qapp;
  static MainWindow* _mw;
  static GradeWindow* _gw;
  static QLabel* _statusLabel;
  static Database* _db;
  static Pages* _pages;
  static size_t _numPagesPerStudent;
};
