#include "gradewindow.h"
#include "ui_gradewindow.h"

GradeWindow::GradeWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GradeWindow)
{
  ui->setupUi(this);
}

GradeWindow::~GradeWindow()
{
  delete ui;
}

void GradeWindow::handleGradeWindow()
{
  if (isVisible())
    hide();
  else
    show();
}
