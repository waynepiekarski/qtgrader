#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T23:41:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtgrader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gradewindow.cpp \
    startup.cpp \
    debug.cpp \
    global.cpp

HEADERS  += mainwindow.h \
    gradewindow.h \
    database.h \
    pages.h \
    debug.h \
    startup.h \
    global.h

FORMS    += mainwindow.ui \
    gradewindow.ui \
    startup.ui
