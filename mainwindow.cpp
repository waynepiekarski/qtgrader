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
  connect(ui->leftPage,   SIGNAL(clicked()), this, SLOT(handleLeftPage()));
  connect(ui->rightPage,  SIGNAL(clicked()), this, SLOT(handleRightPage()));
  connect(ui->zoomIn,     SIGNAL(clicked()), this, SLOT(handleZoomIn()));
  connect(ui->zoomOut,    SIGNAL(clicked()), this, SLOT(handleZoomOut()));
  connect(ui->zoomWidth,  SIGNAL(clicked()), this, SLOT(handleZoomWidth()));
  connect(ui->zoomHeight, SIGNAL(clicked()), this, SLOT(handleZoomHeight()));
  connect(ui->zoomOne,    SIGNAL(clicked()), this, SLOT(handleZoomOne()));

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
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
  ui->image->adjustSize();
}

void MainWindow::handleLeftPage()
{
  if (curPage > 0)
    curPage--;
  fprintf (stderr, "Left page selected %zu\n", curPage);
  ui->image->setPixmap(QPixmap::fromImage(images[curPage]));
  ui->image->adjustSize();
}

void MainWindow::handleRightPage()
{
  if (curPage < (images.size()-1))
    curPage++;
  Q_ASSERT(curPage < images.size());
  fprintf (stderr, "Right page selected %zu\n", curPage);
  ui->image->setPixmap(QPixmap::fromImage(images[curPage]));
  ui->image->adjustSize();
}

void MainWindow::handleZoomIn()
{
  zoomFactor *= 1.25;
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
}

void MainWindow::handleZoomOut()
{
  zoomFactor /= 1.25;
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
}

void MainWindow::handleZoomOne()
{
  zoomFactor = 1.0;
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
}

void MainWindow::handleZoomWidth()
{
  zoomFactor = ui->scrollArea->size().width() / (float)ui->image->pixmap()->size().width();
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
}

void MainWindow::handleZoomHeight()
{
  zoomFactor = ui->scrollArea->size().height() / (float)ui->image->pixmap()->size().height();
  ui->image->resize(zoomFactor * ui->image->pixmap()->size());
}


MainWindow::~MainWindow()
{
    delete ui;
}
