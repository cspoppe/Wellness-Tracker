QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql printsupport

CONFIG += c++17

QMAKE_CXXFLAGS += -Wa,-mbig-obj

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = Wellness_Tracker
TEMPLATE = app

# Tell the qcustomplot header that it will be used as library:
DEFINES += QCUSTOMPLOT_USE_LIBRARY

CONFIG(debug, release|debug) {
  win32:QCPLIB = qcustomplotd2
  else: QCPLIB = qcustomplotd
} else {
  win32:QCPLIB = qcustomplot2
  else: QCPLIB = qcustomplot
}

LIBS += -L$$PWD/./ -l$$QCPLIB

SOURCES += \
    createrecipedialog.cpp \
    editablesqlmodel.cpp \
    exercisemodel.cpp \
    foodsearchproxymodel.cpp \
    macrosmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    mealproxymodel.cpp \
    mealsstructure.cpp \
    mealstruct.cpp \
    mealsummarymodel.cpp \
    nutrientsmodel.cpp \
    recipeeditmodel.cpp \
    recipetablemodel.cpp \
    runningdatamodel.cpp \
    statsplotmodel.cpp \
    utilityFcns.cpp \
    weightlossplandialog.cpp \
    weightlossplanmodel.cpp \
    weighttable.cpp

HEADERS += \
    createrecipedialog.h \
    editablesqlmodel.h \
    exercisemodel.h \
    foodsearchproxymodel.h \
    macrosmodel.h \
    mainwindow.h \
    mealproxymodel.h \
    mealsstructure.h \
    mealstruct.h \
    mealsummarymodel.h \
    nutrientsmodel.h \
    recipeeditmodel.h \
    recipetablemodel.h \
    runningdatamodel.h \
    statsplotmodel.h \
    utilityFcns.h \
    weightlossplandialog.h \
    weightlossplanmodel.h \
    weighttable.h

FORMS += \
    createrecipedialog.ui \
    mainwindow.ui \
    weightlossplandialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
