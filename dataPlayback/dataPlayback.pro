TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

# OpenCV
LIBS += $(shell pkg-config opencv --libs)

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_shape -lopencv_videoio -lopencv_imgproc

# CeleX"
#INCLUDEPATH += ../../Samples/include

#unix:!macx: LIBS += -L$$PWD/../../Samples/lib/Linux/x64/ -lCeleX

#INCLUDEPATH += $$PWD/../../Samples/lib/Linux/x64
#DEPENDPATH += $$PWD/../../Samples/lib/Linux/x64
