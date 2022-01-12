QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp

HEADERS += \
    head.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    image/0l.jpg \
    image/0s.jpg \
    image/10l.jpg \
    image/10s.jpg \
    image/11l.jpg \
    image/11s.jpg \
    image/12l.jpg \
    image/12s.jpg \
    image/13l.jpg \
    image/13s.jpg \
    image/14l.jpg \
    image/14s.jpg \
    image/15l.jpg \
    image/15s.jpg \
    image/1l.jpg \
    image/1s.jpg \
    image/2l.jpg \
    image/2s.jpg \
    image/3l.jpg \
    image/3s.jpg \
    image/4l.jpg \
    image/4s.jpg \
    image/5l.jpg \
    image/5s.jpg \
    image/6l.jpg \
    image/6s.jpg \
    image/7l.jpg \
    image/7s.jpg \
    image/8l.jpg \
    image/8s.jpg \
    image/9l.jpg \
    image/9s.jpg \
    input.txt \
    libws2_32.a

LIBS += -lpthread libwsock32 libws2_32
