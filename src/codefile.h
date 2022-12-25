#ifndef CODEFILE_H
#define CODEFILE_H

#include <QFileInfo>
#include <QDebug>

struct CodeFile {
    enum FileType : int8_t {
        Unknown = -1,
        Binary,
        Structure,
        Image,
        Text,
        Function,
        JsonText,
        Advancement,
        LootTable,
        Meta,
        Predicate,         /* 1.15+ */
        ItemModifier,      /* 1.17+ */
        Recipe,
        Dimension,         /* 1.16.2+ */
        DimensionType,     /* 1.16.2+ */
        WorldGen,          /* 1.16.2+ */
        Biome,             /* 1.16.2+ */
        ConfiguredCarver,  /* 1.16.2+ */
        ConfiguredFeature, /* 1.16.2+ */
        SurfaceBuilder,    /* 1.16.2 - 1.17 */
        StructureFeature,  /* 1.16.2 - 1.18.2, renamed to Structure (1.19+) */
        Noise,             // 1.18+
        NoiseSettings,     /* 1.16.2+ */
        PlacedFeature,     // 1.18+
        ProcessorList,     /* 1.16.2+ */
        StructureSet,      // 1.18.2+
        TemplatePool,      /* 1.16.2+ */
        /*WorldPreset, // 1.19+ */
        WorldGen_end,
        Tag,
        BlockTag, EntityTypeTag, FluidTag,
        FunctionTag, ItemTag, GameEventTag,
        Tag_end,
        JsonText_end,
        Text_end,
    };

    QVariant  data;
    QFileInfo info;
    FileType  fileType   = Unknown;
    bool      isModified = false;

    CodeFile(const QString &path);

    inline bool isValid() const {
        return (!info.filePath().isEmpty()) && data.isValid();
    }
    inline QString name() const {
        return info.fileName();
    }
    inline QString path() const {
        return info.filePath();
    }
    void changePath(const QString &path);
};

QDebug operator<<(QDebug debug, const CodeFile &file);

#endif /* CODEFILE_H */
