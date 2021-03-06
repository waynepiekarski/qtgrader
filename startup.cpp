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

#include <QFileDialog>

#include "startup.h"
#include "ui_startup.h"
#include "global.h"
#include "database.h"
#include "pages.h"
#include "debug.h"

// #define DEBUG_EXISTING_PROJECT QString(QDir::homePath() + "/Desktop/qtgrader-scans" + "/CHEM12A-21550-Fall-2012-Exam/database.qtg")
// #define DEBUG_EXISTING_IMAGES  QString(QDir::homePath() + "/Desktop/qtgrader-scans" + "/CHEM12A-21550-Fall-2012-Exam")

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
  /* Set up the Qt user interface */
  ui->setupUi(this);

  /* Hard code defaults for new project debugging */
  ui->imageDirectory->setText(QDir::homePath() + "/Desktop/qtgrader-scans");
  ui->existingProject->setText("");
  ui->questionsPerStudent->setText("30");
  ui->pagesPerStudent->setText("12");

  /* Implement debugging if desired */
#if defined(DEBUG_EXISTING_PROJECT)
#warning Hard coded existing project
#define SKIP_STARTUP_DIALOG
  ui->existingProject->setText(DEBUG_EXISTING_PROJECT);
  ui->imageDirectory->setText("");
#elif defined(DEBUG_EXISTING_IMAGES)
#warning Hard coded existing scans
#define SKIP_STARTUP_DIALOG
  ui->existingProject->setText("");
  ui->imageDirectory->setText(DEBUG_EXISTING_IMAGES);
#endif

  /* Set up page change buttons */
  connect(ui->selectImageDirectory, SIGNAL(clicked()), this, SLOT(handleSelectImageDirectory()));
  connect(ui->loadExistingProject, SIGNAL(clicked()), this, SLOT(handleLoadExistingProject()));

  /* Execute the dialog to get the result */
#ifndef SKIP_STARTUP_DIALOG
  switch(exec())
    {
    case QDialog::Accepted:
      GDEBUG ("Accepted startup dialog result");
      break;
    case QDialog::Rejected:
      GDEBUG ("Abort selected during startup dialog, so exiting");
      GEXIT(0);
      break;
    default:
      GFATAL ("Unknown exec() return value");
      break;
    }
#endif // SKIP_STARTUP_DIALOG

  /* Decide what to do with the result */
  if (ui->imageDirectory->text().length() == 0)
  {
    /* Handle the case where we load a project file */
    if (ui->existingProject->text().length() == 0)
      GEXITDIALOG("Must specify either a project file or an image directory");
    GDEBUG("Existing project selected [%s]", qPrintable(ui->existingProject->text()));
    Global::initDatabase(ui->existingProject->text());
  }
  else
  {
    if (ui->existingProject->text().length() != 0)
      GEXITDIALOG("Cannot specify project file and image directory at the same time");

    /* Image directory specified, so make sure the number of questions is set */
    bool ok;
    int _questionsPerStudent = ui->questionsPerStudent->text().toInt(&ok);
    if ((!ok) || (_questionsPerStudent <= 0))
      GEXITDIALOG("Must specify a valid integer for the questions per student");

    /* Also check the number of pages is valid */
    int _pagesPerStudent = ui->pagesPerStudent->text().toInt(&ok);
    if ((!ok) || (_pagesPerStudent <= 0))
      GEXITDIALOG("Must specify a valid integer for the pages per student");

    GDEBUG("%d questions and %d pages per student with image directory [%s]",
           _questionsPerStudent, _pagesPerStudent, qPrintable(ui->imageDirectory->text()));

    /* Set up the image cache */
    Global::initPages(ui->imageDirectory->text());

    /* Set up the database */
    int numStudents = Global::getNumPages() / _pagesPerStudent;
    if (Global::getNumPages() % _pagesPerStudent != 0)
    {
      GDEBUG ("Number of pages %zu and pages per student %d has remainder %ld",
              Global::getNumPages(), _pagesPerStudent, Global::getNumPages() % _pagesPerStudent);
      GEXITDIALOG("Number of scan pages does not match number of pages per student, spare pages found");
    }
    Global::initDatabase(numStudents, _questionsPerStudent, _pagesPerStudent);
  }
}

StartupDialog::~StartupDialog()
{
  delete ui;
}

void StartupDialog::handleSelectImageDirectory()
{
  QString startDir = QDir::homePath();
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
                                                  tr("Project File (*.qtg)"));
  if (fileName != NULL)
  {
    ui->existingProject->setText(fileName);
    ui->imageDirectory->setText("");
  }
}
