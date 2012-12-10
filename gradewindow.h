#ifndef GRADEWINDOW_H
#define GRADEWINDOW_H

#include <QMainWindow>

namespace Ui {
  class GradeWindow;
}

class GradeWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit GradeWindow(QWidget *parent = 0);
  ~GradeWindow();
  Ui::GradeWindow* getUI() { return ui; }
  void update();

public slots:
  void handleGradeWindow ();

private:
  Ui::GradeWindow *ui;
};

#endif // GRADEWINDOW_H
