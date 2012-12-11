#include "gradewindow.h"
#include "ui_gradewindow.h"
#include "global.h"

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

void GradeWindow::update()
{
  ui->tableWidget->setColumnCount(2+2*Global::getNumQuestions());
  ui->tableWidget->setRowCount(Global::getNumStudents() + 1);
  ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("StudentID")));
  ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("StudentName")));

  ui->tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(QString("%1").arg(-1)));
  ui->tableWidget->setItem(0, 0, new QTableWidgetItem("MaximumId"));
  ui->tableWidget->setItem(0, 1, new QTableWidgetItem("MaximumName"));
  for (size_t q = 0; q < Global::getNumQuestions(); q++)
  {
    ui->tableWidget->setHorizontalHeaderItem(q+2, new QTableWidgetItem(QString("Q%1").arg(q+1)));
    ui->tableWidget->setHorizontalHeaderItem(q+2+Global::getNumQuestions(), new QTableWidgetItem(QString("Feed%1").arg(q+1)));
    ui->tableWidget->setItem(0, q + 2, new QTableWidgetItem(Database::getStrFromGrade(Global::db()->getQuestionMaximum(q))));
    ui->tableWidget->setItem(0, q + 2 + Global::getNumQuestions(), new QTableWidgetItem("NF"));
  }
  for (size_t s = 0; s < Global::getNumStudents(); s++)
  {
    Student& student = Global::db()->getStudent(s);
    ui->tableWidget->setVerticalHeaderItem(s+1, new QTableWidgetItem(QString("%1").arg(s)));
    ui->tableWidget->setItem(s+1, 0, new QTableWidgetItem(student.getStudentId()));
    ui->tableWidget->setItem(s+1, 1, new QTableWidgetItem(student.getStudentName()));
    for (size_t q = 0; q < Global::getNumQuestions(); q++)
    {
      ui->tableWidget->setItem(s+1, q + 2, new QTableWidgetItem(Database::getStrFromGrade(student.getGrade(q))));
      ui->tableWidget->setItem(s+1, q + 2 + Global::getNumQuestions(), new QTableWidgetItem(student.getFeedback(q)));
    }
  }
  ui->tableWidget->resizeColumnsToContents();

  // Prevent edits to any of the cells
  for (int row = 0; row < ui->tableWidget->rowCount(); row++)
    for (int col = 0; col < ui->tableWidget->columnCount(); col++)
    {
      QTableWidgetItem *i = ui->tableWidget->item(row, col);
      GASSERT(i, "Could not find table item for row %d col %d", row, col);
      i->setFlags(i->flags() ^ Qt::ItemIsEditable);
    }
}

void GradeWindow::update(size_t curStudent, size_t curQuestion)
{
  GDEBUG ("Not finished yet");
  update();

  // Show which question is currently being edited
  ui->tableWidget->setCurrentCell(curStudent+1, curQuestion+2);
}
