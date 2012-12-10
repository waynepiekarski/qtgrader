#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_gradewindow.h"
#include "database.h"
#include "global.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gw(new GradeWindow),
    curPage(0),
    curQuestion(0),
    zoomFactor(1.0)
{
  /* Set up the Qt user interface */
  ui->setupUi(this);

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
  connect(gw->getUI()->actionSave , SIGNAL(triggered()), this, SLOT(handleSave()));
  connect(ui->actionPagePrev, SIGNAL(triggered()), this, SLOT(handlePagePrev()));
  connect(ui->actionPageNext, SIGNAL(triggered()), this, SLOT(handlePageNext()));
  connect(ui->actionStudentPrev, SIGNAL(triggered()), this, SLOT(handleStudentPrev()));
  connect(ui->actionStudentNext, SIGNAL(triggered()), this, SLOT(handleStudentNext()));
  connect(ui->actionGradeWindow, SIGNAL(triggered()), gw, SLOT(handleGradeWindow()));
  connect(gw->getUI()->actionGradeWindow, SIGNAL(triggered()), gw, SLOT(handleGradeWindow()));

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
  Q_ASSERT(curPage < Global::getNumPages());
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

void MainWindow::adjustQuestion(size_t question)
{
  GASSERT(question < Global::getNumQuestions(), "Question %zu is larger than max question %zu", question, Global::getNumQuestions());
  curQuestion = question;
  ui->questionStats->setText(QString("Question %1 of %2").arg(curQuestion+1).arg(Global::getNumQuestions()));
  size_t curStudent = curPage/Global::getNumPagesPerStudent();
  ui->questionFeedback->setText(Global::db()->getStudent(curStudent).getFeedback(curQuestion));
  ui->questionMaximum->setText(QString("%1").arg(Global::db()->getQuestionMaximum(curQuestion)));
  ui->questionScore->setText(QString("%1").arg(Global::db()->getStudent(curStudent).getGrade(curQuestion)));
}

void MainWindow::adjustPage(size_t page)
{
  GASSERT(page < Global::getNumPages(), "Page %zu is larger than max page %zu", page, Global::getNumPages());
  curPage = page;
  QPixmap &pix = Global::getQPixmap(page);
  int width = zoomFactor * pix.size().width();
  int height = zoomFactor * pix.size().height();
  ui->image->setPixmap(pix.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  ui->image->resize(width, height);
  ui->pageStats->setText(QString("Page %1 of %2 (Scan %3 of %4)").arg(curPage%Global::getNumPagesPerStudent()+1).arg(Global::getNumPagesPerStudent()).arg(curPage+1).arg(Global::getNumPages()));
  size_t curStudent = curPage/Global::getNumPagesPerStudent();
  ui->studentStats->setText(QString("Student %1 of %2").arg(curStudent+1).arg(Global::getNumStudents()));
  ui->studentId->setText(Global::db()->getStudent(curStudent).getStudentId());
  ui->studentName->setText(Global::db()->getStudent(curStudent).getStudentName());
  adjustQuestion(curQuestion); // Refresh the question information in the UI
}

void MainWindow::handleQuit()
{
  GEXIT(0);
}

void MainWindow::handleSave()
{
  fprintf (stderr, "Save ignored\n");
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
