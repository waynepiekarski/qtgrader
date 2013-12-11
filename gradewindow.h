// ---------------------------------------------------------------------
//
// qtgrader  -  the interactive high speed grading assistant
// Copyright (C) 2012-2013 Wayne Piekarski
// wayne@tinmith.net http://tinmith.net/wayne
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ---------------------------------------------------------------------

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
  void closeEvent(QCloseEvent *event);

  QTableWidgetItem* getItem(int row, int col);
  QString getTotalGrade(size_t student);
  QString getGradeFeedback(size_t student, size_t question);
  QString getMaximum(size_t question);
  QString getQuestionPage(size_t question);
  QString getStudentId(size_t student);
  QString getStudentName(size_t student);
  QString getTotalMaximum();

  void setTotalGrade(size_t student, int total);
  void setGradeFeedback(size_t student, size_t question, int grade, const QString& feedback);
  void setTotalMaximum(int in);
  void setMaximum(size_t question, int in);
  void setQuestionPage(size_t question, int in);
  void setStudentId(size_t student, const QString& in);
  void setStudentName(size_t student, const QString& in);

public slots:
  void handleGradeWindow (bool init = false);
  void handleCellChanged (int row, int col, int prevRow, int prevCol);
  void handleResizeContents();
  void handleResizeSmall();
  void handleResizeLarge();

private:
  Ui::GradeWindow *ui;
};

#endif // GRADEWINDOW_H
