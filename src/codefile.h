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
        Predicate,           /* 1.15+ */
        ItemModifier,        /* 1.17+ */
        Recipe,
        Dimension,           /* 1.16.2+ */
        DimensionType,       /* 1.16.2+ */
        ChatType,            // 1.19.3+
        DamageType,          // 1.19.4+
        TrimMaterial,        // 1.20+
        TrimPattern,         // 1.20+
        WolfVariant,         // 1.20.5+
        Enchantment,         // 1.21+
        EnchantmentProvider, // 1.21+
        JukeboxSong,         // 1.21+
        PaintingVariant,     // 1.21+
        GoatHornInstrument,  // 1.21.2+
        WorldGen,            /* 1.16.2+ */
        Biome,               /* 1.16.2+ */
        ConfiguredCarver,    /* 1.16.2+ */
        ConfiguredFeature,   /* 1.16.2+ */
        SurfaceBuilder,      /* 1.16.2 - 1.17 */
        StructureFeature,    /* 1.16.2 - 1.18.2, renamed to Structure (1.19+) */
        Noise,               // 1.18+
        NoiseSettings,       /* 1.16.2+ */
        PlacedFeature,       // 1.18+
        ProcessorList,       /* 1.16.2+ */
        StructureSet,        // 1.18.2+
        TemplatePool,        /* 1.16.2+ */
        FlatLevelGenPreset,  // 1.19+
        WorldGen_end,
        Tag,
        BlockTag, EntityTypeTag, FluidTag,
        FunctionTag, ItemTag, GameEventTag,
        Tag_end,
        JsonText_end,
        Jmc,
        JmcHeader,
        JmcCert,
        McBuild,
        McBuildMacro,
        TridentCode,
        CbScript,
        Text_end,
    };

    QFileInfo info;
    FileType  fileType   = Unknown;
    bool      isModified = false;

    CodeFile(const QString &path);

    QString name() const;
    QString path() const;
    void changePath(const QString &path);

    static CodeFile::FileType pathToFileType(const QString &dirpath,
                                             const QString &filepath);
    static QIcon fileTypeToIcon(const CodeFile::FileType type);
    static QString fileTypeToName(const CodeFile::FileType type);
};

static_assert(qIsRelocatable<CodeFile>() == false);
Q_DECLARE_TYPEINFO(CodeFile, Q_RELOCATABLE_TYPE);

QDebug operator<<(QDebug debug, const CodeFile &file);

#endif /* CODEFILE_H */
