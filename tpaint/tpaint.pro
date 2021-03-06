######################################################################
# Automatically generated by qmake (2.01a) Wed Nov 24 17:38:18 2010
######################################################################

TEMPLATE = app
TARGET = tpaint
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += 	mainform.h \
		graphwidget.h \
		logdialog.h \
		connectiondialog.h \
		graphtools/palette.h \
		graphtools/graphtoolsclass.h \
	        network/connections.h \
	        network/serverclass.h \
	        network/clientclass.h \
                optionsdialog.h

SOURCES += 	mainform.cpp \
        	main.cpp \
		graphwidget.cpp \
		logdialog.cpp \
		connectiondialog.cpp \
		graphtools/palette.cpp \
		graphtools/graphtoolsclass.cpp \
		network/connections.cpp \
	        network/serverclass.cpp \
	        network/clientclass.cpp \
                optionsdialog.cpp

QT              += network widgets gui
RESOURCES       += ../artwork/images.qrc
TRANSLATIONS    += ../i18n/tpaint_ru.ts
