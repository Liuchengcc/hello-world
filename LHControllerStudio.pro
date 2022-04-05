#-------------------------------------------------
#
# Project created by QtCreator 2021-07-01T14:09:33
#
#-------------------------------------------------

QT       += core gui network

CONFIG(debug, debug|release){
DEFINES += IS_DEBUG=1
}
else{
DEFINES += IS_DEBUG=0
}
DEFINES += APP_VERSION=0.1

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-implicit-fallthrough
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#check *.i files
#QMAKE_CXXFLAGS += -save-temps

include($$PWD/component/component.pri)
TARGET = LHControllerStudio
TEMPLATE = app
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

MOC_DIR = temp/moc
RCC_DIR = temp/rcc
UI_DIR = temp/ui
OBJECTS_DIR = temp/obj

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        config.cpp \
        configinterface.cpp \
        createprjdialog.cpp \
        iconhelper.cpp \
        instanceinterface.cpp \
        mainwindow.cpp \
        windowsmanager.cpp\
        globalfunction.cpp\
        basewidget.cpp\
        CRC32.c\
        ftpmanager.cpp

HEADERS  += mainwindow.h \
    config.h \
    configinterface.h \
    createprjdialog.h \
    iconhelper.h \
    instanceinterface.h \
    marcroly.h \
    windowsmanager.h\
    globalfunction.h\
    bzresources.h\
    CRC32.h\
    ftpmanager.h\
    basewidget.h

FORMS    += mainwindow.ui \
  createprjdialog.ui

RESOURCES += qss.qrc\
             lhresfile.qrc \
             main.qrc
