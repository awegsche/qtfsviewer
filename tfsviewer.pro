QT       += core gui printsupport opengl

DEFINES += QCUSTOMPLOT_USE_OPENGL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += K:/projects/tfs_cpp/src

SOURCES += \
    darkstyle.cpp \
    main.cpp \
    qcustomplot.cpp \
    qfilterworker.cpp \
    tfsdatafiltermodel.cpp \
    tfsmodel.cpp \
    tfspropertymodel.cpp \
    viewer.cpp \

HEADERS += \
    darkstyle.h \
    data.h \
    qcustomplot.h \
    qfilterworker.h \
    tfs_dataframe.h \
    tfsdatafiltermodel.h \
    tfshelper.h \
    tfsmodel.h \
    tfspropertymodel.h \
    viewer.h \
    tfs_dataframe.h \

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
