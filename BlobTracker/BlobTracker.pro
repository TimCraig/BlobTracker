#-------------------------------------------------
#
# Project created by QtCreator 2014-04-17T14:48:25
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = BlobTrackerApp
TEMPLATE = app

include(D:/Projects/Workspace/ProjectsCommon/Druai.pri)
include(D:/Projects/Workspace/ProjectsCommon/Boost.pri)
include(D:/Projects/Workspace/ProjectsCommon/OpenCV.pri)

SOURCES += BlobTrackerApp.cpp \
        DBlobTrackerMainWindow.cpp \
        DColorRangeTableWidget.cpp \
        DBlobs.cpp \
        DColorWidget.cpp

HEADERS  += DBlobTracker.h \
            DBlobTrackerMainWindow.h \
            DColorRangeTableWidget.h \
            DBlobs.h \
            DColorWidget.h \
    ../Druai/DMath.h

CONFIG += c++14

