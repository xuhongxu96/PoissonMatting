#-------------------------------------------------
#
# Project created by QtCreator 2016-12-06T10:55:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PoissonMatting
TEMPLATE = app

INCLUDEPATH += "D:/opencv/build/include"

LIBS += -LD:/opencv/build/x64/vc14/lib
CONFIG(debug, debug|release)
{
    LIBS += -lopencv_world310d
}
CONFIG(release, debug|release)
{
    LIBS += -lopencv_world310
}

SOURCES += main.cpp\
        mainwindow.cpp \
    poissonmatting.cpp \
    mattingcanvas.cpp

HEADERS  += mainwindow.h \
    poissonmatting.h \
    mattingcanvas.h

FORMS    += mainwindow.ui