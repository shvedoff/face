#-------------------------------------------------
#
# Project created by QtCreator 2017-04-22T20:36:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
INCLUDEPATH += /usr/local/include/opencv2
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_flann
LIBS += -lopencv_videoio

INCLUDEPATH += /usr/local/include/
LIBS += -lfftw3

TARGET = projj
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    saver.cpp \
    logger.cpp \
    qcustomplot.cpp \
    imageprocessor.cpp \
    keepncalc.cpp

HEADERS  += mainwindow.h \
    saver.h \
    logger.h \
    qcustomplot.h \
    imageprocessor.h \
    fir_tuk74.h \
    keepncalc.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS+=-O2 \

DISTFILES += \
    1.png
