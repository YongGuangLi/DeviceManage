#-------------------------------------------------
#
# Project created by QtCreator 2017-12-12T09:08:35
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeviceManage
TEMPLATE = app


SOURCES += main.cpp\
    databasehelper.cpp \
    mainwidget.cpp \
    configini.cpp \
    servicemanage.cpp

HEADERS  += \
    stylehelper.h \
    databasehelper.h \
    mainwidget.h \
    configini.h \
    servicemanage.h \
    datatype.h

FORMS    += \
    mainwidget.ui \
    servicemanage.ui

OTHER_FILES +=

RESOURCES += \
    DeviceManage.qrc
