#include "gradewindow.h"
#include "ui_gradewindow.h"
#include "global.h"

GradeWindow::GradeWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GradeWindow)
{
  ui->setupUi(this);
  ui->tableWidget->setColumnCount(Global::getNumQuestions()+2);
  ui->tableWidget->setRowCount(Global::getNumStudents());
  ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("StudentID")));
  ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Name")));

  for (size_t r = 0; r < Global::getNumStudents(); r++)
  {
    ui->tableWidget->setVerticalHeaderItem(r, new QTableWidgetItem(QString("%1").arg(r)));
    ui->tableWidget->setItem(r, 0, new QTableWidgetItem("StudentID"));
    ui->tableWidget->setItem(r, 1, new QTableWidgetItem("StudentName"));
    for (size_t c = 0; c < Global::getNumQuestions(); c++)
    {
      if (r == 0)
        ui->tableWidget->setHorizontalHeaderItem(c+2, new QTableWidgetItem(QString("Q%1").arg(c+1)));
      ui->tableWidget->setItem(r, c+2, new QTableWidgetItem("N/A"));
    }
  }

  ui->tableWidget->resizeColumnsToContents();
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

void GradeWindow::update()
{
  ui->tableWidget->setColumnCount(10);
  ui->tableWidget->setRowCount(5);
  ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Q1"));
  ui->tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem("0"));
  ui->tableWidget->setItem(0, 0, new QTableWidgetItem("Hello"));
  ui->tableWidget->setItem(1, 0, new QTableWidgetItem());
  ui->tableWidget->item(1,0)->setText("World");
}
