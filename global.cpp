#include "gradewindow.h"
#include "database.h"
#include "pages.h"
#include "global.h"
#include "debug.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

GradeWindow* Global::_gw = NULL;
Database* Global::_db = NULL;
Pages* Global::_pages = NULL;
size_t Global::_numPagesPerStudent = 0;

void Global::save(QString filename)
{
  GASSERT(_db != NULL, "Cannot save empty database");

  QFile fd (filename);
  if (!fd.open(QIODevice::WriteOnly))
    GEXITDIALOG("Could not open project file for writing " + filename);
  QTextStream file (&fd);

  file << "VERSION=" << DB_VERSION << "\n";
  file << "IMAGE_PATH=" << Global::getPages()->getPath() << "\n";
  file << "QUESTIONS_PER_STUDENT=" << Global::db()->getNumQuestions() << "\n";
  GASSERT(_numPagesPerStudent > 0, "_numPagesPerStudent %zu is not valid", _numPagesPerStudent);
  file << "PAGES_PER_STUDENT=" << _numPagesPerStudent << "\n";
  file << "TOTAL_STUDENTS=" << Global::db()->getNumStudents() << "\n";
  file << "SAVE_DATE_TIME=" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";

  file << "-1\tMAX\tMAX";
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << Global::db()->getQuestionMaximum(q);
  for (size_t q = 0; q < Global::db()->getNumQuestions(); q++)
    file << "\t" << "NF"; // Emit empty feedback columns
  file << "\n";

  for (size_t s = 0; s < Global::db()->getNumStudents(); s++)
  {
    Student& student = Global::db()->getStudent(s);
    file << s << "\t" << student.getStudentId() << "\t" << student.getStudentName();
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
  // VERSION = %s
  // IMAGE_PATH = %s
  // QUESTIONS_PER_STUDENT = %d
  // PAGES_PER_STUDENT = %d
  // TOTAL_STUDENTS = %d
  // SAVE_DATE_TIME = %s
  struct _keyvalue { QString key; QString string; size_t number; };
  _keyvalue intro[6];
  QString header_names[6] = { "VERSION", "IMAGE_PATH", "QUESTIONS_PER_STUDENT", "PAGES_PER_STUDENT", "TOTAL_STUDENTS", "SAVE_DATE_TIME" };
  bool expect_numbers[6] = { false, false, true, true, true, false };
  for (size_t i = 0; i < 6; i++)
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

  // Initialize the database since we know all the dimensions now
  initDatabase(numStudents, numQuestions, numPagesPerStudent);

  // Load in the pages as well
  initPages(image_path);

  // Now populate the database. Each entry is of the following form:
  // IDNUM,STUDENTID,STUDENTNAME,Q0,Q1,..,QN,Feed0,Feed1,..,FeedN
  // All fields will contain something except for Feed0..FeedN
  for (int student = -1; student < (int)numStudents; student++)
  {
    if (file.atEnd())
      GEXITDIALOG(QString("Encountered end of file reading student %1 of %2").arg(student+1).arg(numStudents));
    QString in = file.readLine();
    QStringList fields = in.split("\t");
    if (fields.size() != (2*(int)numQuestions + 3))
      GEXITDIALOG(QString("Found only %1 columns in [%2] for student %3 of %4 when expected 2x%5+3 columns").arg(fields.size()).arg(in).arg(student+1).arg(numStudents).arg(numQuestions));
    for (size_t col = 3; col < numQuestions+3; col++) // Do not search student id/name, or any feedback, since these can be empty
      if (fields.at(col) == "")
        GEXITDIALOG(QString("Found empty column %1 for student %3 of %4").arg(col+1).arg(student+1).arg(numStudents));
    bool ok;
    int num = fields.at(0).toInt(&ok);
    if ((!ok) || (num != student))
      GEXITDIALOG(QString("Expected row number %1 but found [%2] instead for student %3 of %4").arg(student).arg(fields.at(0).arg(student+1).arg(numStudents)));

    if (student == -1)
    {
      if ((fields.at(1) != "MAX") && (fields.at(2) != "MAX"))
        GEXITDIALOG(QString("Expected MAXIMUM headers for first row of data, but found [%1] [%2] instead").arg(fields.at(1).arg(fields.at(2))));
      for (size_t q = 0; q < numQuestions; q++)
      {
        int score = fields.at(q+3).toInt(&ok);
        if ((!ok) || (num < -1))
          GEXITDIALOG(QString("Score value [%1] for question %2 was not valid student %3 of %4").arg(fields.at(q+3)).arg(q+1).arg(student+1).arg(numStudents));
        Global::db()->setQuestionMaximum(q, score);
      }
    }
    else
    {
      Student &entry = Global::db()->getStudent(student);
      entry.setStudentId(fields.at(1));
      entry.setStudentName(fields.at(2));
      for (size_t q = 0; q < numQuestions; q++)
      {
        int score = fields.at(q+3).toInt(&ok);
        if ((!ok) || (num < -1) || (num > Global::db()->getQuestionMaximum(q)))
          GEXITDIALOG(QString("Score value [%1] for question %2 was not valid student %3 of %4").arg(fields.at(q+3)).arg(q+1).arg(student+1).arg(numStudents));
        entry.setGrade(q, score);
      }
      for (size_t q = 0; q < numQuestions; q++)
        entry.setFeedback(q, fields.at(q + 3 + numQuestions));
    }
  }
}


