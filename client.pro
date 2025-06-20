QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriend.cpp \
    airecv.cpp \
    cellmain.cpp \
    chatdb.cpp \
    chatwindows.cpp \
    checkpassword.cpp \
    circleoffriends.cpp \
    deletefriend.cpp \
    dialog.cpp \
    emoji.cpp \
    friendsitem.cpp \
    friendworksitem.cpp \
    main.cpp \
    my_tcpsocket.cpp \
    pushworks.cpp \
    signup.cpp

HEADERS += \
    addfriend.h \
    airecv.h \
    cellmain.h \
    chatdb.h \
    chatwindows.h \
    checkpassword.h \
    circleoffriends.h \
    deletefriend.h \
    dialog.h \
    emoji.h \
    friendsitem.h \
    friendworksitem.h \
    my_tcpsocket.h \
    pushworks.h \
    showMessage.h \
    signup.h

FORMS += \
    addfriend.ui \
    cellmain.ui \
    chatwindows.ui \
    checkpassword.ui \
    circleoffriends.ui \
    deletefriend.ui \
    dialog.ui \
    emoji.ui \
    friendsitem.ui \
    friendworksitem.ui \
    pushworks.ui \
    signup.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = wx.icns

RESOURCES += \
    res.qrc
