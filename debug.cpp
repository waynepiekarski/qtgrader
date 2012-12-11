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
