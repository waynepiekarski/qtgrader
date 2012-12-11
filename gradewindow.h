#ifndef GRADEWINDOW_H
#define GRADEWINDOW_H

#include <QTableWidget>
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
  void update(size_t curStudent, size_t curQuestion);

  QTableWidgetItem* getItem(int row, int col);
  QString getGrade(size_t student, size_t question);
  QString getFeedback(size_t student, size_t question);
  QString getMaximum(size_t question);
  QString getStudentId(size_t student);
  QString getStudentName(size_t student);

  void setGrade(size_t student, size_t question, int in);
  void setFeedback(size_t student, size_t question, const QString& in);
  void setMaximum(size_t question, int in);
  void setStudentId(size_t student, const QString& in);
  void setStudentName(size_t student, const QString& in);

public slots:
  void handleGradeWindow ();

private:
  Ui::GradeWindow *ui;
};

#endif // GRADEWINDOW_H
