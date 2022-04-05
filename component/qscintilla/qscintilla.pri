LIBS += -L$$PWD/lib
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/Qsci

CONFIG(debug, debug|release){
    LIBS += -lqscintilla2_qt5d
}
else{
    LIBS += -lqscintilla2_qt5
}

HEADERS += $$PWD/Qsci/qsciscintilla.h
