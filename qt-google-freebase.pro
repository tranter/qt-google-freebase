#-------------------------------------------------
#
# Project created by QtCreator 2012-04-25T12:53:55
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = qtfreebase
TEMPLATE = app

INCLUDEPATH += ../qjson/include

linux* {
LIBS += ../qjson/build/lib/libqjson.so
}

win32-g++ {
LIBS += ../qjson/build/lib/qjson0.dll
}

win32-msvc* {
LIBS += ../qjson/build/lib/qjson0.lib
}

macx* {
LIBS += -F../qjson/build/lib -framework qjson
}

SOURCES += main.cpp\
        mainwindow.cpp \
    oauth2.cpp \
    logindialog.cpp \
    form.cpp \
    freebase_data_manager.cpp \
    treejsonmodel.cpp \
    treejsonitem.cpp

HEADERS  += mainwindow.h \
    oauth2.h \
    logindialog.h \
    form.h \
    freebase_data_manager.h \
    treejsonmodel.h \
    treejsonitem.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    form.ui

RESOURCES += \
    resource.qrc
