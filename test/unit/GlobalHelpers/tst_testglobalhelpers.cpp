#include <QtTest>
#include <QCoreApplication>

#include "../../../src/globalhelpers.h"

class TestGlobalHelpers : public QObject
{
    Q_OBJECT

public:
    TestGlobalHelpers();
    ~TestGlobalHelpers();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void strToVariant();
    void variantToStr();
    void removePrefix();
    void isPathRelativeTo();
    void toNamespacedId();
};

TestGlobalHelpers::TestGlobalHelpers() {
}

TestGlobalHelpers::~TestGlobalHelpers() {
}

void TestGlobalHelpers::initTestCase() {
}

void TestGlobalHelpers::cleanupTestCase() {
}

void TestGlobalHelpers::strToVariant() {
    QCOMPARE(Glhp::strToVariant(u"727"), QVariant::fromValue(727));
    QCOMPARE(Glhp::strToVariant(u"true"), QVariant::fromValue(true));
    QCOMPARE(Glhp::strToVariant(u"false"), QVariant::fromValue(false));
    QCOMPARE(Glhp::strToVariant(u"wtf"),
             QVariant::fromValue(QStringLiteral("wtf")));

    QVariant result;
    QBENCHMARK {
        result = Glhp::strToVariant(u"qwertyuiop");
        result = Glhp::strToVariant(u"false");
        result = Glhp::strToVariant(u"727");
        result = Glhp::strToVariant(u"true");
    }
    Q_UNUSED(result)
}

void TestGlobalHelpers::variantToStr() {
    QCOMPARE(Glhp::variantToStr(QVariant::fromValue(727)), "727");
    QCOMPARE(Glhp::variantToStr(QVariant::fromValue(true)), "true");
    QCOMPARE(Glhp::variantToStr(QVariant::fromValue(false)), "false");
    QCOMPARE(Glhp::variantToStr(QVariant::fromValue(QStringLiteral("wtf"))),
             "wtf");

    QString result;
    QBENCHMARK {
        result = Glhp::variantToStr("qwertyuiop");
        result = Glhp::variantToStr(false);
        result = Glhp::variantToStr(727);
        result = Glhp::variantToStr(true);
    }
    Q_UNUSED(result)
}

void TestGlobalHelpers::removePrefix() {
    QString str = "minecraft:stone";
    bool    ok  = Glhp::removePrefix(str, u"minecraft:");

    QVERIFY(ok);
    QCOMPARE(str, "stone");

    str = "minecraft:stone";
    ok  = Glhp::removePrefix(str, "minecraft:"_QL1);
    QVERIFY(ok);
    QCOMPARE(str, "stone");

    str = "armor_stand";
    ok  = Glhp::removePrefix(str, u"minecraft:");
    QVERIFY(!ok);
    QCOMPARE(str, "armor_stand");

    str = "armor_stand";
    ok  = Glhp::removePrefix(str, "minecraft:"_QL1);
    QVERIFY(!ok);
    QCOMPARE(str, "armor_stand");
}

void TestGlobalHelpers::isPathRelativeTo() {
    QCOMPARE(Glhp::isPathRelativeTo("E:/test",
                                    u"E:/test/data/namespace/functions/fun.mcfunction",
                                    u"functions"), true);
    QCOMPARE(Glhp::isPathRelativeTo("E:/test",
                                    u"E:/test/data/namespace/functions/fun.mcfunction",
                                    u"recipes"), false);
    QCOMPARE(Glhp::isPathRelativeTo("E:/test/data",
                                    u"E:/test/data/namespace/functions/fun.mcfunction",
                                    u"functions"), false);
    QCOMPARE(Glhp::isPathRelativeTo("E:/test",
                                    u"E:/test/data/namespace/fun.mcfunction",
                                    u"functions"), false);

    bool          result;
    const QString dirpath  = "E:/test";
    const QString path     = "E:/test/data/namespace/functions/fun.mcfunction";
    const QString category = "functions";
    QBENCHMARK {
        result = Glhp::isPathRelativeTo(dirpath, path, category);
    }
    Q_UNUSED(result)
}

void TestGlobalHelpers::toNamespacedId() {
    QCOMPARE(Glhp::toNamespacedID("E:/test",
                                  u"E:/test/data/namespace/functions/fun.mcfunction"),
             "namespace:fun");
    QCOMPARE(Glhp::toNamespacedID("E:/test",
                                  u"E:/test/data/minecraft/tags/functions/load.json"),
             "#minecraft:load");
    QCOMPARE(Glhp::toNamespacedID("E:/test",
                                  u"E:/test/data/minecraft/tags/functions/load.json",
                                  true),
             "minecraft:load");
    QCOMPARE(Glhp::toNamespacedID("E:/test",
                                  u"E:/test/data/minecraft/worldgen/biome/deep_ocean.json"),
             "minecraft:deep_ocean");
    QCOMPARE(Glhp::toNamespacedID("E:/folder/packs",
                                  u"E:/folder/packs/data/terralith/tags/worldgen/structure/zpointer/minecraft_badlands.json"),
             "#terralith:zpointer/minecraft_badlands");

    QString       result;
    const QString dirpath = "E:/datapacks/test";
    const QString path    =
        "E:/datapacks/test/data/namespace/worldgen/biome/subfolder/deep_ocean.json";
    QBENCHMARK {
        result = Glhp::toNamespacedID(dirpath, path);
    }
    Q_UNUSED(result)
}

QTEST_MAIN(TestGlobalHelpers)

#include "tst_testglobalhelpers.moc"
