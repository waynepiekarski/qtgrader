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

#include "debug.h"

void _GEXITDIALOG (QString str, const char *file, int line)
{
  QMessageBox error;
  GDEBUG("About to exit for [%s] from %s:%d", qPrintable(str), file, line);
  error.setText(QString("Fatal Error at %1:%2\n\n%3").arg(file).arg(line).arg(str));
  error.exec();
  GDEBUG("User acknowledged exit for [%s]", qPrintable(str));
  GEXIT(0);
}

void _GINFODIALOG (QString str, const char *file, int line)
{
  QMessageBox error;
  GDEBUG("Notify dialog for [%s] from %s:%d", qPrintable(str), file, line);
  error.setText(QString("Message at %1:%2\n\n%3").arg(file).arg(line).arg(str));
  error.exec();
  GDEBUG("User acknowledged notify dialog for [%s]", qPrintable(str));
}

void GEXIT (int result)
{
  GDEBUG ("Exiting with result %d", result);
  exit(result);
}
