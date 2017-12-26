#-------------------------------------------------
#
# Project created by QtCreator 2017-12-12T09:08:35
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeviceManage
TEMPLATE = app

RC_FILE = logo.rc

SOURCES += main.cpp\
    mainwidget.cpp \
    configini.cpp \
    flowlayout.cpp \
    WgjHiRedis.cpp \
    servicemanage.cpp \
    SafeManageMsg.pb.cc \
    redisthread.cpp \
    mainwidget.cpp \
    main.cpp \
    flowlayout.cpp \
    devicedatadisp.cpp \
    databasehelper.cpp \
    configini.cpp \
    redis/win32fixes.c

HEADERS  += \
    stylehelper.h \
    mainwidget.h \
    configini.h \
    datatype.h \
    flowlayout.h \
    WgjHiRedis.h \
    stylehelper.h \
    servicemanage.h \
    SafeManageMsg.pb.h \
    redisthread.h \
    mainwidget.h \
    flowlayout.h \
    devicedatadisp.h \
    datatype.h \
    databasehelper.h \
    configini.h \
    redis/win32fixes.h

FORMS    += \
    mainwidget.ui \
    servicemanage.ui \
    devicedatadisp.ui

RESOURCES += \
    DeviceManage.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobufd
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf-lite
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf-lited
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotoc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotocd
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotoc

INCLUDEPATH += $$PWD/protobuf
DEPENDPATH += $$PWD/protobuf

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/redis/lib/ -lhiredis
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/redis/lib/ -lhiredisd
else:unix: LIBS += -L$$PWD/redis/lib/ -lhiredis

INCLUDEPATH += $$PWD/redis
DEPENDPATH += $$PWD/redis


INCLUDEPATH += $$PWD/protobuf
DEPENDPATH += $$PWD/protobuf
