TEMPLATE = subdirs

SUBDIRS = \
        lib \
        src \
        test

src.depends = lib
test.depends = src

OTHER_FILES += uncrustify.cfg
