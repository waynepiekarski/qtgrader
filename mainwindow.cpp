#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
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
  connect(ui->actionQuit,     SIGNAL(triggered()), this, SLOT(handleQuit()));
  connect(ui->actionSave ,    SIGNAL(triggered()), this, SLOT(handleSave()));
  connect(ui->actionPagePrev, SIGNAL(triggered()), this, SLOT(handlePagePrev()));
  connect(ui->actionPageNext, SIGNAL(triggered()), this, SLOT(handlePageNext()));
  connect(ui->actionStudentPrev, SIGNAL(triggered()), this, SLOT(handleStudentPrev()));
  connect(ui->actionStudentNext, SIGNAL(triggered()), this, SLOT(handleStudentNext()));

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
  }
  Q_ASSERT(images.size() > 0);
  ui->image->setScaledContents(true);
  ui->image->setPixmap(QPixmap::fromImage(images[0]));

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
  ui->image->setPixmap(QPixmap::fromImage(images[page]));
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

void MainWindow::adjustScrollBars(QScrollBar *scroll, float factor)
{
  scroll->setValue(int(factor * scroll->value() + ((factor - 1) * scroll->pageStep()/2)));
}

void MainWindow::adjustZoomRelative(float factor)
{
  float scrollFactor = factor;
  zoomFactor *= factor;
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
  adjustScrollBars(ui->scrollArea->horizontalScrollBar(), scrollFactor);
  adjustScrollBars(ui->scrollArea->verticalScrollBar(), scrollFactor);
  ui->zoomPercent->setText(QString("%1%").arg(int(zoomFactor * 100)));
}

void MainWindow::adjustZoomFixed(float factor)
{
  zoomFactor = factor;
  ui->image->resize(factor * ui->image->pixmap()->size());
  ui->scrollArea->horizontalScrollBar()->setValue(0);
  ui->scrollArea->verticalScrollBar()->setValue(0);
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
  adjustZoomFixed(ui->scrollArea->size().width() / (float)ui->image->pixmap()->size().width());
}

void MainWindow::handleZoomHeight()
{
  adjustZoomFixed(ui->scrollArea->size().height() / (float)ui->image->pixmap()->size().height());
}


MainWindow::~MainWindow()
{
  delete ui;
}
