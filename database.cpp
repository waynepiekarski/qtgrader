#include "gradewindow.h"
#include "global.h"
#include "database.h"

GradeWindow *Student::gw() { return Global::gw(); }

GradeWindow *Database::gw() { return Global::gw(); }
