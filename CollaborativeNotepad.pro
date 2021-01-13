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
    connect_page.cpp \
    menu.cpp \
    notepad.cpp \
    ServerConnection.cpp \
    operation.cpp \
    network_utils.cpp

HEADERS  += \
    connect_page.h \
    menu.h \
    notepad.h \
    network_utils.h \
    ServerConnection.h \
    operation.h \
    colors.h

FORMS    += \
    connect_page.ui \
    menu.ui \
    notepad.ui

RESOURCES += \
    resource.qrc
