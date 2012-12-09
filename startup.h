#pragma once

#include <QDialog>
#include "ui_startup.h"
#include "debug.h"

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
  Q_OBJECT
    
public:
  explicit StartupDialog(QWidget *parent = 0);
  ~StartupDialog();

  int getQuestionsPerStudent() { return _questionsPerStudent; }
  QString getExistingProject()
  {
    GASSERT(ui->imageDirectory->text().length() != 0, "Dialog mismatch");
    GASSERT(_questionsPerStudent != 0, "Dialog mismatch");
    return ui->existingProject->text();
  }
  QString getImageDirectory()
  {
    GASSERT(ui->existingProject->text().length() != 0, "Dialog mismatch");
    GASSERT(_questionsPerStudent == 0, "Dialog mismatch");
    return ui->imageDirectory->text();
  }
  
public slots:
  void handleSelectImageDirectory();
  void handleLoadExistingProject();

private:
  Ui::StartupDialog *ui;
  int _questionsPerStudent;
};
