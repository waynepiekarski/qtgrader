#include <QFileDialog>
#include <QMessageBox>

#include "startup.h"
#include "ui_startup.h"
#include "debug.h"

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
  /* Set up the Qt user interface */
  ui->setupUi(this);

  /* Set up page change buttons */
  connect(ui->selectImageDirectory, SIGNAL(clicked()), this, SLOT(handleSelectImageDirectory()));
  connect(ui->loadExistingProject, SIGNAL(clicked()), this, SLOT(handleLoadExistingProject()));

  /* Execute the dialog to get the result */
  switch(exec())
    {
    case QDialog::Accepted:
      GDEBUG ("Accepted startup dialog result");
      break;
    case QDialog::Rejected:
      GDEBUG ("Abort selected during startup dialog, so exiting");
      exit(0);
      break;
    default:
      GFATAL ("Unknown exec() return value");
      break;
    }

  /* Decide what to do with the result */
  if (ui->imageDirectory->text().length() == 0)
  {
    /* Handle the case where we load a project file */
    if (ui->existingProject->text().length() == 0)
    {
      QMessageBox error;
      error.setText("Must specify either a project file or an image directory");
      error.exec();
      exit(0);
    }
    GDEBUG("Existing project selected [%s]", qPrintable(ui->existingProject->text()));
  }
  else
  {
    if (ui->existingProject->text().length() != 0)
    {
      QMessageBox error;
      error.setText("Cannot specify project file and image directory at the same time");
      error.exec();
      exit(0);
    }

    /* Image directory specified, so make sure the number of questions is set */
    bool ok;
    _questionsPerStudent = ui->questionsPerStudent->text().toInt(&ok);
    if ((!ok) || (_questionsPerStudent <= 0))
    {
      QMessageBox error;
      error.setText("Must specify a valid integer for the questions per student");
      error.exec();
      exit(0);
    }
    GDEBUG("%d questions per student with image directory [%s]", _questionsPerStudent, qPrintable(ui->imageDirectory->text()));
  }
}

StartupDialog::~StartupDialog()
{
  delete ui;
}

void StartupDialog::handleSelectImageDirectory()
{
  GDEBUG("Dir = [%s]", qPrintable(QDir::home().canonicalPath()));
  QString startDir = QDir::home().canonicalPath();
  if (ui->imageDirectory->text().length() != 0)
    startDir = ui->imageDirectory->text();
  QString dirName = QFileDialog::getExistingDirectory(this, tr("Image Scan Directory"), startDir);
  if (dirName != NULL)
  {
      ui->existingProject->setText("");
    ui->imageDirectory->setText(dirName);
  }
}

void StartupDialog::handleLoadExistingProject()
{
  QString startFile = QDir::home().canonicalPath();
  if (ui->existingProject->text().length() != 0)
    startFile = ui->existingProject->text();

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Project"),
                                                  startFile,
                                                  tr("Project File (*.prj)"));
  if (fileName != NULL)
  {
    ui->existingProject->setText(fileName);
    ui->imageDirectory->setText("");
  }
}
