#-------------------------------------------------
#
# Project created by QtCreator 2017-12-30T13:26:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CollaborativeNotepad
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    documentstable.cpp \
    mainmenu.cpp \
    notepad.cpp \
    ServerConnection.cpp \
    operation.cpp \
    network_utils.cpp

HEADERS  += mainwindow.h \
    documentstable.h \
    mainmenu.h \
    notepad.h \
    hack_connect.h \
    network_utils.h \
    ServerConnection.h \
    operation.h \
    colors.h

FORMS    += mainwindow.ui \
    notepad.ui
