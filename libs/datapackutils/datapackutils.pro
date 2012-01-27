# toolkit library project file
TEMPLATE        = lib
TARGET          = DataPack

# include config file
include(../libsworkbench.pri)
include(../datapackutils_dependencies.pri)

DEFINES *= DATAPACKUTILS_LIBRARY

HEADERS += datapack_exporter.h \
    servermanager.h \
    server.h \
    serverdescription.h \
    packdescription.h \
    pack.h \
    serveridentification.h \
    servercontent.h \
    datapackcore.h \
    iservermanager.h \
    widgets/packmanager.h \
    widgets/addserverdialog.h \
    widgets/installpackdialog.h \
    serverandpackmodel.h \
    iserverengine.h \
    serverengines/localserverengine.h \
    serverengines/httpserverengine.h \
    packmodel.h \
    servermodel.h

SOURCES += \
    servermanager.cpp \
    server.cpp \
    serverdescription.cpp \
    packdescription.cpp \
    pack.cpp \
    serveridentification.cpp \
    servercontent.cpp \
    datapackcore.cpp \
    iservermanager.cpp \
    widgets/packmanager.cpp \
    widgets/addserverdialog.cpp \
    widgets/installpackdialog.cpp \
    serverandpackmodel.cpp \
    iserverengine.cpp \
    serverengines/localserverengine.cpp \
    serverengines/httpserverengine.cpp \
    packmodel.cpp \
    servermodel.cpp

FORMS += \
    widgets/packmanager.ui \
    widgets/addserverdialog.ui \
    widgets/installpackdialog.ui










































