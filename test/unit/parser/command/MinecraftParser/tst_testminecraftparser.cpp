#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/minecraftparser.h"

using namespace Command;

class TestMinecraftParser : public QObject
{
    Q_OBJECT

public:
    TestMinecraftParser();
    ~TestMinecraftParser();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void dataCmd();
    void functionCmd();
    void forceloadCmd();
    void giveCmd();
    void particleCmd();
    void scoreboardCmd();
    void setblockCmd();
    void tpCmd();
    void timeCmd();
};

TestMinecraftParser::TestMinecraftParser() {
}

TestMinecraftParser::~TestMinecraftParser() {
}

void TestMinecraftParser::initTestCase() {
    MinecraftParser::setSchema(":/minecraft/info/commands.json");
    for (int i = 1024; i < 2000; ++i) {
        QMetaType metatype(i);
        if (metatype.isRegistered()) {
            qDebug() << i << metatype.name() << metatype.isValid() <<
                metatype.isRegistered();
        } else {
            break;
        }
    }
}

void TestMinecraftParser::cleanupTestCase() {
}

void TestMinecraftParser::dataCmd() {
    MinecraftParser parser(this,
                           "data get entity @p foo.bar[0].\"A [crazy name]!\".baz");

    auto result = parser.parse();

    qDebug() << *result;
    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"foo\").), NbtPathStepNode<Key>(StringNode(\"bar\")), NbtPathStepNode<Index>([IntegerNode(0)].), NbtPathStepNode<Key>(StringNode(\"A [crazy name]!\").), NbtPathStepNode<Key>(StringNode(\"baz\"))))");

    parser.setText("data get entity @p A[-1].B");
    result = parser.parse();

    qDebug() << *result;
    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"A\")), NbtPathStepNode<Index>([IntegerNode(-1)].), NbtPathStepNode<Key>(StringNode(\"B\"))))");

    parser.setText("data modify storage test {} set value {Key:1b}");
    result = parser.parse();

    qDebug() << *result;
}

void TestMinecraftParser::functionCmd() {
    MinecraftParser parser(this, "function custom:example/test");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[2](LiteralNode(function), FunctionNode(custom:example/test))");
}

void TestMinecraftParser::forceloadCmd() {
    MinecraftParser parser(this, "forceload add 0 -1 31 47");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[4](LiteralNode(forceload), LiteralNode(add), ColumnPosNode(x: AxisNode(0), z: AxisNode(-1)), ColumnPosNode(x: AxisNode(31), z: AxisNode(47)))");

    parser.setText("forceload query 3 5");

    result = parser.parse();
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(forceload), LiteralNode(query), ColumnPosNode(x: AxisNode(3), z: AxisNode(5)))");
}

void TestMinecraftParser::giveCmd() {
    MinecraftParser parser(this,
                           R"(give @p diamond_sword{Enchantments:[{id:"minecraft:sharpness",lvl:10}]} 1)");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[4](LiteralNode(give), EntityNode[single, player](TargetSelectorNode(@p)), ItemStackNode(minecraft:diamond_sword){MapNode(Enchantments: NbtListNode(MapNode(id: NbtStringNode(minecraft:sharpness), lvl: NbtIntNode(10))))}, IntegerNode(1))");

    parser.setText("give @a[distance=..5, nbt={Air:300s}] stone");
    result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(give), EntityNode[player](TargetSelectorNode(@a){MapNode(distance: FloatRangeNode(..5), nbt: MapNode(Air: NbtShortNode(300s)))}), ItemStackNode(minecraft:stone))");
}

void TestMinecraftParser::particleCmd() {
    MinecraftParser parser(this,
                           "particle block minecraft:furnace[lit=true] ~ ~ ~ 0 0 0 1 1");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[6](LiteralNode(particle), ParticleNode(minecraft:block, BlockStateNode(minecraft:furnace)[MapNode(lit: StringNode(\"true\"))]), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), Vec3Node(x: AxisNode(0), y: AxisNode(0), z: AxisNode(0)), FloatNode(1), IntegerNode(1))");

    parser.setText("particle dust 0.5 0.2 0.8 1 ~ ~ ~ 0 0 0 1 1");
    result = parser.parse();

    qDebug() << *result;

    parser.setText("particle minecraft:explosion_emitter ~10 ~ ~");
    result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(particle), ParticleNode(minecraft:explosion_emitter), Vec3Node(x: AxisNode(~10), y: AxisNode(~0), z: AxisNode(~0)))");
}

void TestMinecraftParser::scoreboardCmd() {
    MinecraftParser parser(this, "scoreboard objectives add joined dummy");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(scoreboard), LiteralNode(objectives), LiteralNode(add), StringNode(\"joined\"), ObjectiveCriteriaNode(\"dummy\"))");

    parser.setText("scoreboard players enable @a phase");
    result = parser.parse();

    qDebug() << result->toString();
}

void TestMinecraftParser::setblockCmd() {
    MinecraftParser parser(this,
                           "setblock ^ ^ ^3 minecraft:hopper[facing=east]{id: 'minecraft:hopper', TransferCooldown: 23}");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(setblock), BlockPosNode(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^3)), BlockStateNode(minecraft:hopper)[MapNode(facing: StringNode(\"east\"))]{MapNode(TransferCooldown: NbtIntNode(23), id: NbtStringNode(minecraft:hopper))})");
}

void TestMinecraftParser::tpCmd() {
    /*QSKIP("Not completed yet"); */

    MinecraftParser parser(this, "tp 100 ~2 300");

    auto result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[2](LiteralNode(tp), Vec3Node(x: AxisNode(100), y: AxisNode(~2), z: AxisNode(300)))");
}

void TestMinecraftParser::timeCmd() {
    MinecraftParser parser(this, "time add 90s");

    auto result = parser.parse();

    QCOMPARE(result->toString(),
             "RootNode[3](LiteralNode(time), LiteralNode(add), TimeNode(90s))");
}

QTEST_MAIN(TestMinecraftParser)

#include "tst_testminecraftparser.moc"
