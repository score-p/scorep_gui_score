##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2016,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scorep-score-gui
TEMPLATE = app
# the dash separates the version from the package name
VERSION = -2

CONFIG += warn_on debug_and_release
!win32:CONFIG += silent

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/connector.cpp \
        src/score/SCOREP_Score_Estimator.cpp \
        src/score/SCOREP_Score_Profile.cpp \
        src/score/SCOREP_Score_Event.cpp \
        src/score/SCOREP_Score_Group.cpp \
        src/score/SCOREP_Score_Types.cpp

HEADERS  += src/mainwindow.hpp \
            src/connector.hpp \
            src/score/SCOREP_Score_Estimator.hpp \
            src/score/SCOREP_Score_Profile.hpp \
            src/score/SCOREP_Score_Event.hpp \
            src/score/SCOREP_Score_Group.hpp \
            src/score/SCOREP_Score_Types.hpp \
            src/score/SCOREP_Score_EventList.hpp \
            src/data.hpp

CUBE_CONFIG = cube-config

INCLUDEPATH += $$system($$CUBE_CONFIG --cube-include-path)

LIBS += $$system($$CUBE_CONFIG --cube-ldflags)

LIBS += $$system($$CUBE_CONFIG --cubew-shared-ldflags)

DISTFILES += images/save.png \
             images/openFile.png \
             images/exit.png \
             README.md \
             COPYING

RESOURCES += src.qrc

# Uncrustify Setting. add custom build target: use 'make beautify'
beautify.target = beautify
beautify.commands = @cd $$PWD; $$escape_expand(\\)$$escape_expand(\\n\\t)list=$$escape_expand(\')$$HEADERS $$SOURCES$$escape_expand(\'); $$escape_expand(\\)$$escape_expand(\\n\\t)for f in $$escape_expand(\$\$)list; do $$escape_expand(\\)$$escape_expand(\\n\\t) echo beautifying $$escape_expand(\$\$)f && $$escape_expand(\\)$$escape_expand(\\n\\t) uncrustify -c beautify.cfg --no-backup -q $$escape_expand(\$\$)f || $$escape_expand(\\)$$escape_expand(\\n\\t) exit 1; $$escape_expand(\\)$$escape_expand(\\n\\t)done
QMAKE_EXTRA_TARGETS += beautify
DISTFILES += beautify.cfg
