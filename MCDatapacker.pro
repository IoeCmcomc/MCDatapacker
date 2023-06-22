TEMPLATE = subdirs

SUBDIRS = \
        lib \
        src \
        test


src.depends = lib
test.depends = src
#test.depends = lib

OTHER_FILES += uncrustify.cfg
