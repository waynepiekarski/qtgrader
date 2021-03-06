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

#include "gradewindow.h"
#include "database.h"
#include "pages.h"
#include "global.h"
#include "debug.h"

#include <QApplication>
#include <QTextStream>
#include <QDateTime>
#include <QPrinter>
#include <QPainter>
#include <QFile>
#include <QLabel>
#include <math.h>

GradeWindow* Global::_gw = NULL;
MainWindow* Global::_mw = NULL;
QApplication* Global::_qapp = NULL;
QLabel* Global::_statusLabel = NULL;
Database* Global::_db = NULL;
Pages* Global::_pages = NULL;
size_t Global::_numPagesPerStudent = 0;

void Global::generatePDFs(QString dirname)
{
  size_t pageCount = 0;

  for (size_t s = 0; s < getNumStudents(); s++)
  {
    Student& student = db()->getStudent(s);
    // Use the student name to form the file name for the repot
    QString clean = student.getStudentName();
    // Convert all non alpha/num chars into an underscore
    for (QString::iterator i = clean.begin(); i != clean.end(); i++)
    {
      if (!i->isLetterOrNumber())
        *i = '_';
    }
    if (clean.length() == 0)
    {
      GINFODIALOG(QString("Cannot render PDF because student %1 does not have a name assigned").arg(s+1));
      return;
    }
    QString pdfname (dirname + "/report-" + clean + ".pdf");
    GDEBUG ("Generating PDF [%s] for student [%s]", qPrintable(pdfname), qPrintable(student.getStudentId()));

    QPrinter printer (QPrinter::HighResolution);
    printer.setOutputFormat (QPrinter::PdfFormat);
    printer.setOutputFileName (pdfname);
    printer.setPageSize(QPrinter::Letter);
    printer.setResolution(150); // DPI for the printing
    printer.setColorMode(QPrinter::GrayScale);

    QPainter painter;
    if (!painter.begin(&printer)) // Check for errors here
      GFATAL("Failed to do QPainter begin()");

    // Can use this code to change the text color, but causes larger PDF files
    // since it must use a color output format instead.
    //QPen penColor(QColor("#000090")); // Change text to dark blue
    //painter.setPen(penColor);

    for (size_t p = 0; p < getNumPagesPerStudent(); p++)
    {
      pageCount++;
      // Add spaces at the end so the widget can resize into the reserved space without a re-layout
      Global::getStatusLabel()->setText(QString("Generating PDF for student %1 of %2, page %3 of %4 (%5 percent)     ").
                                        arg(s+1).arg(getNumStudents()).arg(p+1).arg(getNumPagesPerStudent()).arg(rint(0.5+100.0*pageCount/(1.0*getNumPages()))));
      // Flush out Qt events so that the UI update occurs inside this handler
      Global::getQApplication()->processEvents();

      GDEBUG ("Printing page %zu of %zu for report [%s]", p+1, getNumPagesPerStudent(), qPrintable(pdfname));
      QPixmap pix = getPages()->getQPixmap(p+s*getNumPagesPerStudent());
      // Scale the pixmap to fit the printer
      pix = pix.scaled(printer.pageRect().width(), printer.pageRect().height(), Qt::KeepAspectRatio);
      // Draw the pixmap to the printer
      painter.drawPixmap (0, 0, pix);

      // Print out the student details at the top of the page
      QString title = QString("Name: %1  ID: %2  Page: %3 of %4  Final Grade: %5 of %6").arg(student.getStudentName()).arg(student.getStudentId()).arg(p+1).arg(getNumPagesPerStudent()).arg(student.getTotal()).arg(db()->getTotalMaximum());
      painter.drawText(0, 0, title);

      // Build up a results string to print onto the page
      QString grades ("Results:");
      size_t pageTotal = 0;
      size_t pageMax = 0;
      for (size_t q = 0; q < getNumQuestions(); q++)
      {
        // See if the question is on this page
        GASSERT(Global::db()->getQuestionPage(q) != 0, "Cannot have page 0 assigned for question %zu", q);
        if (Global::db()->getQuestionPage(q) < 0)
        {
          GINFODIALOG(QString("Cannot render PDF because question %1 does not have a page assigned").arg(q+1));
          return;
        }
        if (Global::db()->getQuestionPage(q) == ((int)p+1))
        {
          if (student.getGrade(q) < 0)
          {
            GINFODIALOG(QString("Cannot render PDF for student [%1] because question %2 has no grade assigned").arg(student.getStudentName()).arg(q+1));
            return;
          }
          pageTotal += student.getGrade(q);
          pageMax += Global::db()->getQuestionMaximum(q);
          grades += QString("  Q%1 = %2/%3").arg(q+1).arg(student.getGrade(q)).arg(Global::db()->getQuestionMaximum(q));
          if (student.getFeedback(q) != "")
            grades += QString(" [%1]").arg(student.getFeedback(q));
        }
      }
      grades += QString("  Totals = %1/%2").arg(pageTotal).arg(pageMax);
      if (pageMax == 0)
          grades = QString("No Results For This Page");
      // Wrap the text to fit a bounding box that is the width of the page, align to the bottom of the page
      painter.drawText(0, 30, printer.pageRect().width(), printer.pageRect().height()-30, Qt::TextWordWrap | Qt::AlignBottom, grades);

      // Insert a new page except on the last one
      if (p < getNumPagesPerStudent()-1)
        if (!printer.newPage()) // Check for errors here
          GFATAL("Failed to do newPage() call");
    }

    painter.end();
  }
  Global::getStatusLabel()->setText("");
}

void Global::save(QString filename)
{
  GASSERT(_db != NULL, "Cannot save empty database");

  QFile fd (filename);
  if (!fd.open(QIODevice::WriteOnly))
    GEXITDIALOG("Could not open project file for writing " + filename);
  QTextStream file (&fd);

  file << "VERSION=" << DB_VERSION << "\n";
  file << "IMAGE_PATH=.\n"; // Images are always in the same directory as the project file
  file << "QUESTIONS_PER_STUDENT=" << Global::db()->getNumQuestions() << "\n";
  GASSERT(_numPagesPerStudent > 0, "_numPagesPerStudent %zu is not valid", _numPagesPerStudent);
  file << "PAGES_PER_STUDENT=" << _numPagesPerStudent << "\n";
  file << "TOTAL_STUDENTS=" << Global::db()->getNumStudents() << "\n";
  file << "SAVE_DATE_TIME=" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
  file << "NUM_IMAGES=" << Global::getPages()->size() << "\n";
  if (Global::getPages()->size() != Global::db()->getNumStudents() * _numPagesPerStudent)
  {
    GDEBUG("Detected mismatch in number of pages %zu, number of students %zu, and number of pages per student %zu",
           Global::getPages()->size(), Global::db()->getNumStudents(), _numPagesPerStudent);
    GINFODIALOG("Detected page mismatch during save, but will continue anyway");
  }

  file << "-2\tPAGE\tPAGE\t" << -1;
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << Global::db()->getQuestionPage(q);
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << "NF"; // Emit empty feedback columns
  file << "\n";

  file << "-1\tMAX\tMAX\t" << Global::db()->getTotalMaximum();
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << Global::db()->getQuestionMaximum(q);
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << "NF"; // Emit empty feedback columns
  file << "\n";

  for (size_t s = 0; s < Global::db()->getNumStudents(); s++)
  {
    Student& student = Global::db()->getStudent(s);
    file << s << "\t" << student.getStudentId() << "\t" << student.getStudentName();
    file << "\t" << student.getTotal();
    for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
      file << "\t" << student.getGrade(q);
    for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
      file << "\t" << student.getFeedback(q);
    file << "\n";
  }

  // Verify that the write actually worked ok
  fd.flush();
  if (fd.error() != QFile::NoError)
    GINFODIALOG ("Failed to write out complete project file - will not exit, but you need to re-save your work!");
  fd.close();

  // TODO: We could also dump out the Qt table as well as a backup in a different file
}

void Global::initDatabase(QString filename)
{
  GASSERT(_db == NULL, "Cannot initialize database twice");

  QFile fd (filename);
  if (!fd.open(QIODevice::ReadOnly))
    GEXITDIALOG("Could not open project file for reading " + filename);
  QTextStream file (&fd);

  // The first five lines contain the following information
  // VERSION=%s
  // IMAGE_PATH=%s
  // QUESTIONS_PER_STUDENT=%d
  // PAGES_PER_STUDENT=%d
  // TOTAL_STUDENTS=%d
  // SAVE_DATE_TIME=%s
  // NUM_IMAGES=%d
  struct _keyvalue { QString key; QString string; size_t number; };
  _keyvalue intro[7];
  QString header_names[7] = { "VERSION", "IMAGE_PATH", "QUESTIONS_PER_STUDENT", "PAGES_PER_STUDENT", "TOTAL_STUDENTS", "SAVE_DATE_TIME", "NUM_IMAGES" };
  bool expect_numbers[7] = { false, false, true, true, true, false, true };
  for (size_t i = 0; i < 7; i++)
  {
    if (file.atEnd())
      GEXITDIALOG(QString("Encountered end of file reading line %1").arg(i));
    QString in = file.readLine();
    QStringList fields = in.split("=");
    if (fields.size() != 2)
      GEXITDIALOG(QString("Could not find key/value pair separated by = in [%1] reading line %2").arg(in).arg(i));
    intro[i].key = fields.at(0);
    if (header_names[i] != fields.at(0))
      GEXITDIALOG(QString("Line %1 header was not [%2] but found instead [%3]").arg(i).arg(header_names[i]).arg(intro[i].key));
    intro[i].string = fields.at(1);
    if (fields.at(0).size() == 0)
      GEXITDIALOG(QString("Key in [%1] was empty reading line %2").arg(in).arg(i));
    if (fields.at(1).size() == 0)
      GEXITDIALOG(QString("Value in [%1] was empty reading line %2").arg(in).arg(i));
    intro[i].number = -1;
    if (expect_numbers[i])
    {
      bool ok;
      int num = fields.at(1).toInt(&ok);
      if (!ok)
        GEXITDIALOG(QString("Value [%1] in [%2] was not an integer").arg(fields.at(1)).arg(in));
      if (num <= 0)
        GEXITDIALOG(QString("Value [%1] in [%2] was not a positive integer").arg(fields.at(1)).arg(in));
      intro[i].number = num;
    }
    if (expect_numbers[i])
      GDEBUG ("Parsed key [%s] = number [%zu]", qPrintable(intro[i].key), intro[i].number);
    else
      GDEBUG ("Parsed key [%s] = string [%s]", qPrintable(intro[i].key), qPrintable(intro[i].string));
  }

  QString version = intro[0].string;
  QString image_path = intro[1].string;
  size_t numQuestions = intro[2].number;
  size_t numPagesPerStudent = intro[3].number;
  size_t numStudents = intro[4].number;
  QString unused_date_time = intro[5].string;
  size_t numImages = intro[6].number;

  // Initialize the database since we know all the dimensions now
  initDatabase(numStudents, numQuestions, numPagesPerStudent);

  // Check the version is what we are expecting
  if (version != DB_VERSION)
    GEXITDIALOG(QString("Found version [%1] when only version [%2] is supported").arg(version).arg(DB_VERSION));

  // Image paths should always be . right now, nothing else is supported
  if (image_path != ".")
    GEXITDIALOG("Non-empty path found for image loading");
  int slash = filename.lastIndexOf("/");
  if (slash < 0)
    GEXITDIALOG("Could not find ending slash in file name");
  QString full_image_path = filename.left(slash);
  GDEBUG("Using image path [%s] from input file [%s]", qPrintable(full_image_path), qPrintable(filename));

  initPages(full_image_path);

  // Check that the number of images loaded matches what we are expecting
  GDEBUG ("Loaded in %zu images from [%s], input file says %zu images", getPages()->size(), qPrintable(image_path), numImages);
  if (getPages()->size() != numImages)
    GEXITDIALOG(QString("Found different number of images than was used for the save"));

  // Now populate the database. Each entry is of the following form:
  // IDNUM,STUDENTID,STUDENTNAME,TOTAL,Q0,Q1,..,QN,Feed0,Feed1,..,FeedN
  // All fields will contain something except for Feed0..FeedN
  for (int student = -2; student < (int)numStudents; student++)
  {
    if (file.atEnd())
      GEXITDIALOG(QString("Encountered end of file reading student %1 of %2").arg(student+1).arg(numStudents));
    QString in = file.readLine();
    QStringList fields = in.split("\t");
    if (fields.size() != (2*(int)numQuestions + 4))
      GEXITDIALOG(QString("Found only %1 columns in [%2] for student %3 of %4 when expected 2x%5+4 columns").arg(fields.size()).arg(in).arg(student+1).arg(numStudents).arg(numQuestions));
    for (size_t col = 4; col < numQuestions+4; col++) // Do not search student id/name, or any feedback, since these can be empty
      if (fields.at(col) == "")
        GEXITDIALOG(QString("Found empty column %1 for student %3 of %4").arg(col+1).arg(student+1).arg(numStudents));
    bool ok;
    {
      int num = fields.at(0).toInt(&ok);
      if ((!ok) || (num != student))
        GEXITDIALOG(QString("Expected row number %1 but found [%2] instead for student %3 of %4").arg(student).arg(fields.at(0)).arg(student+1).arg(numStudents));
    }

    if (student == -2)
    {
      if ((fields.at(1) != "PAGE") && (fields.at(2) != "PAGE"))
        GEXITDIALOG(QString("Expected PAGE headers for first row of data, but found [%1] [%2] instead").arg(fields.at(1).arg(fields.at(2))));
      for (size_t q = 0; q < numQuestions; q++)
      {
        int page = fields.at(q+4).toInt(&ok);
        if ((!ok) || (page < -1))
          GEXITDIALOG(QString("Page value [%1] for question %2 was not valid for maximum values").arg(fields.at(q+4)).arg(q+1));
        Global::db()->setQuestionPage(q, page, false);
      }
      int total = fields.at(3).toInt(&ok);
      if ((!ok) || (total != -1))
        GEXITDIALOG(QString("Page total in file [%1] was not -1").arg(fields.at(3)));
    }
    else if (student == -1)
    {
      if ((fields.at(1) != "MAX") && (fields.at(2) != "MAX"))
        GEXITDIALOG(QString("Expected MAX headers for first row of data, but found [%1] [%2] instead").arg(fields.at(1).arg(fields.at(2))));
      for (size_t q = 0; q < numQuestions; q++)
      {
        int score = fields.at(q+4).toInt(&ok);
        if ((!ok) || (score < -1))
          GEXITDIALOG(QString("Score value [%1] for question %2 was not valid for maximum values").arg(fields.at(q+4)).arg(q+1));
        Global::db()->setQuestionMaximum(q, score, false);
      }
      int total = fields.at(3).toInt(&ok);
      if ((!ok) || (total != Global::db()->getTotalMaximum()))
        GEXITDIALOG(QString("Maximum total in file [%1] did not match calculated maximum total %2").arg(fields.at(3)).arg(Global::db()->getTotalMaximum()));
    }
    else
    {
      Student &entry = Global::db()->getStudent(student);
      entry.setStudentId(fields.at(1), false);
      entry.setStudentName(fields.at(2), false);
      for (size_t q = 0; q < numQuestions; q++)
      {
        int score = fields.at(q+4).toInt(&ok);
        if ((!ok) || (score < -1) || (score > Global::db()->getQuestionMaximum(q)))
          GEXITDIALOG(QString("Score value [%1] for question %2 was not valid student %3 of %4").arg(fields.at(q+4)).arg(q+1).arg(student+1).arg(numStudents));
        entry.setGrade(q, score, false);
      }
      for (size_t q = 0; q < numQuestions; q++)
        entry.setFeedback(q, fields.at(q+4+numQuestions), false);
      int total = fields.at(3).toInt(&ok);
      if ((!ok) || (total != entry.getTotal()))
        GEXITDIALOG(QString("Total in file [%1] did not match calculated total %2").arg(fields.at(3)).arg(entry.getTotal()));
    }
  }
}


