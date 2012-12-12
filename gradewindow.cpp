#include "gradewindow.h"
#include "ui_gradewindow.h"
#include "global.h"

GradeWindow::GradeWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GradeWindow)
{
  ui->setupUi(this);
  connect(ui->tableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(handleCellChanged(int,int,int,int)));

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
    ui->tableWidget->setItem(0, q + 2, new QTableWidgetItem(getStrFromGrade(Global::db()->getQuestionMaximum(q))));
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
      ui->tableWidget->setItem(s+1, q + 2, new QTableWidgetItem(getStrFromGrade(student.getGrade(q))));
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

GradeWindow::~GradeWindow()
{
  delete ui;
}

void GradeWindow::handleCellChanged(int row, int col, int prevRow, int prevCol)
{
  // It appears to be difficult to prevent cell changes from occurring, this code doesn't work
  /*
  bool changed = false;
  if (row < 1)
  {
    changed = true;
    row = 1;
  }
  if (col < 2)
  {
    changed = true;
    col = 2;
  }
  if (changed)
    ui->tableWidget->setCurrentCell(row, col);
  */

  /* Some testing code to detect changes, but it is also affected by the other parts of the UI */
  /*
  if ((row >= 1) && (col >= 3))
  {
    GDEBUG("In active region");
    size_t student = row - 1;
    size_t q = col - 3;
    if (q < Global::getNumQuestions())
      GDEBUG("Student %zu, Question %zu", student+1, q+1);
    else if (q < Global::getNumQuestions()*2)
      GDEBUG("Student %zu, Feedback %zu", student+1, q-Global::getNumQuestions()+1);
    GASSERT(q < 2*Global::getNumQuestions(), "Q value %zu is not valid", q);
  }
  */
}

void GradeWindow::handleGradeWindow()
{
  if (isVisible())
    hide();
  else
    show();
}

QTableWidgetItem* GradeWindow::getItem(int row, int col)
{
  GASSERT((row >= 0) && (row < ui->tableWidget->rowCount()), "Row %d is not within the table with %d rows", row, ui->tableWidget->rowCount());
  GASSERT((col >= 0) && (col < ui->tableWidget->columnCount()), "Column %d is not within the table with %d columns", col, ui->tableWidget->columnCount());
  QTableWidgetItem *i = ui->tableWidget->item(row, col);
  GASSERT(i, "Could not find widget item at row %d col %d", row, col);
  return i;
}

QString GradeWindow::getGrade(size_t student, size_t question)
{
  QTableWidgetItem *i = getItem(student+1, question+2);
  return i->text();
}

QString GradeWindow::getFeedback(size_t student, size_t question)
{
  QTableWidgetItem *i = getItem(student+1, question+2+Global::getNumQuestions());
  return i->text();
}

QString GradeWindow::getMaximum(size_t question)
{
  QTableWidgetItem *i = getItem(0, question+2);
  return i->text();
}

QString GradeWindow::getStudentId(size_t student)
{
  QTableWidgetItem *i = getItem(student+1, 0);
  return i->text();
}

QString GradeWindow::getStudentName(size_t student)
{
  QTableWidgetItem *i = getItem(student+1, 1);
  return i->text();
}

void GradeWindow::setGrade(size_t student, size_t question, int in)
{
  QTableWidgetItem *i = getItem(student+1, question+2);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setFeedback(size_t student, size_t question, const QString& in)
{
  QTableWidgetItem *i = getItem(student+1, question+2+Global::getNumQuestions());
  i->setText(in);
}

void GradeWindow::setMaximum(size_t question, int in)
{
  QTableWidgetItem *i = getItem(0, question+2);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setStudentId(size_t student, const QString& in)
{
  QTableWidgetItem *i = getItem(student+1, 0);
  i->setText(in);
}

void GradeWindow::setStudentName(size_t student, const QString& in)
{
  QTableWidgetItem *i = getItem(student+1, 1);
  i->setText(in);
}

void GradeWindow::update(size_t curStudent, size_t curQuestion)
{
  // Show which question is currently being edited
  ui->tableWidget->setCurrentCell(curStudent+1, curQuestion+2);
}
