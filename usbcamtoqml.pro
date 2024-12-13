QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        videocapture.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

#test1

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#this section includes the used opencv libraries
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_highgui490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_core490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_imgcodecs490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_videoio490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_imgproc490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_objdetect490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_dnn490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_calib3d490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_features2d490.dll
win32: LIBS += -L'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/x64/mingw/lib/' -llibopencv_flann490.dll

INCLUDEPATH += 'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/include'
DEPENDPATH += 'C:/Users/BEN ALI/Documents/opencv4.9.0_mingw/include'

HEADERS += \
    videocapture.h
