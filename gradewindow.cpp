#include "gradewindow.h"
#include "ui_gradewindow.h"
#include "global.h"

#define RESERVED 3

GradeWindow::GradeWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GradeWindow)
{
  ui->setupUi(this);
  connect(ui->tableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(handleCellChanged(int,int,int,int)));
  connect(ui->actionResizeContents, SIGNAL(triggered()), this, SLOT(handleResizeContents()));
  connect(ui->actionResizeLarge, SIGNAL(triggered()), this, SLOT(handleResizeLarge()));
  connect(ui->actionResizeSmall, SIGNAL(triggered()), this, SLOT(handleResizeSmall()));

  ui->tableWidget->setColumnCount(RESERVED+Global::getNumQuestions());
  ui->tableWidget->setRowCount(Global::getNumStudents() + 1);
  ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("StudentID")));
  ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("StudentName")));
  ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("Total")));

  ui->tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(QString("%1").arg(-1)));
  ui->tableWidget->setItem(0, 0, new QTableWidgetItem("-"));
  ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Maximum"));
  ui->tableWidget->setItem(0, 2, new QTableWidgetItem(getStrFromGrade(Global::db()->getTotalMaximum())));

  for (size_t q = 0; q < Global::getNumQuestions(); q++)
  {
    ui->tableWidget->setHorizontalHeaderItem(q+RESERVED, new QTableWidgetItem(QString("Q%1").arg(q+1)));
    ui->tableWidget->setItem(0, q+RESERVED, new QTableWidgetItem(getStrFromGrade(Global::db()->getQuestionMaximum(q))));
  }
  for (size_t s = 0; s < Global::getNumStudents(); s++)
  {
    Student& student = Global::db()->getStudent(s);
    ui->tableWidget->setVerticalHeaderItem(s+1, new QTableWidgetItem(QString("%1").arg(s)));
    ui->tableWidget->setItem(s+1, 0, new QTableWidgetItem(student.getStudentId()));
    ui->tableWidget->setItem(s+1, 1, new QTableWidgetItem(student.getStudentName()));
    ui->tableWidget->setItem(s+1, 2, new QTableWidgetItem(getStrFromGrade(student.getTotal())));
    for (size_t q = 0; q < Global::getNumQuestions(); q++)
    {
      ui->tableWidget->setItem(s+1, q+RESERVED, new QTableWidgetItem());
      setGradeFeedback(student.getSeq(), q, student.getGrade(q), student.getFeedback(q));
    }
  }

  // Resize so each cell can see just the grade value
  handleResizeSmall();

  // Prevent edits to any of the cells
  for (int row = 0; row < ui->tableWidget->rowCount(); row++)
    for (int col = 0; col < ui->tableWidget->columnCount(); col++)
    {
      QTableWidgetItem *i = ui->tableWidget->item(row, col);
      GASSERT(i, "Could not find table item for row %d col %d", row, col);
      i->setFlags(i->flags() ^ Qt::ItemIsEditable);
    }
}

void GradeWindow::handleResizeContents()
{
  ui->tableWidget->resizeColumnsToContents();
}

void GradeWindow::handleResizeSmall()
{
  ui->tableWidget->resizeColumnsToContents();
  for (int col = RESERVED; col < ui->tableWidget->columnCount(); col++)
    ui->tableWidget->setColumnWidth(col, 50);
}

void GradeWindow::handleResizeLarge()
{
  ui->tableWidget->resizeColumnsToContents();
  for (int col = RESERVED; col < ui->tableWidget->columnCount(); col++)
    ui->tableWidget->setColumnWidth(col, 100);
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
  if ((row >= 1) && (col >= RESERVED))
  {
    GDEBUG("In active region");
    size_t student = row - 1;
    size_t q = col - 3;
    GASSERT(q < Global::getNumQuestions(), "Q value %zu is not valid", q);
    GDEBUG("Student %zu, Question %zu", student+1, q+1);
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

QString GradeWindow::getTotalGrade(size_t student)
{
  QTableWidgetItem *i = getItem(student+1, 2);
  return i->text();
}

QString GradeWindow::getGradeFeedback(size_t student, size_t question)
{
  QTableWidgetItem *i = getItem(student+1, question+RESERVED);
  return i->text();
}

QString GradeWindow::getMaximum(size_t question)
{
  QTableWidgetItem *i = getItem(0, question+RESERVED);
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

QString GradeWindow::getTotalMaximum()
{
  QTableWidgetItem *i = getItem(0, 2);
  return i->text();
}

void GradeWindow::setTotalGrade(size_t student, int total)
{
  QTableWidgetItem *i = getItem(student+1, 2);
  i->setText(getStrFromGrade(total));
}

void GradeWindow::setGradeFeedback(size_t student, size_t question, int grade, const QString& feedback)
{
  QTableWidgetItem *i = getItem(student+1, question+RESERVED);
  if (feedback.length() == 0)
    i->setText(getStrFromGrade(grade));
  else
    i->setText(QString("%1=%2").arg(getStrFromGrade(grade)).arg(feedback));
}

void GradeWindow::setMaximum(size_t question, int in)
{
  QTableWidgetItem *i = getItem(0, question+RESERVED);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setTotalMaximum(int in)
{
  QTableWidgetItem *i = getItem(0, 2);
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
  ui->tableWidget->setCurrentCell(curStudent+1, curQuestion+RESERVED);
}
