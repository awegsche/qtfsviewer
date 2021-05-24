QT       += core gui printsupport opengl

DEFINES += QCUSTOMPLOT_USE_OPENGL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/darkstyle.cpp \
    src/main.cpp \
    src/qcustomplot.cpp \
    src/qfilterworker.cpp \
    src/tfsdatafiltermodel.cpp \
    src/tfsmodel.cpp \
    src/tfspropertymodel.cpp \
    src/viewer.cpp \

HEADERS += \
    src/darkstyle.h \
    src/data.h \
    src/qcustomplot.h \
    src/qfilterworker.h \
    src/tfs_dataframe.h \
    src/tfsdatafiltermodel.h \
    src/tfshelper.h \
    src/tfsmodel.h \
    src/tfspropertymodel.h \
    src/viewer.h \
    src/tfs_dataframe.h \

FORMS += \
    viewer.ui

TRANSLATIONS += \
    tfsviewer_en_150.ts

DEFINES += QCUSTOMPLOT_USE_OPENGL

LIBS += -lopengl32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
