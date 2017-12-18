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
    servicemanage.cpp \
    flowlayout.cpp \
    devicedatadisp.cpp \
    SafeManageMsg.pb.cc

HEADERS  += \
    stylehelper.h \
    databasehelper.h \
    mainwidget.h \
    configini.h \
    servicemanage.h \
    datatype.h \
    flowlayout.h \
    devicedatadisp.h \
    SafeManageMsg.pb.h

FORMS    += \
    mainwidget.ui \
    servicemanage.ui \
    devicedatadisp.ui

RESOURCES += \
    DeviceManage.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf-lite
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf-lite
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotoc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/protobuf/lib/ -llibprotoc
else:unix: LIBS += -L$$PWD/protobuf/lib/ -llibprotobuf

INCLUDEPATH += $$PWD/protobuf
DEPENDPATH += $$PWD/protobuf
