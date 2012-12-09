#pragma once

#include "database.h"
#include "debug.h"

class Pages;

class Global
{
public:
  static void initDatabase(size_t numStudents, size_t numQuestions, size_t numPagesPerStudent)
  {
    GASSERT(_db == NULL, "Cannot initialize database twice");
    _numPagesPerStudent = numPagesPerStudent;
    _db = new Database(numStudents, numQuestions);
  }

  static void initPages(QString path)
  {
    GASSERT(_pages == NULL, "Cannot initialize pages twice");
    _pages = new Pages(path);
  }

  static Database* getDatabase() { GASSERT(_db, "DB not initialized"); return _db; }
  static Pages* getPages() { GASSERT(_pages, "Pages not initialized"); return _pages; }

  static size_t getNumPages() { return getPages()->size(); }
  static size_t getNumStudents() { return getDatabase()->getNumStudents(); }
  static size_t getNumQuestions() { return getDatabase()->getNumQuestions(); }
  static size_t getNumPagesPerStudent()
  {
    GASSERT(getNumPages() % getNumStudents() == 0, "%zu pages do not align with %zu students", getNumPages(), getNumStudents());
    return (getNumPages() / getNumStudents());
  }
  static QPixmap& getQPixmap(size_t page) { return getPages()->getQPixmap(page); }

private:
  static Database *_db;
  static Pages *_pages;
  static size_t _numPagesPerStudent;
};
