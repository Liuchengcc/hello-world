INCLUDEPATH += $$PWD/lib
INCLUDEPATH += $$PWD/h
LIBS += -L$$PWD/lib


CONFIG(debug, debug|release){
    win32 {
        LIBS += -lqtadvanceddockingd
    }
    else:mac {
        LIBS += -lqtadvanceddocking_debug
    }
    else {
        LIBS += -lqtadvanceddocking
    }
}
else{
    LIBS += -lqtadvanceddocking
}

unix:!macx {
    LIBS += -lxcb
    QT += x11extras
}
