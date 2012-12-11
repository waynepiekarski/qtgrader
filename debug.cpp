#include "debug.h"

void GEXITDIALOG (QString str)
{
  QMessageBox error;
  GDEBUG("About to exit for [%s]", qPrintable(str));
  error.setText(str);
  error.exec();
  GDEBUG("User acknowledged exit for [%s]", qPrintable(str));
  GEXIT(0);
}

void GINFODIALOG (QString str)
{
  QMessageBox error;
  GDEBUG("Notify dialog for [%s]", qPrintable(str));
  error.setText(str);
  error.exec();
  GDEBUG("User acknowledged notify dialog for [%s]", qPrintable(str));
}

void GEXIT (int result)
{
  GDEBUG ("Exiting with result %d", result);
  exit(result);
}
