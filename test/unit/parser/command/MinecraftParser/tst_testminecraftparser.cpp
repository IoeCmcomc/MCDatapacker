#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/minecraftparser.h"

/*#define QBENCHMARK    if (true) */

using namespace Command;

class TestMinecraftParser : public QObject
{
    Q_OBJECT

public:
    TestMinecraftParser();
    ~TestMinecraftParser();

    QElapsedTimer timer;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void dataCmd();
    void executeCmd();
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

    timer.start();
}

void TestMinecraftParser::cleanupTestCase() {
    qDebug() << "Elapsed time in ms:" << timer.elapsed();
}

void TestMinecraftParser::dataCmd() {
    MinecraftParser parser(this,
                           "data get entity @p foo.bar[0].\"A [crazy name]!\".baz");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"foo\").), NbtPathStepNode<Key>(StringNode(\"bar\")), NbtPathStepNode<Index>([IntegerNode(0)].), NbtPathStepNode<Key>(StringNode(\"A [crazy name]!\").), NbtPathStepNode<Key>(StringNode(\"baz\"))))");

    parser.setText("data get entity @p A[-1].B");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"A\")), NbtPathStepNode<Index>([IntegerNode(-1)].), NbtPathStepNode<Key>(StringNode(\"B\"))))");

    parser.setText("data modify storage test {} set value {Air: 299s, ok: true}");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[8](LiteralNode(data), LiteralNode(modify), LiteralNode(storage), ResourceLocationNode(test), NbtPathNode(NbtPathStepNode<Root>({MapNode()})), LiteralNode(set), LiteralNode(value), MapNode(Air: NbtShortNode(299s), ok: NbtByteNode(1b)))");
}

void TestMinecraftParser::executeCmd() {
    MinecraftParser parser(this,
                           "execute as @a[x_rotation=-90..-1,scores={uncraftfly=1..},gamemode=!creative] at @s if block ~ ~-1 ~ air run effect give @s minecraft:levitation 1 1 true");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[17](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a){MultiMapNode(gamemode: (!StringNode(\"creative\")); scores: (MapNode(uncraftfly: IntRangeNode(1..0))); x_rotation: (FloatRangeNode(-90..-1)))}), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~-1), z: AxisNode(~0)), BlockPredicateNodee(minecraft:air), LiteralNode(run), LiteralNode(effect), LiteralNode(give), EntityNode[single](TargetSelectorNode(@s)), MobEffectNode(minecraft:levitation), IntegerNode(1), IntegerNode(1), BoolNode(true))");

    parser.setText("execute as @s run tp @s 12 45 78");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[7](LiteralNode(execute), LiteralNode(as), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(run), LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(12), y: AxisNode(45), z: AxisNode(78)))");

    parser.setText(
        "execute as @e[type = !player, tag = test, type = !villager, tag = second, tag = !bonk] rotated 0 ~90 store success storage mcdatapacker:test executeSuccess byte 1 run say hi");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[15](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(tag: (!StringNode(\"bonk\"), StringNode(\"second\"), StringNode(\"test\")); type: (!ResourceLocationNode(villager), !ResourceLocationNode(player)))}), LiteralNode(rotated), RotationNode(x: AxisNode(0), z: AxisNode(~90)), LiteralNode(store), LiteralNode(success), LiteralNode(storage), ResourceLocationNode(mcdatapacker:test), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"executeSuccess\"))), LiteralNode(byte), DoubleNode(1), LiteralNode(run), LiteralNode(say), MessageNode(\"hi\"))");

    parser.setText(
        "execute if block ~ ~1 ~ #minecraft:chest[a=b]{Lock:\"\"} run say hi");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[8](LiteralNode(execute), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~1), z: AxisNode(~0)), BlockPredicateNodee(minecraft:chest)[MapNode(a: StringNode(\"b\"))]{MapNode(Lock: NbtStringNode())}, LiteralNode(run), LiteralNode(say), MessageNode(\"hi\"))");
}

void TestMinecraftParser::functionCmd() {
    MinecraftParser parser(this, "function custom:example/test");

    QBENCHMARK {
        auto result = parser.parse();
        QVERIFY(result->isVaild());
        QCOMPARE(
            result->toString(),
            "RootNode[2](LiteralNode(function), FunctionNode(custom:example/test))");
    }
}

void TestMinecraftParser::forceloadCmd() {
    MinecraftParser           parser(this, "forceload add 0 -1 31 47");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[4](LiteralNode(forceload), LiteralNode(add), ColumnPosNode(x: AxisNode(0), z: AxisNode(-1)), ColumnPosNode(x: AxisNode(31), z: AxisNode(47)))");

    parser.setText("forceload query 3 5");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(forceload), LiteralNode(query), ColumnPosNode(x: AxisNode(3), z: AxisNode(5)))");
}

void TestMinecraftParser::giveCmd() {
    MinecraftParser parser(this,
                           R"(give @p diamond_sword{ Enchantments:[{id:"minecraft:sharpness",lvl:10}]} 1)");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[4](LiteralNode(give), EntityNode[single, player](TargetSelectorNode(@p)), ItemStackNode(minecraft:diamond_sword){MapNode(Enchantments: NbtListNode(MapNode(id: NbtStringNode(minecraft:sharpness), lvl: NbtIntNode(10))))}, IntegerNode(1))");

    parser.setText("give @a[distance=..5, nbt={Air:300s}] stone");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(give), EntityNode[player](TargetSelectorNode(@a){MultiMapNode(distance: (FloatRangeNode(..5)); nbt: (MapNode(Air: NbtShortNode(300s))))}), ItemStackNode(minecraft:stone))");
}

void TestMinecraftParser::particleCmd() {
    MinecraftParser parser(this,
                           "particle block minecraft:furnace[lit=true] ~ ~ ~ 0 0 0 1 1");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[6](LiteralNode(particle), ParticleNode(minecraft:block, BlockStateNode(minecraft:furnace)[MapNode(lit: StringNode(\"true\"))]), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), Vec3Node(x: AxisNode(0), y: AxisNode(0), z: AxisNode(0)), FloatNode(1), IntegerNode(1))");

    parser.setText("particle dust 0.5 0.2 0.8 1 ~ ~ ~ 0 0 0 1 1");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[6](LiteralNode(particle), ParticleNode(minecraft:dust, ParticleColorNode(FloatNode(0.5), FloatNode(0.2), FloatNode(0.8), FloatNode(1))), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), Vec3Node(x: AxisNode(0), y: AxisNode(0), z: AxisNode(0)), FloatNode(1), IntegerNode(1))");

    parser.setText("particle minecraft:explosion_emitter ~10 ~ ~");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(particle), ParticleNode(minecraft:explosion_emitter), Vec3Node(x: AxisNode(~10), y: AxisNode(~0), z: AxisNode(~0)))");
}

void TestMinecraftParser::scoreboardCmd() {
    MinecraftParser parser(this,
                           "scoreboard objectives add joined dummy");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[5](LiteralNode(scoreboard), LiteralNode(objectives), LiteralNode(add), StringNode(\"joined\"), ObjectiveCriteriaNode(\"dummy\"))");

    parser.setText("scoreboard players enable @a phase");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    qDebug() << result->toString();
    QCOMPARE(
        result->toString().toStdString(),
        "RootNode[5](LiteralNode(scoreboard), LiteralNode(players), LiteralNode(enable), ScoreHolderNodee[player](TargetSelectorNode(@a)), ObjectiveNode(\"phase\"))");

    parser.setText("scoreboard players set a b 0");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[6](LiteralNode(scoreboard), LiteralNode(players), LiteralNode(set), ScoreHolderNodee[single](StringNode(\"a\")), ObjectiveNode(\"b\"), IntegerNode(0))");
}

void TestMinecraftParser::setblockCmd() {
    MinecraftParser parser(this,
                           "setblock ^ ^ ^3 minecraft:hopper[facing=east]{id: 'minecraft:hopper', TransferCooldown: 23}");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(setblock), BlockPosNode(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^3)), BlockStateNode(minecraft:hopper)[MapNode(facing: StringNode(\"east\"))]{MapNode(TransferCooldown: NbtIntNode(23), id: NbtStringNode(minecraft:hopper))})");
}

void TestMinecraftParser::tpCmd() {
    MinecraftParser           parser(this, "tp 100 ~2 300");
    QSharedPointer<ParseNode> result = nullptr;

    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[2](LiteralNode(tp), Vec3Node(x: AxisNode(100), y: AxisNode(~2), z: AxisNode(300)))");

    parser.setText("tp @s ^ ^ ^1.5");
    QBENCHMARK {
        result = parser.parse();
    }
    QVERIFY(result->isVaild());
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^1.5)))");
}

void TestMinecraftParser::timeCmd() {
    MinecraftParser parser(this, "time add 90s");

    QBENCHMARK {
        auto result = parser.parse();
        QVERIFY(result->isVaild());
        QCOMPARE(
            result->toString(),
            "RootNode[3](LiteralNode(time), LiteralNode(add), TimeNode(90s))");
    }
}

QTEST_MAIN(TestMinecraftParser)

#include "tst_testminecraftparser.moc"
