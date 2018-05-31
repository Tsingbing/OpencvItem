TEMPLATE = lib
TARGET = OpencvItem
QT += qml quick
CONFIG += plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.robosea.opencv

# Input
SOURCES += \
        opencvitem_plugin.cpp \
        opencvitem.cpp \
    cameraThread/camerathread.cpp \
    perspectiveCorrect/PerspectiveCorrect.cpp \
    matchTemp/MatchTemp.cpp

HEADERS += \
        opencvitem_plugin.h \
        opencvitem.h \
    cameraThread/camerathread.h \
    perspectiveCorrect/PerspectiveCorrect.h \
    matchTemp/MatchTemp.h


LIBS += D:/opencv-3.2-build/install/x64/vc14/lib/opencv_aruco320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_bgsegm320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_bioinspired320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_calib3d320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_ccalib320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_core320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_datasets320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_dnn320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_dpm320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_face320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_features2d320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_flann320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_fuzzy320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_highgui320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_imgcodecs320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_imgproc320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_line_descriptor320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_ml320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_objdetect320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_optflow320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_phase_unwrapping320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_photo320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_plot320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_reg320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_rgbd320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_saliency320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_shape320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_stereo320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_stitching320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_structured_light320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_superres320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_surface_matching320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_text320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_tracking320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_video320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_videoio320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_videostab320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_xfeatures2d320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_ximgproc320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_xobjdetect320d.lib \
        D:/opencv-3.2-build/install/x64/vc14/lib/opencv_xphoto320d.lib

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
