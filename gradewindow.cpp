#include "gradewindow.h"
#include "ui_gradewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"

#define RESERVED_COL 3
#define RESERVED_ROW 2

GradeWindow::GradeWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GradeWindow)
{
  ui->setupUi(this);
  handleGradeWindow(true); // Make sure the table widget initially appears in the main window
  connect(ui->tableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(handleCellChanged(int,int,int,int)));
  connect(ui->actionResizeContents, SIGNAL(triggered()), this, SLOT(handleResizeContents()));
  connect(ui->actionResizeLarge, SIGNAL(triggered()), this, SLOT(handleResizeLarge()));
  connect(ui->actionResizeSmall, SIGNAL(triggered()), this, SLOT(handleResizeSmall()));

  ui->tableWidget->setColumnCount(RESERVED_COL+Global::getNumQuestions());
  ui->tableWidget->setRowCount(RESERVED_ROW+Global::getNumStudents());
  ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("StudentID")));
  ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("StudentName")));
  ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("Total")));

  ui->tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem("Pg"));
  ui->tableWidget->setItem(0, 0, new QTableWidgetItem("-"));
  ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Page"));
  ui->tableWidget->setItem(0, 2, new QTableWidgetItem("-"));

  ui->tableWidget->setVerticalHeaderItem(1, new QTableWidgetItem("Mx"));
  ui->tableWidget->setItem(1, 0, new QTableWidgetItem("-"));
  ui->tableWidget->setItem(1, 1, new QTableWidgetItem("Maximum"));
  ui->tableWidget->setItem(1, 2, new QTableWidgetItem(getStrFromGrade(Global::db()->getTotalMaximum())));

  for (size_t q = 0; q < Global::getNumQuestions(); q++)
  {
    ui->tableWidget->setHorizontalHeaderItem(q+RESERVED_COL, new QTableWidgetItem(QString("Q%1").arg(q+1)));
    ui->tableWidget->setItem(0, q+RESERVED_COL, new QTableWidgetItem(getStrFromGrade(Global::db()->getQuestionPage(q))));
    ui->tableWidget->setItem(1, q+RESERVED_COL, new QTableWidgetItem(getStrFromGrade(Global::db()->getQuestionMaximum(q))));
  }
  for (size_t s = 0; s < Global::getNumStudents(); s++)
  {
    Student& student = Global::db()->getStudent(s);
    ui->tableWidget->setVerticalHeaderItem(s+RESERVED_ROW, new QTableWidgetItem(QString("%1").arg(s+1)));
    ui->tableWidget->setItem(s+RESERVED_ROW, 0, new QTableWidgetItem(student.getStudentId()));
    ui->tableWidget->setItem(s+RESERVED_ROW, 1, new QTableWidgetItem(student.getStudentName()));
    ui->tableWidget->setItem(s+RESERVED_ROW, 2, new QTableWidgetItem(getStrFromGrade(student.getTotal())));
    for (size_t q = 0; q < Global::getNumQuestions(); q++)
    {
      ui->tableWidget->setItem(s+RESERVED_ROW, q+RESERVED_COL, new QTableWidgetItem());
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
  for (int col = RESERVED_COL; col < ui->tableWidget->columnCount(); col++)
    ui->tableWidget->setColumnWidth(col, 50);
}

void GradeWindow::handleResizeLarge()
{
  ui->tableWidget->resizeColumnsToContents();
  for (int col = RESERVED_COL; col < ui->tableWidget->columnCount(); col++)
    ui->tableWidget->setColumnWidth(col, 100);
}

GradeWindow::~GradeWindow()
{
  delete ui;
}

void GradeWindow::handleCellChanged(int row, int col, int prevRow, int prevCol)
{
  Q_UNUSED(row);
  Q_UNUSED(col);
  Q_UNUSED(prevRow);
  Q_UNUSED(prevCol);
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
  if ((row >= 1) && (col >= RESERVED_COL))
  {
    GDEBUG("In active region");
    size_t student = row - 1;
    size_t q = col - 3;
    GASSERT(q < Global::getNumQuestions(), "Q value %zu is not valid", q);
    GDEBUG("Student %zu, Question %zu", student+1, q+1);
  }
  */
}

void GradeWindow::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  handleGradeWindow(true);
}

void GradeWindow::handleGradeWindow(bool init)
{
  // During initialization, we need to put the table widget inside the main window
  if (isVisible() || init)
  {
    ui->tableWidget->setParent(Global::getMainWindow()->getUI()->gradeWidget);
    Global::getMainWindow()->getUI()->gridLayout->addWidget(ui->tableWidget, 0, 0, 1, 1);
    hide(); // Hide the pop up window
  }
  else
  {
    ui->tableWidget->setParent(ui->scrollAreaWidgetContents);
    ui->gridLayout->addWidget(ui->tableWidget, 0, 0, 1, 1);
    show(); // Show the pop up window to hold this widget
  }
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
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 2);
  return i->text();
}

QString GradeWindow::getGradeFeedback(size_t student, size_t question)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, question+RESERVED_COL);
  return i->text();
}

QString GradeWindow::getQuestionPage(size_t question)
{
  QTableWidgetItem *i = getItem(0, question+RESERVED_COL);
  return i->text();
}

QString GradeWindow::getMaximum(size_t question)
{
  QTableWidgetItem *i = getItem(1, question+RESERVED_COL);
  return i->text();
}

QString GradeWindow::getStudentId(size_t student)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 0);
  return i->text();
}

QString GradeWindow::getStudentName(size_t student)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 1);
  return i->text();
}

QString GradeWindow::getTotalMaximum()
{
  QTableWidgetItem *i = getItem(0, 2);
  return i->text();
}

void GradeWindow::setTotalGrade(size_t student, int total)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 2);
  i->setText(getStrFromGrade(total));
}

void GradeWindow::setGradeFeedback(size_t student, size_t question, int grade, const QString& feedback)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, question+RESERVED_COL);
  if (feedback.length() == 0)
    i->setText(getStrFromGrade(grade));
  else
    i->setText(QString("%1=%2").arg(getStrFromGrade(grade)).arg(feedback));
}

void GradeWindow::setQuestionPage(size_t question, int in)
{
  QTableWidgetItem *i = getItem(0, question+RESERVED_COL);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setMaximum(size_t question, int in)
{
  QTableWidgetItem *i = getItem(1, question+RESERVED_COL);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setTotalMaximum(int in)
{
  QTableWidgetItem *i = getItem(0, 2);
  i->setText(getStrFromGrade(in));
}

void GradeWindow::setStudentId(size_t student, const QString& in)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 0);
  i->setText(in);
}

void GradeWindow::setStudentName(size_t student, const QString& in)
{
  QTableWidgetItem *i = getItem(student+RESERVED_ROW, 1);
  i->setText(in);
}

void GradeWindow::update(size_t curStudent, size_t curQuestion)
{
  // Show which question is currently being edited
  ui->tableWidget->setCurrentCell(curStudent+RESERVED_ROW, curQuestion+RESERVED_COL);
}
