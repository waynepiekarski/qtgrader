#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_gradewindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gw(new GradeWindow),
    curPage(0),
    zoomFactor(1.0)
{
  /* Set up the Qt user interface */
  ui->setupUi(this);

  /* Set up page change buttons */
  connect(ui->pageLeft ,   SIGNAL(clicked()), this, SLOT(handlePagePrev()));
  connect(ui->pageRight,   SIGNAL(clicked()), this, SLOT(handlePageNext()));
  connect(ui->studentPrev, SIGNAL(clicked()), this, SLOT(handleStudentPrev()));
  connect(ui->studentNext, SIGNAL(clicked()), this, SLOT(handleStudentNext()));
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

  /* Load in all the images */
  fprintf (stderr, "Hello world\n");
  QDir dir ("../../../../scans", "*.jpg");
  dir.setFilter(QDir::Files);
  fprintf (stderr, "Scanning for images in dir [%s]\n",qPrintable(dir.canonicalPath()));
  QFileInfoList jpegs = dir.entryInfoList();
  for (int i = 0; i < jpegs.size(); i++)
  {
    QFileInfo f = jpegs.at(i);
    fprintf (stderr, "Loading image %d from file [%s]\n", i, qPrintable(f.fileName()));
    QImage img (f.filePath());
    images.push_back(img);
    pixes.push_back(QPixmap::fromImage(img));
  }
  Q_ASSERT(images.size() > 0);

  adjustPage(curPage);
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
  if (curPage < (images.size()-1))
    curPage++;
#else
  if (curPage == (images.size()-1))
    curPage = 0;
  else
    curPage++;
#endif // NO_WRAPPING
  Q_ASSERT(curPage < images.size());
  adjustPage(curPage);
}

void MainWindow::handleStudentNext()
{
  if ((curPage + numPagesPerTest) < images.size())
    curPage += numPagesPerTest;
  adjustPage(curPage);
}

void MainWindow::handleStudentPrev()
{
  if (curPage >= numPagesPerTest)
    curPage -= numPagesPerTest;
  adjustPage(curPage);
}

void MainWindow::adjustPage(size_t page)
{
  Q_ASSERT(page < images.size());
  curPage = page;
  QPixmap &pix = pixes[page];
  int width = zoomFactor * pix.size().width();
  int height = zoomFactor * pix.size().height();
  ui->image->setPixmap(pix.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  ui->image->resize(width, height);
  ui->pageStats->setText(QString("Page %1 of %2 (Scan %3 of %4)").arg(curPage%numPagesPerTest+1).arg(numPagesPerTest).arg(curPage+1).arg(images.size()));
  ui->studentStats->setText(QString("Student %1 of %2").arg(curPage/numPagesPerTest+1).arg(images.size()/numPagesPerTest));
}

void MainWindow::handleQuit()
{
  exit(0);
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
  adjustZoomFixed(ui->scrollArea->size().width() / (float)pixes[curPage].size().width());
}

void MainWindow::handleZoomHeight()
{
  adjustZoomFixed(ui->scrollArea->size().height() / (float)pixes[curPage].size().height());
}


MainWindow::~MainWindow()
{
  delete ui;
}
