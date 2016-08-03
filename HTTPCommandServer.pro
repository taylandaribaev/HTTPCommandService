#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = HTTPCommandServer
TEMPLATE = app

CONFIG += c++11

CONFIG(debug, debug|release){
    DESTDIR = ../../bin/debug
}else{
    DESTDIR = ../../bin/release
}

win32 {
    mingw {
        INCLUDEPATH += ./
        INCLUDEPATH += /usr/include
        LIBS += -L/usr/lib
    }else{
        INCLUDEPATH += ./
        CONFIG(debug, debug|release){
            LIBS += -L$$PWD/bin/debug/
        }else{
            LIBS += -L$$PWD/bin/release/
        }
        LIBS += -lHTTPCommandService
    }
}
unix {
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib
}

SOURCES += main.cpp

#LIBS += -lHTTPCommandService -ldl
