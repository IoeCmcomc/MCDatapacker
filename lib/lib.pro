TEMPLATE = lib

CONFIG += staticlib qt gui c++17 conan_basic_setup

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DISTFILES += \
    QFindDialogs/LICENSE \
    QFindDialogs/src/README.txt

HEADERS += \
    QFindDialogs/src/finddialog.h \
    QFindDialogs/src/findreplacedialog.h \
    json/single_include/nlohmann/json.hpp \
    lru-cache/include/lru/cache-tags.hpp \
    lru-cache/include/lru/cache.hpp \
    lru-cache/include/lru/entry.hpp \
    lru-cache/include/lru/error.hpp \
    lru-cache/include/lru/insertion-result.hpp \
    lru-cache/include/lru/internal/base-cache.hpp \
    lru-cache/include/lru/internal/base-iterator.hpp \
    lru-cache/include/lru/internal/base-ordered-iterator.hpp \
    lru-cache/include/lru/internal/base-unordered-iterator.hpp \
    lru-cache/include/lru/internal/callback-manager.hpp \
    lru-cache/include/lru/internal/definitions.hpp \
    lru-cache/include/lru/internal/hash.hpp \
    lru-cache/include/lru/internal/information.hpp \
    lru-cache/include/lru/internal/last-accessed.hpp \
    lru-cache/include/lru/internal/optional.hpp \
    lru-cache/include/lru/internal/statistics-mutator.hpp \
    lru-cache/include/lru/internal/timed-information.hpp \
    lru-cache/include/lru/internal/utility.hpp \
    lru-cache/include/lru/iterator-tags.hpp \
    lru-cache/include/lru/key-statistics.hpp \
    lru-cache/include/lru/lowercase.hpp \
    lru-cache/include/lru/lru.hpp \
    lru-cache/include/lru/statistics.hpp \
    lru-cache/include/lru/timed-cache.hpp \
    lru-cache/include/lru/wrap.hpp \
    nbt-cpp/include/nbt.hpp \
    nbt-cpp/src/nbt_internal.hpp

SOURCES += \
    QFindDialogs/src/finddialog.cpp \
    QFindDialogs/src/findreplacedialog.cpp \
    nbt-cpp/src/nbt.cpp

INCLUDEPATH += lru-cache/include
INCLUDEPATH += nbt-cpp/include


message($$INCLUDEPATH)
