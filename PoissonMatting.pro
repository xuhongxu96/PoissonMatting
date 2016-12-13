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
DEBUG:LIBS += -lopencv_world310
RELEASE:LIBS += -lopencv_world310

SOURCES += main.cpp\
        mainwindow.cpp \
    poissonmatting.cpp \
    mattingcanvas.cpp

HEADERS  += mainwindow.h \
    poissonmatting.h \
    mattingcanvas.h

FORMS    += mainwindow.ui

DISTFILES += \
    README.md \
    poisson1.png \
    poisson2.png \
    poisson3.png \
    poisson4.png \
    poisson5.png \
    poisson6.png
