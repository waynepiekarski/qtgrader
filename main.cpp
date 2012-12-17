#include "mainwindow.h"
#include "gradewindow.h"
#include "startup.h"
#include "global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Global::setQApplication(&a);

  /* Query for what startup options we will use */
  StartupDialog sd;

  MainWindow w;
  w.showMaximized();

  /* Set initial document zoom, can only do it here since the window is now visible
   * and it can compute its size for the scaling factor */
  w.handleZoomHeight();

  return a.exec();
}
