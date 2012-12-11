#pragma once

#include <QtGlobal>
#include <vector>

#include "gradewindow.h"
#include "debug.h"
#include "pages.h"

static QString getStrFromGrade(int grade)
{
  if (grade >= 0)
    return QString("%1").arg(grade);
  else
    return QString("");
}

class GradeArray
{
public:
  GradeArray(size_t size) :
    _values(size, -1),
    _feedback(size)
  { }

  void set(size_t elem, int value)
  {
    GASSERT((value == -1)||(value >= 0), "Value suppled %d is not a valid grade", value);
    GASSERT(elem < _values.size(), "Element %zu is out of bounds for %zu elements", elem, _values.size());
    _values[elem] = value;
  }

  int get(size_t elem)
  {
    GASSERT(elem < _values.size(), "Element %zu is out of bounds for %zu elements", elem, _values.size());
    int value = _values[elem];
    GASSERT((value == -1)||(value >= 0), "Value retrieved %d is not a valid grade", value);
    return value;
  }

  const QString& getFeedback(size_t elem)
  {
    GASSERT(elem < _values.size(), "Element %zu is out of bounds for %zu elements", elem, _values.size());
    return _feedback[elem];
  }

  void setFeedback(size_t elem, const QString& str)
  {
    GASSERT(elem < _values.size(), "Element %zu is out of bounds for %zu elements", elem, _values.size());
    Q_ASSERT(elem < _values.size());
    _feedback[elem] = str;
  }

  size_t size() { return _values.size(); }

private:
  std::vector<int> _values;
  std::vector<QString> _feedback;
};

class Student
{
public:
  Student(size_t seq, size_t numQuestions) :
    _seq(seq),
    _grades(numQuestions)
  { }

  size_t getSeq() { return _seq; }
  int getGrade(size_t question)
  {
    return _grades.get(question);
  }
  const QString& getFeedback(size_t question)
  {
    return _grades.getFeedback(question);
  }

  static GradeWindow *gw();
  const QString& getStudentName() { return _name; }
  const QString& getStudentId() { return _studentid; }
  void setGrade(size_t question, int in, bool check = true) { if (check) { GASSERT(gw()->getGrade(_seq,question)==getStrFromGrade(_grades.get(question)), "GW mismatch"); gw()->setGrade(_seq,question,in); } _grades.set(question, in); }
  void setFeedback(size_t question, const QString& in, bool check = true) { if (check) { GASSERT(gw()->getFeedback(_seq,question)==_grades.getFeedback(question), "GW mismatch"); gw()->setFeedback(_seq,question,in); } _grades.setFeedback(question, in);  }
  void setStudentName(const QString& in, bool check = true) { if (check) { GASSERT(gw()->getStudentName(_seq)==_name, "GW mismatch"); gw()->setStudentName(_seq,in); } _name = in; }
  void setStudentId(const QString& in, bool check = true) { if (check) { GASSERT(gw()->getStudentId(_seq)==_studentid, "GW mismatch"); gw()->setStudentId(_seq,in); } _studentid = in; }

private:
  size_t _seq;
  QString _name;
  QString _studentid;
  GradeArray  _grades;
};

class StudentArray
{
public:
  StudentArray(size_t numStudents, size_t numQuestions)
  {
    GASSERT(numStudents > 0, "Number of students %zu is not valid", numStudents);
    for (size_t i = 0; i < numStudents; i++)
      _students.push_back(Student(i, numQuestions));
  }

  Student& get(size_t elem)
  {
    GASSERT(elem < _students.size(), "Element %zu is out of bounds for %zu elements", elem, _students.size());
    GASSERT(elem == _students[elem].getSeq(), "Element %zu does not match stored sequence number %zu", elem, _students[elem].getSeq());
    return _students[elem];
  }

  size_t size() { return _students.size(); }

private:
  std::vector<Student> _students;
};


class Database
{
  friend class Global;

private:
  Database(size_t numStudents, size_t numQuestions) :
    _maxGrades(numQuestions),
    _students(numStudents, numQuestions)
  { }

public:
  static GradeWindow *gw();
  size_t getNumQuestions() { return _maxGrades.size(); }
  size_t getNumStudents() { return _students.size(); }
  Student& getStudent(size_t elem) { return _students.get(elem); }
  int getQuestionMaximum(size_t question) { return _maxGrades.get(question); }
  void setQuestionMaximum(size_t question, int in, bool check = true) { if (check) { GASSERT(gw()->getMaximum(question)==getStrFromGrade(_maxGrades.get(question)), "GW mismatch"); gw()->setMaximum(question, in); } _maxGrades.set(question, in); }

private:
  GradeArray _maxGrades;
  StudentArray _students;
};
