#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_gradewindow.h"
#include "database.h"
#include "global.h"
#include <QDir>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    curPage(0),
    curQuestion(0),
    zoomFactor(1.0)
{
  /* Set up the Qt user interface */
  ui->setupUi(this);

  /* Set up the GradeWindow and also fill the table with initial values from the database */
  Global::setGradeWindow(new GradeWindow);

  /* Set up page change buttons */
  connect(ui->pageLeft ,   SIGNAL(clicked()), this, SLOT(handlePagePrev()));
  connect(ui->pageRight,   SIGNAL(clicked()), this, SLOT(handlePageNext()));
  connect(ui->studentPrev, SIGNAL(clicked()), this, SLOT(handleStudentPrev()));
  connect(ui->studentNext, SIGNAL(clicked()), this, SLOT(handleStudentNext()));
  connect(ui->questionPrev, SIGNAL(clicked()), this, SLOT(handleQuestionPrev()));
  connect(ui->questionNext, SIGNAL(clicked()), this, SLOT(handleQuestionNext()));
  connect(ui->zoomIn,     SIGNAL(clicked()), this, SLOT(handleZoomIn()));
  connect(ui->zoomOut,    SIGNAL(clicked()), this, SLOT(handleZoomOut()));
  connect(ui->zoomWidth,  SIGNAL(clicked()), this, SLOT(handleZoomWidth()));
  connect(ui->zoomHeight, SIGNAL(clicked()), this, SLOT(handleZoomHeight()));
  connect(ui->zoomOne,    SIGNAL(clicked()), this, SLOT(handleZoomOne()));

  /* Set up various actions */
  connect(ui->actionSave ,    SIGNAL(triggered()), this, SLOT(handleSave()));
  connect(Global::gw()->getUI()->actionSave, SIGNAL(triggered()), this, SLOT(handleSave()));
  connect(ui->actionPagePrev, SIGNAL(triggered()), this, SLOT(handlePagePrev()));
  connect(ui->actionPageNext, SIGNAL(triggered()), this, SLOT(handlePageNext()));
  connect(ui->actionStudentPrev, SIGNAL(triggered()), this, SLOT(handleStudentPrev()));
  connect(ui->actionStudentNext, SIGNAL(triggered()), this, SLOT(handleStudentNext()));
  connect(ui->actionGradeWindow, SIGNAL(triggered()), Global::gw(), SLOT(handleGradeWindow()));
  connect(Global::gw()->getUI()->actionGradeWindow, SIGNAL(triggered()), Global::gw(), SLOT(handleGradeWindow()));

  /* Detect user input in data entry fields */
  connect(ui->studentId,        SIGNAL(textEdited(const QString&)), this, SLOT(handleEditStudentId(const QString&)));
  connect(ui->studentName,      SIGNAL(textEdited(const QString&)), this, SLOT(handleEditStudentName(const QString&)));
  connect(ui->questionScore,    SIGNAL(textEdited(const QString&)), this, SLOT(handleEditQuestionScore(const QString&)));
  connect(ui->questionFeedback, SIGNAL(textEdited(const QString&)), this, SLOT(handleEditQuestionFeedback(const QString&)));
  connect(ui->questionMaximum,  SIGNAL(textEdited(const QString&)), this, SLOT(handleEditQuestionMaximum(const QString&)));
  connect(ui->action0_Points,   SIGNAL(triggered()), this, SLOT(handleScore0()));
  connect(ui->action1_Point,    SIGNAL(triggered()), this, SLOT(handleScore1()));
  connect(ui->action2_Points,   SIGNAL(triggered()), this, SLOT(handleScore2()));
  connect(ui->action3_Points,   SIGNAL(triggered()), this, SLOT(handleScore3()));
  connect(ui->action4_Points,   SIGNAL(triggered()), this, SLOT(handleScore4()));
  connect(ui->action5_Points,   SIGNAL(triggered()), this, SLOT(handleScore5()));
  connect(ui->action6_Points,   SIGNAL(triggered()), this, SLOT(handleScore6()));
  connect(ui->action7_Points,   SIGNAL(triggered()), this, SLOT(handleScore7()));
  connect(ui->action8_Points,   SIGNAL(triggered()), this, SLOT(handleScore8()));
  connect(ui->actionEmpty_Points, SIGNAL(triggered()), this, SLOT(handleScoreEmpty()));

  /* Now set the first page and first question */
  adjustPage(0);
  adjustQuestion(0);
}

void MainWindow::handlePagePrev()
{
#define NO_WRAPPING
#ifdef NO_WRAPPING
  if (curPage > 0)
    curPage--;
#else
  if (curPage == 0)
    curPage = images.size()-1;
  else
    curPage--;
#endif // NO_WRAPPING
  adjustPage(curPage);
}

void MainWindow::handlePageNext()
{
#ifdef NO_WRAPPING
  if (curPage < (Global::getNumPages()-1))
    curPage++;
#else
  if (curPage == (Global::getNumPages()-1))
    curPage = 0;
  else
    curPage++;
#endif // NO_WRAPPING
  GASSERT(curPage < Global::getNumPages(), "Current page %zu is out of bounds for %zu pages", curPage, Global::getNumPages());
  adjustPage(curPage);
}

void MainWindow::handleStudentNext()
{
  if ((curPage + Global::getNumPagesPerStudent()) < Global::getNumPages())
    curPage += Global::getNumPagesPerStudent();
  adjustPage(curPage);
}

void MainWindow::handleStudentPrev()
{
  if (curPage >= Global::getNumPagesPerStudent())
    curPage -= Global::getNumPagesPerStudent();
  adjustPage(curPage);
}

void MainWindow::handleQuestionPrev()
{
  if (curQuestion > 0)
    curQuestion--;
  adjustQuestion(curQuestion);
}

void MainWindow::handleQuestionNext()
{
  if (curQuestion < (Global::getNumQuestions()-1))
    curQuestion++;
  adjustQuestion(curQuestion);
}

void MainWindow::handleEditStudentId(const QString& in)
{
  Global::db()->getStudent(curStudent()).setStudentId(in);
}

void MainWindow::handleEditStudentName(const QString& in)
{
  Global::db()->getStudent(curStudent()).setStudentName(in);
}

void MainWindow::handleEditQuestionScore(const QString& in)
{
  bool ok;
  int num;
  if (in == "")
  {
    // Convert empty strings into -1 marker to indicate no grade entered
    num = -1;
  }
  else
  {
    num = in.toInt(&ok);
    if (!ok)
    {
      GINFODIALOG("Grade entered must be an integer");
      ui->questionScore->setText("");
      return;
    }
    if ((num < 0) || (num > Global::db()->getQuestionMaximum(curQuestion)))
    {
      GINFODIALOG("Grade entered is not within acceptable range");
      ui->questionScore->setText("");
      return;
    }
  }
  Global::db()->getStudent(curStudent()).setGrade(curQuestion, num);
}

void MainWindow::handleScore(int in)
{
  if (in > Global::db()->getQuestionMaximum(curQuestion))
  {
    GDEBUG ("Rejected score %d from function keys since it is equal or higher than maximum %d", in, Global::db()->getQuestionMaximum(curQuestion));
    GINFODIALOG("Rejected score since it is higher than the maximum");
    return;
  }
  GASSERT(in >= -1, "Score value %d is not valid", in);
  GDEBUG ("Received score %d from function keys", in);
  Global::db()->getStudent(curStudent()).setGrade(curQuestion, in);
  ui->questionScore->setText(getStrFromGrade(in));
}

void MainWindow::handleEditQuestionFeedback(const QString& in)
{
  Global::db()->getStudent(curStudent()).setFeedback(curQuestion, in);
}

void MainWindow::handleEditQuestionMaximum(const QString& in)
{
  bool ok;
  int num;
  if (in == "")
  {
    // Convert empty strings into -1 marker to indicate no grade entered
    num = -1;
  }
  else
  {
    num = in.toInt(&ok);
    if (!ok)
    {
      GINFODIALOG("Maximum question value provided must be an integer");
      ui->questionMaximum->setText("");
      return;
    }
    if (num < 0)
    {
      GINFODIALOG("Maximum question value provided is negative");
      ui->questionMaximum->setText("");
      return;
    }
  }

  // Check all entered grades to see if they are less than the new maximum
  int max = -1;
  for (size_t i = 0; i < Global::db()->getNumStudents(); i++)
  {
    int v = Global::db()->getStudent(i).getGrade(curQuestion);
    if (v > max)
      max = v;
  }
  if (num < max)
  {
    GINFODIALOG("Maximum question value is less than existing grades");
    ui->questionMaximum->setText("");
    return;
  }

  Global::db()->setQuestionMaximum(curQuestion, num);
}

size_t MainWindow::curStudent()
{
  return curPage / Global::getNumPagesPerStudent();
}

void MainWindow::adjustQuestion(size_t question)
{
  GASSERT(question < Global::getNumQuestions(), "Question %zu is larger than max question %zu", question, Global::getNumQuestions());
  curQuestion = question;
  ui->questionStats->setText(QString("Question %1 of %2").arg(curQuestion+1).arg(Global::getNumQuestions()));
  ui->questionFeedback->setText(Global::db()->getStudent(curStudent()).getFeedback(curQuestion));
  ui->questionMaximum->setText(getStrFromGrade(Global::db()->getQuestionMaximum(curQuestion)));
  ui->questionScore->setText(getStrFromGrade(Global::db()->getStudent(curStudent()).getGrade(curQuestion)));
  Global::gw()->update(curStudent(), curQuestion);
}

void MainWindow::adjustPage(size_t page)
{
  GASSERT(page < Global::getNumPages(), "Page %zu is larger than max page %zu", page, Global::getNumPages());
  curPage = page;
  QPixmap pix = Global::getQPixmap(page);
  int width = zoomFactor * pix.size().width();
  int height = zoomFactor * pix.size().height();
  ui->image->setPixmap(pix.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  ui->image->resize(width, height);
  ui->pageStats->setText(QString("Page %1 of %2 (Scan %3 of %4)").arg(curPage%Global::getNumPagesPerStudent()+1).arg(Global::getNumPagesPerStudent()).arg(curPage+1).arg(Global::getNumPages()));
  ui->studentStats->setText(QString("Student %1 of %2").arg(curStudent()+1).arg(Global::getNumStudents()));
  ui->studentNum->setText(QString("Student #%1").arg(curStudent()+1));
  ui->studentId->setText(Global::db()->getStudent(curStudent()).getStudentId());
  ui->studentName->setText(Global::db()->getStudent(curStudent()).getStudentName());
  adjustQuestion(curQuestion); // Refresh the question information in the UI
}

void MainWindow::handleQuit()
{
  GEXIT(0);
}

void MainWindow::handleSave()
{
  QString savefile (QDir(Global::getPath()).canonicalPath() + "/database.qtg");
  GDEBUG("Writing out main database to [%s]", qPrintable(savefile));
  Global::save(savefile);
  QDateTime current = QDateTime::currentDateTime();
  QString bakfile (QDir(Global::getPath()).canonicalPath() + "/database-" + current.toString("yyyyMMdd-hhmmss") + ".bak");
  GDEBUG("Writing out backup database to [%s]", qPrintable(bakfile));
  Global::save(bakfile);
}

void MainWindow::adjustZoomRelative(float factor)
{
  zoomFactor *= factor;
  adjustPage(curPage);
  ui->zoomPercent->setText(QString("%1%").arg(int(zoomFactor * 100)));
}

void MainWindow::adjustZoomFixed(float factor)
{
  zoomFactor = factor;
  adjustPage(curPage);
  ui->zoomPercent->setText(QString("%1%").arg(int(zoomFactor * 100)));
}

void MainWindow::handleZoomIn()
{
  adjustZoomRelative(1.25);
}

void MainWindow::handleZoomOut()
{
  adjustZoomRelative(1 / 1.25);
}

void MainWindow::handleZoomOne()
{
  adjustZoomFixed(1.0);
}

void MainWindow::handleZoomWidth()
{
  adjustZoomFixed(ui->scrollArea->size().width() / (float)Global::getQPixmap(curPage).size().width());
}

void MainWindow::handleZoomHeight()
{
  adjustZoomFixed(ui->scrollArea->size().height() / (float)Global::getQPixmap(curPage).size().height());
}


MainWindow::~MainWindow()
{
  delete ui;
}
