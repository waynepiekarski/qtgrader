#include "database.h"
#include "pages.h"
#include "global.h"

Database* Global::_db = NULL;
Pages* Global::_pages = NULL;
size_t Global::_numPagesPerStudent = 0;
