#-------------------------------------------------
#
# Project created by QtCreator 2016-10-09T03:12:12
#
#-------------------------------------------------

#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pupil_extract
TEMPLATE = app


SOURCES += main.cpp \
    tempMatchPupilDetect.cpp \
    motionToColor.cpp \
    opticalFlowRoiDetect.cpp \
    imgDiffProcess.cpp \
    pupilDetect.cpp




win32{
    INCLUDEPATH += D:\opencv\build\include
    LIBS += -LD:\opencv\build\x64\vc12\lib
    #LIBS += -lopencv_core249 -lopencv_highgui249 -lopencv_imgproc249

    CONFIG(debug,debug|release) {
        message("debug mode")
        LIBS += -lopencv_world310d
    }else {
        message("release mode")
        LIBS += -lopencv_world310
    }
}



unix{
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -lopencv_objdetect -lopencv_video
}



HEADERS += \
    tempMatchPupilDetect.h \
    motionToColor.h \
    opticalFlowRoiDetect.h \
    imgDiffProcess.h \
    pupilDetect.h \
    programMode.h
