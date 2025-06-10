QT          += widgets
QT          += serialport

CONFIG += C++11

win32:RC_FILE = app.rc

#1#
#CONFIG+= staticlib

HEADERS     = \
              widget.h \
              window.h \
              vismo.h \
              pbsetup.h \
    appset.h \
    saver.h \
    admin.h \
    cserialport.h \
    cserialport.h \
    wappsett.h \
    wabout.h \
    wlog.h \
    processing.h \
    wwarning.h \
    dbg.h \
    wconfirm.h

SOURCES     = \
              main.cpp \
              widget.cpp \
              window.cpp \
              vismo.cpp \
              pbsetup.cpp \
    appset.cpp \
    saver.cpp \
    admin.cpp \
    cserialport.cpp \
    wappsett.cpp \
    wabout.cpp \
    wlog.cpp \
    processing.cpp \
    wwarning.cpp \
    wconfirm.cpp
#2#
#INCLUDEPATH += ../staticLibrary
#LIBS+= -L../staticLibrary/debug -lstaticLibrary

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/2dpainting
INSTALLS += target

FORMS += \
              pbsetup.ui \
    appset.ui \
    admin.ui \
    wappsett.ui \
    wabout.ui \
    wlog.ui \
    processing.ui \
    wwarning.ui \
    wconfirm.ui
