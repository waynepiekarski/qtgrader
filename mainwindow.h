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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <vector>

#include "gradewindow.h"
#include "pages.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  Ui::MainWindow* getUI() { return ui; }
    
public slots:
  void handlePagePrev();
  void handlePageNext();
  void handleStudentPrev();
  void handleStudentNext();
  void handleQuestionPrev();
  void handleQuestionNext();
  void handleZoomIn();
  void handleZoomOut();
  void handleZoomWidth();
  void handleZoomHeight();
  void handleZoomOne();
  void handleSave();
  void handleGeneratePDFs();
  void handleQuit();
  void handleEditStudentId(const QString&);
  void handleEditStudentName(const QString&);
  void handleEditQuestionScore(const QString&);
  void handleEditQuestionFeedback(const QString&);
  void handleEditQuestionMaximum(const QString&);
  void handleEditQuestionPage(const QString&);
  void handleScore0() { handleScore(0); }
  void handleScore1() { handleScore(1); }
  void handleScore2() { handleScore(2); }
  void handleScore3() { handleScore(3); }
  void handleScore4() { handleScore(4); }
  void handleScore5() { handleScore(5); }
  void handleScore6() { handleScore(6); }
  void handleScore7() { handleScore(7); }
  void handleScore8() { handleScore(8); }
  void handleScoreEmpty() { handleScore(-1); }

private:
  void handleScore(int);
  void adjustAnswer(size_t question);
  void adjustZoomRelative(float factor);
  void adjustZoomFixed(float factor);
  void adjustQuestion(size_t question);
  void adjustPage(size_t page);
  size_t curStudent();

  Ui::MainWindow *ui;
  size_t curPage;
  size_t curQuestion;
  float zoomFactor;
};

#endif // MAINWINDOW_H
