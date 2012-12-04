#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
  void handleLeftPage();
  void handleRightPage();
  void handleZoomIn();
  void handleZoomOut();
  void handleZoomWidth();
  void handleZoomHeight();
  void handleZoomOne();

private:
  Ui::MainWindow *ui;
  std::vector<QImage> images;
  size_t curPage;
  float zoomFactor;
};

#endif // MAINWINDOW_H
