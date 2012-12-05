#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
    
public slots:
  void handlePagePrev();
  void handlePageNext();
  void handleStudentPrev();
  void handleStudentNext();
  void handleZoomIn();
  void handleZoomOut();
  void handleZoomWidth();
  void handleZoomHeight();
  void handleZoomOne();
  void handleSave();
  void handleQuit();

private:
  void adjustScrollBars(QScrollBar *scroll, float factor);
  void adjustZoom(float factor);
  void adjustPage(size_t page);

  Ui::MainWindow *ui;
  std::vector<QImage> images;
  std::vector<QString> studentNames;
  size_t curPage;
  float zoomFactor;

  static const size_t numPagesPerTest = 4; // Fixed constant, need to read this from a file
};

#endif // MAINWINDOW_H
