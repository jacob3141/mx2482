QT += core gui widgets
OBJECTS_DIR = obj
MOC_DIR = moc
DESTDIR = bin
TARGET = mx2482
TEMPLATE = app
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS += -O3
CONFIG -= console
CONFIG += flat

INCLUDEPATH += ../libqjackaudio

LIBS += -L../libqjackaudio/lib \
                -lqjackaudio \
                -ljack \
                -lfftw3

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    channelwidget.cpp \
    mainmixerwidget.cpp \
    aboutdialog.cpp

HEADERS += \
    mainwindow.h \
    channelwidget.h \
    mainmixerwidget.h \
    aboutdialog.h

FORMS += \
    mainwindow.ui \
    channelwidget.ui \
    mainmixerwidget.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc

