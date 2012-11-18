######################################################################
# Automatically generated by qmake (2.01a) Sat Nov 7 01:19:33 2009
######################################################################

TEMPLATE = app
TARGET = rawcam
INCLUDEPATH += ../../include


CONFIG += debug warn_on
QT += declarative

CONFIG += qt mobility
LIBS += -lpthread -L../.. -ljpeg -lFCam
MOBILITY += sensors

HEADERS += OverlayWidget.h CameraThread.h ExampleOverlayWidget.h \
    myproximitysensor.h \
    CameraParameters.h \
    LEDBlinker.h \
    dot.h \
    feedback.h \
    RawcamApp.h \
    ImageItem.h \
    pasyncfilewriter.h \
    paccelerometer.h
SOURCES += rawcam.cpp OverlayWidget.cpp CameraThread.cpp ExampleOverlayWidget.cpp \
    myproximitysensor.cpp \
    CameraParameters.cpp \
    LEDBlinker.cpp \
    dot.cpp \
    feedback.cpp \
    RawcamApp.cpp \
    ImageItem.cpp \
    pasyncfilewriter.cpp \
    paccelerometer.cpp



OBJECTS_DIR = build
MOC_DIR     = build
UI_DIR      = build
DESTDIR     = build


INSTALLS += target
target.path = /opt/rawcam/bin

INSTALLS += syspart
syspart.path = /usr/share/policy/etc/syspart.conf.d
syspart.files = rawcam.syspart.conf

contains(MEEGO_EDITION,harmattan) {
    INSTALLS    += desktop
    desktop.path  = /usr/share/applications/
    desktop.files  = rawcam.desktop
}

INSTALLS    += style
style.path  = /opt/rawcam/
style.files  = style.css \
               icon.png \
               helpBg.png shutter.png halftone.png spot.png spotGrey.png \
               noflash.png noflashGrey.png \
               flashFull.png flashFullGrey.png \
               flashHalf.png flashHalfGrey.png \
               backCurtain.png backCurtainGrey.png \
               halftoneBlack.png \
               gallery.png settings.png settings.qml


#
# Targets for debian source and binary package creation

#debian-src.commands = dpkg-buildpackage -S -r -us -uc -d -I'\\.svn';
#debian-bin.commands = dpkg-buildpackage -b -r -uc -d;

#debian-all.depends = debian-src debian-bin

##-----------------------------
## Clean all but Makefile

#compiler_clean.commands = -$(DEL_FILE) $(TARGET)

#QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin compiler_clean

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    style.css \
    settings.qml \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    rawcamF.desktop


contains(MEEGO_EDITION,harmattan) {
    icon.files = rawcam.png
    icon.path = /usr/share/icons/hicolor/80x80/apps
    INSTALLS += icon
}

maemo5 {
    icon.files = rawcam.png
    icon.path = /usr/share/icons/hicolor/96x96/apps
    INSTALLS += icon
    INSTALLS    += desktop
    desktop.path  = /usr/share/applications/hildon
    desktop.files  = rawcamF.desktop
}

#while [ true ]; do chmod +x /tmp/qtc_packaging_rawcam/rawcam/debian/rules; done

#todo: check if dcim exists

