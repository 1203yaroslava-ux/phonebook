CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = PhoneBook
TEMPLATE = app
INCLUDEPATH += $$PWD

SOURCES += main.cpp \
    core/validator.cpp \
    core/phonebook.cpp \
    storage/filestorage.cpp

HEADERS += \
    core/contact.h \
    core/validator.h \
    core/phonebook.h \
    storage/istorage.h \
    storage/filestorage.h
