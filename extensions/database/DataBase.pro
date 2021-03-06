#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = DataBase
TEMPLATE = lib

DEFINES += \
    #WITH_JAVASCRIPT \
    WITH_COOKIE \
    WITH_JSONCPP \
    #WITH_TINYXML2 \
    #WITH_SSL \
    WITH_CTPP \
    WITH_INICONFIG \
    WITH_POSTGRESQL


DEFINES += CRT_SECURE_NO_WARNINGS

CONFIG += c++11

win32 {

    mingw {
        #QMAKE_CXXFLAGS = -fPIC

        INCLUDEPATH += /usr/include
        LIBS += -L/usr/lib

        # Boost
        BOOSTDIR = $(EXTERNALDIR)/boost_1_54_0
        INCLUDEPATH += $${BOOSTDIR}
    } else {

        DEFINES += \
            CRT_SECURE_NO_WARNINGS \
            _WIN32_WINDOWS \
            BOOST_ASIO_HAS_MOVE

        contains(DEFINES, WITH_JSONCPP) {
            JSONDIR = $(EXTERNALDIR)/jsoncpp-src-0.5.0
            INCLUDEPATH += $${JSONDIR}/include

            debug {
                LIBS += -L$${JSONDIR}/lib/debug
            } else {
                LIBS += -L$${JSONDIR}/lib/release
            }
        }

        contains(DEFINES, WITH_TINYXML2) {
            TINYXMLDIR = $(EXTERNALDIR)/tinyxml2-master
            INCLUDEPATH += $${TINYXMLDIR}
            LIBS += -L$${TINYXMLDIR}/lib
        }

        # Boost
        BOOSTDIR = $(EXTERNALDIR)/boost_1_56_0
        INCLUDEPATH += $${BOOSTDIR}
        LIBS += -L$${BOOSTDIR}/lib32-msvc-12.0
    }
}

unix {
    QMAKE_CXXFLAGS = -fPIC
    INCLUDEPATH += ./

    INCLUDEPATH += /usr/include/jsoncpp
    INCLUDEPATH += /usr/include/inih
    INCLUDEPATH += /usr/include/HTTPCommandService
    INCLUDEPATH += /usr/include/postgresql
    INCLUDEPATH += /usr/local/include/soci

}

HEADERS += \
    databasemodule.h \
    dbcontext.h \
    testcontroller.h

SOURCES += \
    databasemodule.cpp \
    dbcontext.cpp \
    testcontroller.cpp

win32 {
    mingw {
        BOOST_VER = 1_54
        COMPILER_SHORT = mgw48

        debug {
            BOOST_SUFFIX = $${COMPILER_SHORT}-sd-$${BOOST_VER}
        }

        release {
            BOOST_SUFFIX = $${COMPILER_SHORT}-s-$${BOOST_VER}
        }

        LIBS += -L$(EXTERNALDIR)/boost_1_54_0/stage/lib

        LIBS += -lboost_system-$${BOOST_SUFFIX} -lboost_filesystem-$${BOOST_SUFFIX}

        debug {
        LIBS += -lboost_thread-mgw48-mt-sd-1_54
        }

        release {
        LIBS += -lboost_thread-mgw48-mt-s-1_54
        }
    } else {
    }

    LIBS += -lws2_32 -lmswsock

}

unix {
    LIBS += -L/opt/local/lib
    LIBS += -L/usr/local/lib
    LIBS += -Wl,-rpath=/usr/local/lib
    LIBS += -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization
    LIBS += -lpthread
    #LIBS += -lssl -lcrypto
    LIBS += -linih
}

contains(DEFINES, WITH_JSONCPP) {
    win32 {
        mingw {
            LIBS += -ljsoncpp
        } else {
            LIBS += -ljsoncpp_mt
        }
    } else {
        LIBS += -ljsoncpp
    }
}

contains(DEFINES, WITH_TINYXML2) {
    LIBS += -tinyxml2
}

win32 {
    mingw {
        BOOST_VER = 1_54
        COMPILER_SHORT = mgw48

        debug {
            BOOST_SUFFIX = $${COMPILER_SHORT}-sd-$${BOOST_VER}
        }

        release {
            BOOST_SUFFIX = $${COMPILER_SHORT}-s-$${BOOST_VER}
        }

        LIBS += -L$(EXTERNALDIR)/boost_1_54_0/stage/lib

        LIBS += -lboost_system-$${BOOST_SUFFIX} -lboost_filesystem-$${BOOST_SUFFIX}

        debug {
        LIBS += -lboost_thread-mgw48-mt-sd-1_54
        }

        release {
        LIBS += -lboost_thread-mgw48-mt-s-1_54
        }
    } else {
    }

    LIBS += -lws2_32 -lmswsock

}

unix {
    LIBS += -L/opt/local/lib
    LIBS += -L/usr/local/lib
    LIBS += -Wl,-rpath=/usr/local/lib
    LIBS += -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization
    LIBS += -lpthread -ldl
    #LIBS += -lssl -lcrypto
}

contains(DEFINES, WITH_JSONCPP) {
    win32 {
        mingw {
            LIBS += -ljsoncpp
        } else {
            LIBS += -ljsoncpp_mt
        }
    } else {
        LIBS += -ljsoncpp
    }
}

contains(DEFINES, WITH_TINYXML2) {
    LIBS += -tinyxml2
}


# Install.
win32{
    INSTALL_PREFIX = /usr/include/$$TARGET
    INSTALL_HEADERS = $$HEADERS

    for(header, INSTALL_HEADERS) {
        path = $${INSTALL_PREFIX}/$${dirname(header)}
        eval(headers_$${path}.files += $$header)
        eval(headers_$${path}.path = $$path)
        eval(INSTALLS *= headers_$${path})
    }
    #target_headers.files  = $$HEADERS
    #target_headers.path = /usr/include/$$TARGET
    #INSTALLS += target_headers

    target.path = /usr/lib/
    INSTALLS += target
}


unix{
    INSTALL_PREFIX = /usr/include/$$TARGET
    INSTALL_HEADERS = $$HEADERS

    for(header, INSTALL_HEADERS) {
        path = $${INSTALL_PREFIX}/$${dirname(header)}
        eval(headers_$${path}.files += $$header)
        eval(headers_$${path}.path = $$path)
        eval(INSTALLS *= headers_$${path})
    }
    #target_headers.files  = $$HEADERS
    #target_headers.path = /usr/include/$$TARGET
    #INSTALLS += target_headers

    target.path = /usr/lib/
    INSTALLS += target
}

