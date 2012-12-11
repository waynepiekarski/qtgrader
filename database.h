#pragma once

#include <QtGlobal>
#include <vector>

#include "pages.h"

class GradeArray
{
public:
  GradeArray(size_t size) :
    _values(size, -1),
    _feedback(size)
  { }

  void set(size_t elem, int value)
  {
    Q_ASSERT((value == -1)||(value >= 0));
    Q_ASSERT(elem < _values.size());
    _values[elem] = value;
  }

  int get(size_t elem)
  {
    Q_ASSERT(elem < _values.size());
    int value = _values[elem];
    Q_ASSERT((value == -1)||(value >= 0));
    return value;
  }

  const QString& getFeedback(size_t elem)
  {
    Q_ASSERT(elem < _values.size());
    return _feedback[elem];
  }

  void setFeedback(size_t elem, const QString& str)
  {
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
  Student(size_t numQuestions) :
    _grades(numQuestions)
  { }

  int getGrade(size_t question) { return _grades.get(question); }
  const QString& getFeedback(size_t question) { return _grades.getFeedback(question); }
  const QString& getStudentName() { return _name; }
  const QString& getStudentId() { return _studentid; }
  void setGrade(size_t question, int in) { _grades.set(question, in); }
  void setFeedback(size_t question, const QString& in) { _grades.setFeedback(question, in); }
  void setStudentName(const QString& in) { _name = in; }
  void setStudentId(const QString& in) { _studentid = in; }

private:
  QString _name;
  QString _studentid;
  GradeArray  _grades;
};

class StudentArray
{
public:
  StudentArray(size_t numStudents, size_t numQuestions)
  {
    Q_ASSERT(numStudents > 0);
    for (size_t i = 0; i < numStudents; i++)
      _students.push_back(Student(numQuestions));
  }

  Student& get(size_t elem)
  {
    Q_ASSERT(elem < _students.size());
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
  size_t getNumQuestions() { return _maxGrades.size(); }
  size_t getNumStudents() { return _students.size(); }
  Student& getStudent(size_t elem) { return _students.get(elem); }
  int getQuestionMaximum(size_t question) { return _maxGrades.get(question); }
  void setQuestionMaximum(size_t question, int in) { _maxGrades.set(question, in); }

private:
  GradeArray _maxGrades;
  StudentArray _students;
};
