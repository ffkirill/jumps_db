CONFIG += c++11

TEMPLATE = app

QT += qml quick sql

SOURCES += main.cpp \
    jumpssqlmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
#include(deployment.pri)
#include(material/material.pri)

HEADERS += \
    jumpssqlmodel.h

OTHER_FILES += \
    main.qml

DISTFILES += \
    modules/ManifestorLoader.qml \
    modules/JumpsListView.qml \
    modules/AllJumpsTab.qml \
    modules/ReportsTab.qml
