#pragma once

#include <QtGlobal>
#include <string>
#include <vector>

#include "pages.h"

class GradeArray
{
public:
  GradeArray(size_t size) :
    _values(size, -1)
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

private:
  std::vector<int> _values;
};

class Student
{
public:
  Student(size_t numQuestions) :
    _grades(numQuestions)
  { }

private:
  std::string _name;
  std::string _studentid;
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

private:
  std::vector<Student> _students;
};


class Database
{
  friend class Global;

private:
  Database(size_t numStudents, size_t numQuestions) :
    _numQuestions(numQuestions),
    _maxGrades(numQuestions),
    _students(numStudents, numQuestions)
  { }

private:
  size_t _numQuestions;
  GradeArray _maxGrades;
  StudentArray _students;
};
