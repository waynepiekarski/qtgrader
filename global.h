#pragma once

#include "database.h"
#include "debug.h"

class Pages;

class Global
{
public:
  static void initDatabase(size_t numStudents, size_t numQuestions)
  {
    GASSERT(_db == NULL, "Cannot initialize database twice");
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
  static QPixmap& getQPixmap(size_t page) { return getPages()->getQPixmap(page); }

private:
  static Database *_db;
  static Pages *_pages;
};
