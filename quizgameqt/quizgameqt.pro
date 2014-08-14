TEMPLATE = app

QT += qml quick widgets
LIBS += -L../librgbmatrix -lrgbmatrix -lfreetype -lpng -lz
INCLUDEPATH += ../librgbmatrix /usr/include/freetype2

QMAKE_CXXFLAGS += -fpermissive

SOURCES += main.cpp \
    qquiztimecounter.cpp \
    qquizquestions.cpp \
    quizquestion.cpp \
    qquizuicontroller.cpp \
    qquizgamestate.cpp \
    qquizresult.cpp \
    qquizresults.cpp \
    qquizbuttonthread.cpp \
    qquizleddisplay.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    qquiztimecounter.h \
    qquizquestions.h \
    quizquestion.h \
    qquizuicontroller.h \
    qquizgamestate.h \
    qquizresult.h \
    qquizresults.h \
    qquizbuttonthread.h \
    defines.h \
    qquizleddisplay.h

OTHER_FILES += \
    questions.json \
    ff-strike3.wav


