#-------------------------------------------------
#
# Project created by QtCreator 2017-06-03T22:16:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hospitalSystem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logwidget.cpp \
    addpatient.cpp \
    myTcpsocket.cpp \
    patientbydoctor.cpp \
    searchwidget.cpp \
    searchoutput.cpp \
    patientbynurse.cpp \
    buymedicine.cpp \
    mifeedback.cpp \
    doctorfeedback.cpp

HEADERS  += mainwindow.h \
    logwidget.h \
    addpatient.h \
    myTcpsocket.h \
    patientbydoctor.h \
    searchwidget.h \
    searchoutput.h \
    patientbynurse.h \
    buymedicine.h \
    mifeedback.h \
    doctorfeedback.h

FORMS    += mainwindow.ui \
    logwidget.ui \
    addpatient.ui \
    patientbydoctor.ui \
    searchwidget.ui \
    searchoutput.ui \
    patientbynurse.ui \
    buymedicine.ui \
    medicineinsfeedback.ui \
    mifeedback.ui \
    doctorfeedback.ui

RESOURCES += \
    resource.qrc
