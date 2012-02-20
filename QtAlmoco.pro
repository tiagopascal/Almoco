#-----------------------------------------------------------
#
# Project created by NecessitasQtCreator 2012-02-04T14:20:46
#
#-----------------------------------------------------------

QT       += core gui sql network

TARGET = Almoco
TEMPLATE = app

SOURCES += \
    source/main.cpp \
    source/almoco.cpp \
    source/funcoes.cpp \
    source/ConexaoBancoAtendimento.cpp

HEADERS  += \
    header/almoco.h \
    header/funcoes.h \
    header/ConexaoBancoAtendimento.h

FORMS    += \
    ui/almoco.ui

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe44abb40
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

OTHER_FILES += \
    android/res/values-ro/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-el/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/layout/splash.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-fr/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-id/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/res/values-pt-rBR/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/values-fr/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pl/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-es/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-et/strings.xml \
    android/version.xml

RESOURCES += \
    imagens.qrc











