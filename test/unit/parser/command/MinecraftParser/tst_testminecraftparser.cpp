#include <QtTest>
#include <QCoreApplication>

#include "../../../../../src/parsers/command/minecraftparser.h"

/*#define QBENCHMARK    if (true) */

#define DECLARE_TAG_VARS    int i = 0; int c = 0; QString cat;
#define SET_TAG(x)    i           = 0; cat =#x;
#define GEN_TAG             genTag(cat, i, c)

using namespace Command;

auto genTag(const QString &cat, int &i, int &c) {
    static const QString templStr("%1_%2");

    ++c;
    return templStr.arg(cat).arg(++i).toLatin1();
}

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
    void genRepr_data();
    void genRepr();
    void commands_data();
    void commands();
    void benchmark_data();
    void benchmark();
    void benchmarkCommandBoxes_data();
    void benchmarkCommandBoxes();
};

TestMinecraftParser::TestMinecraftParser() {
}

TestMinecraftParser::~TestMinecraftParser() {
}

void TestMinecraftParser::initTestCase() {
    Command::MinecraftParser::setSchema(
        ":/minecraft/1.17/mcdata/processed/reports/commands/data.min.json");
    Command::MinecraftParser::setTestMode(true);
    /*
       for (int i = 1024; i < 2000; ++i) {
        QMetaType metatype(i);
        if (metatype.isRegistered()) {
            qDebug() << i << metatype.name() << metatype.isValid() <<
                metatype.isRegistered();
        } else {
            break;
        }
       }
     */

    timer.start();
    /*QThread::sleep(5); */
}

void TestMinecraftParser::cleanupTestCase() {
    qDebug() << "Total elapsed time in ms:" << timer.nsecsElapsed() / 1e6;
}

void TestMinecraftParser::genRepr_data() {
    QSKIP("");
    /*DECLARE_TAG_VARS; */

    QTest::addColumn<QString>("command");
}

void TestMinecraftParser::genRepr() {
    QFETCH(QString, command);
    MinecraftParser parser(this, command);
    const auto      result = parser.parse();

    QVERIFY(result->isVaild());
    qDebug() << result->toString();
}

void TestMinecraftParser::commands_data() {
    DECLARE_TAG_VARS

    QTest::addColumn<QString>("command");

    QTest::addColumn<QString>("parseTreeRepr");

    /* Some commands copied from the Minecraft Wiki
     * used to contain the character 'ZERO WIDTH NON-JOINER' (U+200C),
     * which caused the code formatter to panic
     * and caused the MinecraftParser to produce errors.
     *
     * These characters are removed in this file.
     */

    QElapsedTimer localTimer;
    localTimer.start();

    SET_TAG(advancement)
    QTest::newRow(GEN_TAG) <<
        "advancement grant @s only minecraft:story/shiny_gear" <<
        "RootNode[5](LiteralNode(advancement), LiteralNode(grant), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(only), ResourceLocationNode(minecraft:story/shiny_gear))";
    QTest::newRow(GEN_TAG) << "advancement grant @a everything" <<
        "RootNode[4](LiteralNode(advancement), LiteralNode(grant), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(everything))";

    SET_TAG(attribute)
    QTest::newRow(GEN_TAG) <<
        "attribute @s minecraft:generic.armor base set 5" <<
        "RootNode[6](LiteralNode(attribute), EntityNode[single](TargetSelectorNode(@s)), ResourceLocationNode(minecraft:generic.armor), LiteralNode(base), LiteralNode(set), DoubleNode(5))";

    SET_TAG(clear)
    QTest::newRow(GEN_TAG) << "clear" << "RootNode[1](LiteralNode(clear))";
    QTest::newRow(GEN_TAG) << "clear Alice" <<
        "RootNode[2](LiteralNode(clear), EntityNode[single](StringNode(\"Alice\")))";
    QTest::newRow(GEN_TAG) << "clear Alice minecraft:wool" <<
        "RootNode[3](LiteralNode(clear), EntityNode[single](StringNode(\"Alice\")), ItemPredicateNode(minecraft:wool))";
    QTest::newRow(GEN_TAG) << "clear @a minecraft:orange_wool" <<
        "RootNode[3](LiteralNode(clear), EntityNode[player](TargetSelectorNode(@a)), ItemPredicateNode(minecraft:orange_wool))";
    QTest::newRow(GEN_TAG) << "clear @r minecraft:stone 0" <<
        "RootNode[4](LiteralNode(clear), EntityNode[single, player](TargetSelectorNode(@r)), ItemPredicateNode(minecraft:stone), IntegerNode(0))";
    QTest::newRow(GEN_TAG) <<
        R"(clear @p minecraft:golden_sword{Enchantments:[{id:"minecraft:sharpness",lvl:1s}]})"
                           <<
        "RootNode[3](LiteralNode(clear), EntityNode[single, player](TargetSelectorNode(@p)), ItemPredicateNode(minecraft:golden_sword))";

    SET_TAG(data)
    QTest::newRow(GEN_TAG) <<
        "data get entity @p foo.bar[0].\"A [crazy name]!\".baz" <<
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"foo\").), NbtPathStepNode<Key>(StringNode(\"bar\")), NbtPathStepNode<Index>([IntegerNode(0)].), NbtPathStepNode<Key>(StringNode(\"A [crazy name]!\").), NbtPathStepNode<Key>(StringNode(\"baz\"))))";
    QTest::newRow(GEN_TAG) <<
        "data get entity @p A[-1].B" <<
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"A\")), NbtPathStepNode<Index>([IntegerNode(-1)].), NbtPathStepNode<Key>(StringNode(\"B\"))))";
    QTest::newRow(GEN_TAG) <<
        "data modify storage test {} set value {Air: 299s, ok: true}" <<
        "RootNode[8](LiteralNode(data), LiteralNode(modify), LiteralNode(storage), ResourceLocationNode(test), NbtPathNode(NbtPathStepNode<Root>({NbtCompoundNode()})), LiteralNode(set), LiteralNode(value), NbtCompoundNode(Air: NbtShortNode(299s), ok: NbtByteNode(1b)))";
    QTest::newRow(GEN_TAG) << "data get entity @s foodSaturationLevel" <<
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single](TargetSelectorNode(@s)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"foodSaturationLevel\"))))";
    QTest::newRow(GEN_TAG) <<
        "data modify entity @e[type=item,distance=..10,limit=1,sort=nearest] PickupDelay set value -1"
                           <<
        "RootNode[8](LiteralNode(data), LiteralNode(modify), LiteralNode(entity), EntityNode[](TargetSelectorNode(@e){MultiMapNode(distance: (FloatRangeNode(..10)); limit: (IntegerNode(1)); sort: (StringNode(\"nearest\")); type: (ResourceLocationNode(item)))}), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"PickupDelay\"))), LiteralNode(set), LiteralNode(value), NbtIntNode(-1))";
    QTest::newRow(GEN_TAG) <<
        "data get entity @e[type=item,limit=1,sort=random] Pos[1]" <<
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[](TargetSelectorNode(@e){MultiMapNode(limit: (IntegerNode(1)); sort: (StringNode(\"random\")); type: (ResourceLocationNode(item)))}), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"Pos\")), NbtPathStepNode<Index>([IntegerNode(1)])))";
    QTest::newRow(GEN_TAG) << "data get entity @p Inventory[{Slot:0b}].id" <<
        "RootNode[5](LiteralNode(data), LiteralNode(get), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"Inventory\")), NbtPathStepNode<Index>([{NbtCompoundNode(Slot: NbtByteNode(0b))}].), NbtPathStepNode<Key>(StringNode(\"id\"))))";
    QTest::newRow(GEN_TAG) <<
        R"(data modify entity @e[x=0,y=64,z=0,type=dolphin,limit=1] Attributes[{Name:"generic.armor"}].Base set value 20)"
                           <<
        "RootNode[8](LiteralNode(data), LiteralNode(modify), LiteralNode(entity), EntityNode[](TargetSelectorNode(@e){MultiMapNode(limit: (IntegerNode(1)); type: (ResourceLocationNode(dolphin)); x: (DoubleNode(0)); y: (DoubleNode(64)); z: (DoubleNode(0)))}), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"Attributes\")), NbtPathStepNode<Index>([{NbtCompoundNode(Name: NbtStringNode(generic.armor))}].), NbtPathStepNode<Key>(StringNode(\"Base\"))), LiteralNode(set), LiteralNode(value), NbtIntNode(20))";
    QTest::newRow(GEN_TAG) <<
        R"(data modify block 1 64 1 Items[0].id set value "minecraft:diamond_block")"
                           <<
        "RootNode[8](LiteralNode(data), LiteralNode(modify), LiteralNode(block), BlockPosNode(x: AxisNode(1), y: AxisNode(64), z: AxisNode(1)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"Items\")), NbtPathStepNode<Index>([IntegerNode(0)].), NbtPathStepNode<Key>(StringNode(\"id\"))), LiteralNode(set), LiteralNode(value), NbtStringNode(minecraft:diamond_block))";
    QTest::newRow(GEN_TAG) <<
        "data merge entity @e[type=zombie,limit=1,sort=nearest] {HandDropChances: [0f, 0.8f]}"
                           <<
        "RootNode[5](LiteralNode(data), LiteralNode(merge), LiteralNode(entity), EntityNode[](TargetSelectorNode(@e){MultiMapNode(limit: (IntegerNode(1)); sort: (StringNode(\"nearest\")); type: (ResourceLocationNode(zombie)))}), NbtCompoundNode(HandDropChances: NbtListNode(NbtFloatNode(0f), NbtFloatNode(0.8f))))";

    SET_TAG(difficulty)
    QTest::newRow(GEN_TAG) << "difficulty hard" <<
        "RootNode[2](LiteralNode(difficulty), LiteralNode(hard))";

    SET_TAG(effect)
    QTest::newRow(GEN_TAG) <<
        "effect give @s minecraft:resistance 1000000 4 true" <<
        "RootNode[7](LiteralNode(effect), LiteralNode(give), EntityNode[single](TargetSelectorNode(@s)), MobEffectNode(minecraft:resistance), IntegerNode(1000000), IntegerNode(4), BoolNode(true))";
    QTest::newRow(GEN_TAG) << "effect give @p minecraft:speed 60 2" <<
        "RootNode[6](LiteralNode(effect), LiteralNode(give), EntityNode[single, player](TargetSelectorNode(@p)), MobEffectNode(minecraft:speed), IntegerNode(60), IntegerNode(2))";
    QTest::newRow(GEN_TAG) << "effect clear @a minecraft:haste" <<
        "RootNode[4](LiteralNode(effect), LiteralNode(clear), EntityNode[player](TargetSelectorNode(@a)), MobEffectNode(minecraft:haste))";
    QTest::newRow(GEN_TAG) << "effect clear @e[type=zombie]" <<
        "RootNode[3](LiteralNode(effect), LiteralNode(clear), EntityNode[](TargetSelectorNode(@e){MultiMapNode(type: (ResourceLocationNode(zombie)))}))";

    SET_TAG(enchant)
    QTest::newRow(GEN_TAG) << "enchant @a infinity" <<
        "RootNode[3](LiteralNode(enchant), EntityNode[player](TargetSelectorNode(@a)), ItemEnchantmentNode(infinity))";
    QTest::newRow(GEN_TAG) << "enchant @p sharpness 5" <<
        "RootNode[4](LiteralNode(enchant), EntityNode[single, player](TargetSelectorNode(@p)), ItemEnchantmentNode(sharpness), IntegerNode(5))";

    SET_TAG(execute)
    QTest::newRow(GEN_TAG) <<
        "execute as @a[x_rotation=-90..-1,scores={uncraftfly=1..},gamemode=!creative] at @s if block ~ ~-1 ~ air run effect give @s minecraft:levitation 1 1 true"
                           <<
        "RootNode[17](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a){MultiMapNode(gamemode: (!StringNode(\"creative\")); scores: (MapNode(uncraftfly: IntRangeNode(1..0))); x_rotation: (FloatRangeNode(-90..-1)))}), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~-1), z: AxisNode(~0)), BlockPredicateNode(minecraft:air), LiteralNode(run), LiteralNode(effect), LiteralNode(give), EntityNode[single](TargetSelectorNode(@s)), MobEffectNode(minecraft:levitation), IntegerNode(1), IntegerNode(1), BoolNode(true))";
    QTest::newRow(GEN_TAG) <<
        "execute as @s run tp @s 12 45 78" <<
        "RootNode[7](LiteralNode(execute), LiteralNode(as), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(run), LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(12), y: AxisNode(45), z: AxisNode(78)))";
    QTest::newRow(GEN_TAG) <<
        "execute as @e[type = !player, tag = test, type = !villager, tag = second, tag = !stonks] rotated 0 ~90 store success storage mcdatapacker:test executeSuccess byte 1 run say hi"
                           <<
        "RootNode[15](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(tag: (!StringNode(\"stonks\"), StringNode(\"second\"), StringNode(\"test\")); type: (!ResourceLocationNode(villager), !ResourceLocationNode(player)))}), LiteralNode(rotated), RotationNode(x: AxisNode(0), z: AxisNode(~90)), LiteralNode(store), LiteralNode(success), LiteralNode(storage), ResourceLocationNode(mcdatapacker:test), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"executeSuccess\"))), LiteralNode(byte), DoubleNode(1), LiteralNode(run), LiteralNode(say), MessageNode(\"hi\"))";
    QTest::newRow(GEN_TAG) <<
        "execute if block ~ ~1 ~ #minecraft:chest[a=b]{Lock:\"\"} run say hi" <<
        "RootNode[8](LiteralNode(execute), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~1), z: AxisNode(~0)), BlockPredicateNode(minecraft:chest)[MapNode(a: StringNode(\"b\"))]{NbtCompoundNode(Lock: NbtStringNode())}, LiteralNode(run), LiteralNode(say), MessageNode(\"hi\"))";
    QTest::newRow(GEN_TAG) << "execute as @e at @s run tp ^ ^ ^1" <<
        "RootNode[8](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e)), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(run), LiteralNode(tp), Vec3Node(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^1)))";
    QTest::newRow(GEN_TAG) << "execute align yxz run spawnpoint @p ~ ~ ~" <<
        "RootNode[7](LiteralNode(execute), LiteralNode(align), SwizzleNode(xyz), LiteralNode(run), LiteralNode(spawnpoint), EntityNode[single, player](TargetSelectorNode(@p)), BlockPosNode(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)))";
    QTest::newRow(GEN_TAG) << "execute anchored eyes run tp ^5 ^ ^" <<
        "RootNode[6](LiteralNode(execute), LiteralNode(anchored), EntityAnchorNode(\"eyes\"), LiteralNode(run), LiteralNode(tp), Vec3Node(x: AxisNode(^5), y: AxisNode(^0), z: AxisNode(^0)))";
    QTest::newRow(GEN_TAG) <<
        "execute as @e[type=villager] run data merge entity @s {Invulnerable:1}"
                           <<
        "RootNode[9](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(type: (ResourceLocationNode(villager)))}), LiteralNode(run), LiteralNode(data), LiteralNode(merge), LiteralNode(entity), EntityNode[single](TargetSelectorNode(@s)), NbtCompoundNode(Invulnerable: NbtIntNode(1)))";
    QTest::newRow(GEN_TAG) << "execute at @e[type=sheep] run kill @s" <<
        "RootNode[6](LiteralNode(execute), LiteralNode(at), EntityNode[](TargetSelectorNode(@e){MultiMapNode(type: (ResourceLocationNode(sheep)))}), LiteralNode(run), LiteralNode(kill), EntityNode[single](TargetSelectorNode(@s)))";
    QTest::newRow(GEN_TAG) <<
        "execute as @e[type=!player] at @s facing entity @p feet run tp @s ^ ^ ^1"
                           <<
        "RootNode[13](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(type: (!ResourceLocationNode(player)))}), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(facing), LiteralNode(entity), EntityNode[single, player](TargetSelectorNode(@p)), EntityAnchorNode(\"feet\"), LiteralNode(run), LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^1)))";
    QTest::newRow(GEN_TAG) <<
        "execute in minecraft:the_nether run tp ~ ~ ~" <<
        "RootNode[6](LiteralNode(execute), LiteralNode(in), DimensionNode(minecraft:the_nether), LiteralNode(run), LiteralNode(tp), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)))";
    QTest::newRow(GEN_TAG) <<
        "execute in minecraft:the_nether positioned as @s run tp ~ ~ ~" <<
        "RootNode[9](LiteralNode(execute), LiteralNode(in), DimensionNode(minecraft:the_nether), LiteralNode(positioned), LiteralNode(as), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(run), LiteralNode(tp), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)))";
    QTest::newRow(GEN_TAG) << "execute positioned 0 64 0 run locate village" <<
        "RootNode[6](LiteralNode(execute), LiteralNode(positioned), Vec3Node(x: AxisNode(0), y: AxisNode(64), z: AxisNode(0)), LiteralNode(run), LiteralNode(locate), LiteralNode(village))";
    QTest::newRow(GEN_TAG) <<
        "execute as @e[type=sheep] positioned as @s rotated as @p run tp @s ^ ^ ^1"
                           <<
        "RootNode[13](LiteralNode(execute), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(type: (ResourceLocationNode(sheep)))}), LiteralNode(positioned), LiteralNode(as), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(rotated), LiteralNode(as), EntityNode[single, player](TargetSelectorNode(@p)), LiteralNode(run), LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^1)))";
    QTest::newRow(GEN_TAG) <<
        "execute as @a at @s if block ~ ~-1 ~ #wool run kill @s" <<
        "RootNode[12](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~-1), z: AxisNode(~0)), BlockPredicateNode(minecraft:wool), LiteralNode(run), LiteralNode(kill), EntityNode[single](TargetSelectorNode(@s)))";
    QTest::newRow(GEN_TAG) <<
        R"(execute as @a if data entity @s Inventory[{Slot:0b}].tag.Enchantments[{id:"minecraft:efficiency"}] run tp @s 0 64 0)"
                           <<
        "RootNode[12](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(if), LiteralNode(data), LiteralNode(entity), EntityNode[single](TargetSelectorNode(@s)), NbtPathNode(NbtPathStepNode<Key>(StringNode(\"Inventory\")), NbtPathStepNode<Index>([{NbtCompoundNode(Slot: NbtByteNode(0b))}].), NbtPathStepNode<Key>(StringNode(\"tag\").), NbtPathStepNode<Key>(StringNode(\"Enchantments\")), NbtPathStepNode<Index>([{NbtCompoundNode(id: NbtStringNode(minecraft:efficiency))}])), LiteralNode(run), LiteralNode(tp), EntityNode[single](TargetSelectorNode(@s)), Vec3Node(x: AxisNode(0), y: AxisNode(64), z: AxisNode(0)))";
    QTest::newRow(GEN_TAG) <<
        "execute as @a at @s anchored eyes run particle smoke ^ ^ ^3" <<
        "RootNode[11](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(anchored), EntityAnchorNode(\"eyes\"), LiteralNode(run), LiteralNode(particle), ParticleNode(minecraft:smoke), Vec3Node(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^3)))";
    QTest::newRow(GEN_TAG) <<
        R"(execute as @a at @s if block ~ ~ ~ water run say "My feet are soaked!")"
                           <<
        "RootNode[12](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(at), EntityNode[single](TargetSelectorNode(@s)), LiteralNode(if), LiteralNode(block), BlockPosNode(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), BlockPredicateNode(minecraft:water), LiteralNode(run), LiteralNode(say), MessageNode(\"\"My feet are soaked!\"\"))";
    QTest::newRow(GEN_TAG) <<
        R"(execute as @a unless score @s test = @s test run say "Score is reset")"
                           <<
        "RootNode[13](LiteralNode(execute), LiteralNode(as), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(unless), LiteralNode(score), ScoreHolderNode[single](TargetSelectorNode(@s)), ObjectiveNode(\"test\"), LiteralNode(=), ScoreHolderNode[single](TargetSelectorNode(@s)), ObjectiveNode(\"test\"), LiteralNode(run), LiteralNode(say), MessageNode(\"\"Score is reset\"\"))";
    QTest::newRow(GEN_TAG) <<
        "execute at @p as @e[type=pig,distance=..3] run data merge entity @s {Motion:[0.0,2.0,0.0]}"
                           <<
        "RootNode[11](LiteralNode(execute), LiteralNode(at), EntityNode[single, player](TargetSelectorNode(@p)), LiteralNode(as), EntityNode[](TargetSelectorNode(@e){MultiMapNode(distance: (FloatRangeNode(..3)); type: (ResourceLocationNode(pig)))}), LiteralNode(run), LiteralNode(data), LiteralNode(merge), LiteralNode(entity), EntityNode[single](TargetSelectorNode(@s)), NbtCompoundNode(Motion: NbtListNode(NbtDoubleNode(0), NbtDoubleNode(2), NbtDoubleNode(0))))";
    QTest::newRow(GEN_TAG) << "execute align yxz run spawnpoint @p ~ ~ ~" <<
        "RootNode[7](LiteralNode(execute), LiteralNode(align), SwizzleNode(xyz), LiteralNode(run), LiteralNode(spawnpoint), EntityNode[single, player](TargetSelectorNode(@p)), BlockPosNode(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)))";

    SET_TAG(experience)
    QTest::newRow(GEN_TAG) << "experience query Steve levels" <<
        "RootNode[4](LiteralNode(experience), LiteralNode(query), EntityNode[single](StringNode(\"Steve\")), LiteralNode(levels))";
    QTest::newRow(GEN_TAG) << "experience add @s 7" <<
        "RootNode[4](LiteralNode(experience), LiteralNode(add), EntityNode[single](TargetSelectorNode(@s)), IntegerNode(7))";

    SET_TAG(fill)
    QTest::newRow(GEN_TAG) <<
        "fill 52 63 -1516 33 73 -1536 minecraft:gold_block replace minecraft:orange_glazed_terracotta"
                           <<
        "RootNode[6](LiteralNode(fill), BlockPosNode(x: AxisNode(52), y: AxisNode(63), z: AxisNode(-1516)), BlockPosNode(x: AxisNode(33), y: AxisNode(73), z: AxisNode(-1536)), BlockStateNode(minecraft:gold_block), LiteralNode(replace), BlockPredicateNode(minecraft:orange_glazed_terracotta))";
    QTest::newRow(GEN_TAG) << "fill ~-3 ~-3 ~-3 ~3 ~-1 ~3 water" <<
        "RootNode[4](LiteralNode(fill), BlockPosNode(x: AxisNode(~-3), y: AxisNode(~-3), z: AxisNode(~-3)), BlockPosNode(x: AxisNode(~3), y: AxisNode(~-1), z: AxisNode(~3)), BlockStateNode(minecraft:water))";
    QTest::newRow(GEN_TAG) <<
        "fill ~-3 ~ ~-4 ~3 ~4 ~4 minecraft:stone hollow" <<
        "RootNode[5](LiteralNode(fill), BlockPosNode(x: AxisNode(~-3), y: AxisNode(~0), z: AxisNode(~-4)), BlockPosNode(x: AxisNode(~3), y: AxisNode(~4), z: AxisNode(~4)), BlockStateNode(minecraft:stone), LiteralNode(hollow))";
    QTest::newRow(GEN_TAG) << "fill ~-15 ~-15 ~-15 ~15 ~15 ~15 stone" <<
        "RootNode[4](LiteralNode(fill), BlockPosNode(x: AxisNode(~-15), y: AxisNode(~-15), z: AxisNode(~-15)), BlockPosNode(x: AxisNode(~15), y: AxisNode(~15), z: AxisNode(~15)), BlockStateNode(minecraft:stone))";
    QTest::newRow(GEN_TAG) <<
        "fill ~-1 ~ ~ ~1 ~ ~ minecraft:prismarine_brick_stairs[facing=south,waterlogged=true]"
                           <<
        "RootNode[4](LiteralNode(fill), BlockPosNode(x: AxisNode(~-1), y: AxisNode(~0), z: AxisNode(~0)), BlockPosNode(x: AxisNode(~1), y: AxisNode(~0), z: AxisNode(~0)), BlockStateNode(minecraft:prismarine_brick_stairs)[MapNode(facing: StringNode(\"south\"), waterlogged: StringNode(\"true\"))])";

    SET_TAG(forceload)
    QTest::newRow(GEN_TAG) << "forceload add 0 -1 31 47" <<
        "RootNode[4](LiteralNode(forceload), LiteralNode(add), ColumnPosNode(x: AxisNode(0), z: AxisNode(-1)), ColumnPosNode(x: AxisNode(31), z: AxisNode(47)))";
    QTest::newRow(GEN_TAG) << "forceload query 3 5" <<
        "RootNode[3](LiteralNode(forceload), LiteralNode(query), ColumnPosNode(x: AxisNode(3), z: AxisNode(5)))";

    SET_TAG(function)
    QTest::newRow(GEN_TAG) << "function custom:example/test" <<
        "RootNode[2](LiteralNode(function), FunctionNode(custom:example/test))";
    QTest::newRow(GEN_TAG) << "function #custom:example/test" <<
        "RootNode[2](LiteralNode(function), FunctionNode(#custom:example/test))";

    SET_TAG(gamemode)
    QTest::newRow(GEN_TAG) << "gamemode creative" <<
        "RootNode[2](LiteralNode(gamemode), LiteralNode(creative))";
    QTest::newRow(GEN_TAG) << "gamemode survival @a" <<
        "RootNode[3](LiteralNode(gamemode), LiteralNode(survival), EntityNode[player](TargetSelectorNode(@a)))";

    SET_TAG(gamerule)
    QTest::newRow(GEN_TAG) << "gamerule doDaylightCycle false" <<
        "RootNode[3](LiteralNode(gamerule), LiteralNode(doDaylightCycle), BoolNode(false))";
    QTest::newRow(GEN_TAG) << "gamerule keepInventory true" <<
        "RootNode[3](LiteralNode(gamerule), LiteralNode(keepInventory), BoolNode(true))";
    QTest::newRow(GEN_TAG) << "gamerule randomTickSpeed 100" <<
        "RootNode[3](LiteralNode(gamerule), LiteralNode(randomTickSpeed), IntegerNode(100))";
    QTest::newRow(GEN_TAG) << "gamerule doInsomnia" <<
        "RootNode[2](LiteralNode(gamerule), LiteralNode(doInsomnia))";

    SET_TAG(give)
    QTest::newRow(GEN_TAG) <<
        R"(give @p diamond_sword{ Enchantments:[{id:"minecraft:sharpness",lvl:10}]} 1)"
                           <<
        "RootNode[4](LiteralNode(give), EntityNode[single, player](TargetSelectorNode(@p)), ItemStackNode(minecraft:diamond_sword){NbtCompoundNode(Enchantments: NbtListNode(NbtCompoundNode(id: NbtStringNode(minecraft:sharpness), lvl: NbtIntNode(10))))}, IntegerNode(1))";
    QTest::newRow(GEN_TAG) << "give @a[distance=..5, nbt={Air:300s}] stone" <<
        "RootNode[3](LiteralNode(give), EntityNode[player](TargetSelectorNode(@a){MultiMapNode(distance: (FloatRangeNode(..5)); nbt: (NbtCompoundNode(Air: NbtShortNode(300s))))}), ItemStackNode(minecraft:stone))";

    SET_TAG(locate)
    QTest::newRow(GEN_TAG) << "locate mansion" <<
        "RootNode[2](LiteralNode(locate), LiteralNode(mansion))";

    SET_TAG(locatebiome)
    QTest::newRow(GEN_TAG) << "locatebiome desert" <<
        "RootNode[2](LiteralNode(locatebiome), ResourceLocationNode(desert))";

    SET_TAG(particle)
    QTest::newRow(GEN_TAG) <<
        "particle block minecraft:furnace[lit=true] ~ ~ ~ 0 0 0 1 1" <<
        "RootNode[6](LiteralNode(particle), ParticleNode(minecraft:block, BlockStateNode(minecraft:furnace)[MapNode(lit: StringNode(\"true\"))]), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), Vec3Node(x: AxisNode(0), y: AxisNode(0), z: AxisNode(0)), FloatNode(1), IntegerNode(1))";
    QTest::newRow(GEN_TAG) << "particle dust 0.5 0.2 0.8 1 ~ ~ ~ 0 0 0 1 1" <<
        "RootNode[6](LiteralNode(particle), ParticleNode(minecraft:dust, ParticleColorNode(FloatNode(0.5), FloatNode(0.2), FloatNode(0.8), FloatNode(1))), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), Vec3Node(x: AxisNode(0), y: AxisNode(0), z: AxisNode(0)), FloatNode(1), IntegerNode(1))";
    QTest::newRow(GEN_TAG) << "particle minecraft:explosion_emitter ~10 ~ ~" <<
        "RootNode[3](LiteralNode(particle), ParticleNode(minecraft:explosion_emitter), Vec3Node(x: AxisNode(~10), y: AxisNode(~0), z: AxisNode(~0)))";
    QTest::newRow(GEN_TAG) << "particle minecraft:explosion_emitter ~10 ~ ~" <<
        "RootNode[3](LiteralNode(particle), ParticleNode(minecraft:explosion_emitter), Vec3Node(x: AxisNode(~10), y: AxisNode(~0), z: AxisNode(~0)))";

    SET_TAG(playsound)
    QTest::newRow(GEN_TAG) <<
        "playsound minecraft:entity.wither.ambient voice DigMinecraft" <<
        "RootNode[4](LiteralNode(playsound), ResourceLocationNode(minecraft:entity.wither.ambient), LiteralNode(voice), EntityNode[single](StringNode(\"DigMinecraft\")))";
    QTest::newRow(GEN_TAG) << "playsound minecraft:music.creative music @p" <<
        "RootNode[4](LiteralNode(playsound), ResourceLocationNode(minecraft:music.creative), LiteralNode(music), EntityNode[single, player](TargetSelectorNode(@p)))";

    SET_TAG(scoreboard)
    QTest::newRow(GEN_TAG) << "scoreboard objectives add joined dummy" <<
        "RootNode[5](LiteralNode(scoreboard), LiteralNode(objectives), LiteralNode(add), StringNode(\"joined\"), ObjectiveCriteriaNode(\"dummy\"))";
    QTest::newRow(GEN_TAG) << "scoreboard players enable @a phase" <<
        "RootNode[5](LiteralNode(scoreboard), LiteralNode(players), LiteralNode(enable), ScoreHolderNode[player](TargetSelectorNode(@a)), ObjectiveNode(\"phase\"))";
    QTest::newRow(GEN_TAG) << "scoreboard players set a b 0" <<
        "RootNode[6](LiteralNode(scoreboard), LiteralNode(players), LiteralNode(set), ScoreHolderNode[single](StringNode(\"a\")), ObjectiveNode(\"b\"), IntegerNode(0))";

    SET_TAG(setblock)
    QTest::newRow(GEN_TAG) <<
        "setblock ^ ^ ^3 minecraft:hopper[facing=east]{id: 'minecraft:hopper', TransferCooldown: 23}"
                           <<
        "RootNode[3](LiteralNode(setblock), BlockPosNode(x: AxisNode(^0), y: AxisNode(^0), z: AxisNode(^3)), BlockStateNode(minecraft:hopper)[MapNode(facing: StringNode(\"east\"))]{NbtCompoundNode(TransferCooldown: NbtIntNode(23), id: NbtStringNode(minecraft:hopper))})";

    SET_TAG(spreadplayers)
    QTest::newRow(GEN_TAG) << "spreadplayers 0 0 200 500 true @a" <<
        "RootNode[6](LiteralNode(spreadplayers), Vec2Node(x: AxisNode(0), z: AxisNode(0)), FloatNode(200), FloatNode(500), BoolNode(true), EntityNode[player](TargetSelectorNode(@a)))";

    SET_TAG(summon)
    QTest::newRow(GEN_TAG) << "summon lightning_bolt ~-10 ~ ~" <<
        "RootNode[3](LiteralNode(summon), EntitySummonNode(lightning_bolt), Vec3Node(x: AxisNode(~-10), y: AxisNode(~0), z: AxisNode(~0)))";
    QTest::newRow(GEN_TAG) <<
        R"(summon creeper ~ ~ ~ {powered:1b,CustomName:'{"Powered Creeper"}'})"
                           <<
        "RootNode[4](LiteralNode(summon), EntitySummonNode(creeper), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), NbtCompoundNode(CustomName: NbtStringNode({\"Powered Creeper\"}), powered: NbtByteNode(1b)))";
    QTest::newRow(GEN_TAG) <<
        R"(summon spider ~ ~ ~ {Passengers:[{id:"minecraft:skeleton",HandItems:[{id:"minecraft:bow",Count:1b}]}]})"
                           <<
        "RootNode[4](LiteralNode(summon), EntitySummonNode(spider), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), NbtCompoundNode(Passengers: NbtListNode(NbtCompoundNode(HandItems: NbtListNode(NbtCompoundNode(Count: NbtByteNode(1b), id: NbtStringNode(minecraft:bow))), id: NbtStringNode(minecraft:skeleton)))))";
    QTest::newRow(GEN_TAG) <<
        R"(summon villager ~ ~ ~ {Offers:{Recipes:[{buy:{id:dirt,Count:1},sell:{id:diamond,Count:1},rewardExp:false}]}})"
                           <<
        "RootNode[4](LiteralNode(summon), EntitySummonNode(villager), Vec3Node(x: AxisNode(~0), y: AxisNode(~0), z: AxisNode(~0)), NbtCompoundNode(Offers: NbtCompoundNode(Recipes: NbtListNode(NbtCompoundNode(buy: NbtCompoundNode(Count: NbtIntNode(1), id: NbtStringNode(dirt)), rewardExp: NbtByteNode(0b), sell: NbtCompoundNode(Count: NbtIntNode(1), id: NbtStringNode(diamond)))))))";

    SET_TAG(tellraw)
    QTest::newRow(GEN_TAG) << R"(tellraw @a "text to display")" <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode(\"text to display\"))";
    QTest::newRow(GEN_TAG) <<
        R"(tellraw @a {"text":"I am blue","color":"blue"})" <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode({\"color\":\"blue\",\"text\":\"I am blue\"}))";
    QTest::newRow(GEN_TAG) <<
        R"(tellraw @a {"text":"I am blue","color":"#5555ff"})" <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode({\"color\":\"#5555ff\",\"text\":\"I am blue\"}))";
    QTest::newRow(GEN_TAG) <<
        R"(tellraw @a {"text":"Hover me!","hoverEvent":{"action":"show_text","value":"Hi!"}})"
                           <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode({\"hoverEvent\":{\"action\":\"show_text\",\"value\":\"Hi!\"},\"text\":\"Hover me!\"}))";
    QTest::newRow(GEN_TAG) << R"(tellraw @a "Text1\nText2")" <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode(\"Text1\\nText2\"))";
    QTest::newRow(GEN_TAG) <<
        R"(tellraw @a {"rawtext":[{"translate":"chat.type.announcement","with":["value1","value2"]}]})"
                           <<
        "RootNode[3](LiteralNode(tellraw), EntityNode[player](TargetSelectorNode(@a)), ComponentNode({\"rawtext\":[{\"translate\":\"chat.type.announcement\",\"with\":[\"value1\",\"value2\"]}]}))";

    SET_TAG(time)
    QTest::newRow(GEN_TAG) << "time add 90s" <<
        "RootNode[3](LiteralNode(time), LiteralNode(add), TimeNode(90s))";

    SET_TAG(title)
    QTest::newRow(GEN_TAG) <<
        R"(title @a subtitle {"text":"The story begins...","color":"gray","italic":true})"
                           <<
        "RootNode[4](LiteralNode(title), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(subtitle), ComponentNode({\"color\":\"gray\",\"italic\":true,\"text\":\"The story begins...\"}))";
    QTest::newRow(GEN_TAG) <<
        R"(title @a title {"text":"Chapter I","bold":true})" <<
        "RootNode[4](LiteralNode(title), EntityNode[player](TargetSelectorNode(@a)), LiteralNode(title), ComponentNode({\"bold\":true,\"text\":\"Chapter I\"}))";

    SET_TAG(tp)
    QTest::newRow(GEN_TAG) << "tp 100 ~2 300" <<
        "RootNode[2](LiteralNode(tp), Vec3Node(x: AxisNode(100), y: AxisNode(~2), z: AxisNode(300)))";

    SET_TAG(trigger)
    QTest::newRow(GEN_TAG) << "trigger php_help" <<
        "RootNode[2](LiteralNode(trigger), ObjectiveNode(\"php_help\"))";
    QTest::newRow(GEN_TAG) << "trigger php add 1" <<
        "RootNode[4](LiteralNode(trigger), ObjectiveNode(\"php\"), LiteralNode(add), IntegerNode(1))";

    SET_TAG(weather)
    QTest::newRow(GEN_TAG) << "weather clear" <<
        "RootNode[2](LiteralNode(weather), LiteralNode(clear))";
    QTest::newRow(GEN_TAG) << "weather rain 81920" <<
        "RootNode[3](LiteralNode(weather), LiteralNode(rain), IntegerNode(81920))";

    SET_TAG(worldborder)
    QTest::newRow(GEN_TAG) << "worldborder set 5000000" <<
        "RootNode[3](LiteralNode(worldborder), LiteralNode(set), DoubleNode(5e+06))";
    QTest::newRow(GEN_TAG) << "worldborder damage amount 1" <<
        "RootNode[4](LiteralNode(worldborder), LiteralNode(damage), LiteralNode(amount), FloatNode(1))";

    SET_TAG(xp)
    QTest::newRow(GEN_TAG) << "xp add Avicii 1000 levels" <<
        "RootNode[5](LiteralNode(xp), LiteralNode(add), EntityNode[single](StringNode(\"Avicii\")), IntegerNode(1000), LiteralNode(levels))";
    QTest::newRow(GEN_TAG) << "xp set @a 0" <<
        "RootNode[4](LiteralNode(xp), LiteralNode(set), EntityNode[player](TargetSelectorNode(@a)), IntegerNode(0))";

    qDebug() << "Tested" << c << "commands.";
    qDebug() << "Elapsed time in ms:" << localTimer.nsecsElapsed() / 1e6;
}

void TestMinecraftParser::commands() {
    QFETCH(QString, command);
    QFETCH(QString, parseTreeRepr);

    MinecraftParser           parser(this, command);
    QSharedPointer<ParseNode> result = nullptr;
    QBENCHMARK
    {
        result = parser.parse();
    }
    Q_ASSERT(result);
    QVERIFY(result->isVaild());
/*    qDebug() << result->toString(); */
    QCOMPARE(result->toString(), parseTreeRepr);
}

void TestMinecraftParser::benchmark_data() {
    QTest::addColumn<QString>("command");

    /* Last result: 6 msecs per iteration (total: 93, iterations: 16) */
    QTest::addRow("Protector 1.16") <<
        R"(summon minecraft:iron_golem ~ ~ ~ {Health:9999999,Invulnerable:1b,OnGround:1b,PersistenceRequired:1b,Attributes:[{Name:"generic.attack_speed",Base:1024f},{Name:"generic.attack_damage",Base:2048f},{Name:"generic.max_health",Base:9999999F}],ActiveEffects:[{Id:1,Amplifier:5,Duration:999999},{Id:5,Amplifier:1234,Duration:999999},{Id:10,Amplifier:5,Duration:999999},{Id:12,Amplifier:0,Duration:999999},{Id:30,Amplifier:0,Duration:999999},{Id:8,Amplifier:1,Duration:999999},{Id:13,Amplifier:0,Duration:999999}],Passengers:[{id:snow_golem,Pumpkin:0,Invulnerable:1b,ArmorItems:[{id:netherite_boots,Count:1},{id:netherite_leggings,Count:1},{id:netherite_chestplate,Count:1},{id:netherite_helmet,Count:1}],ArmorDropChances:[0F,0F,0F,0F],Passengers:[{id:snow_golem,Pumpkin:0,CustomName:'[{"text":"Guardian of all life"}]',Health:99999999,FallFlying:1b,Invulnerable:1b,PersistenceRequired:1b,HandItems:[{id:netherite_hoe,tag:{display:{Name:"\"GGEZ\""},Unbreakable:1,Enchantments:[{id:sharpness,lvl:10},{id:efficiency,lvl:10},{id:unbreaking,lvl:10},{id:fortune,lvl:4},{id:mending,lvl:1},{id:knockback,lvl:1},{id:fire_aspect,lvl:10},{id:looting,lvl:8}]},Count:1}],HandDropChances:[0.00f],ArmorItems:[{Count:1}],ArmorDropChances:[0.00f],Attributes:[{Name:"generic.follow_range",Base:42f},{Name:"generic.armor_toughness",Base:20f},{Name:"generic.attack_speed",Base:1024f},{Name:"generic.attack_knockback",Base:100f},{Name:"generic.attack_damage",Base:2048f},{Name:"generic.max_health",Base:99999999F}],ActiveEffects:[{Id:1,Amplifier:5,Duration:999999},{Id:5,Amplifier:1234,Duration:999999},{Id:10,Amplifier:5,Duration:999999},{Id:12,Amplifier:0,Duration:999999},{Id:30,Amplifier:0,Duration:999999},{Id:8,Amplifier:1,Duration:999999},{Id:13,Amplifier:0,Duration:999999}]}]}]})";
/*
    QTest::addRow("Dye Villager 1.16.3") <<
    R"(summon minecraft:villager ~1 ~ ~ {VillagerData:{type:plains,profession:shepherd,level:4},Offers:{Recipes:[{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:black_dye",Count:64}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:bone_meal",Count:64,Damage:15}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:gray_dye",Count:64,Damage:7}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:light_gray_dye",Count:64,Damage:1}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:red_dye",Count:64,Damage:14}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:orange_dye",Count:64,Damage:11}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:yellow_dye",Count:64,Damage:2}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:green_dye",Count:64,Damage:10}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:lime_dye",Count:64,Damage:4}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:lapis_lazuli",Count:20,Damage:6}},{buy:{id:"emerald",Count:30},maxUses:1e+25,sell:{id:"minecraft:cyan_dye",Count:64,Damage:12}},{buy:{id:"emerald",Count:30},maxUses:9.9999991e+32,sell:{id:"minecraft:light_blue_dye",Count:64,Damage:5}},{buy:{id:"emerald",Count:30},maxUses:9.9999991e+32,sell:{id:"minecraft:purple_dye",Count:64,Damage:13}},{buy:{id:"emerald",Count:30},maxUses:9.9999991e+32,sell:{id:"minecraft:magenta_dye",Count:64,Damage:9}},{buy:{id:"emerald",Count:30},maxUses:9.9999991e+32,sell:{id:"minecraft:pink_dye",Count:64,Damage:3}},{buy:{id:"emerald",Count:30},maxUses:9.9999991e+32,sell:{id:"minecraft:cocoa_beans",Count:64,Damage:8}}]}})";

    QTest::addRow("real infinity sword 1.16.5") <<
      R"(give @p netherite_sword{display:{Name:"Infinity Sword",AttributeModifiers:[{AttributeName: "generic.attackDamage",Name:"generic.attackDamage",Amount:1e+25,Operation:0,UUIDMost:3058,UUIDLeast:711621}],enchantments:[{id:sharpness,lvl:2147483647},{id:smite,lvl:2147483647},{id:bane_of_arthropods,lvl:2147483647},{id:sweeping_edge,lvl:2147483647},{id:looting,lvl:12},{id:fire_aspect,lvl:13}]})";
 */

    QTest::addRow("A God Sword") <<
        R"(give @p diamond_sword{display:{Name:"{\"text\":\"God Sword\",\"italic\":\"false\",\"color\":\"green\",\"bold\":\"true\"}",Lore:['{"text":"All sword enchantments!"}']},Enchantments:[{lvl:10s,id:"minecraft:knockback"},{lvl:1000s,id:"minecraft:sharpness"},{lvl:100s,id:"minecraft:sweeping"},{lvl:100,id:"minecraft:fire_aspect"},{lvl:100,id:"minecraft:smite"},{lvl:100,id:"minecraft:bane_of_arthropods"},{lvl:100,id:"minecraft:looting"}],Unbreakable:1b,AttributeModifiers:[{AttributeName:"generic.attackSpeed", Name:"generic.attackSpeed", Amount:2000.0, Operation:0, UUIDMost:184053922560081L, UUIDLeast:-864050194979497L}],HideFlags:7} 1)";

    QTest::addRow("kminecraft-pre3 kls:mcplus3/load") <<
        R"(execute if score #K1S_MCPLUS k1s_gen matches 4.. run tellraw @a[tag=!global.ignore.gui] ["",{"text":"Datapack [file/KMinecraft+ pre3] has been disabled by itself","color":"red"},{"text":"\n"},{"text":"Cause: There is/was a newer version in this World","color":"gold"},{"text":"\n"},{"text":"If you think it's a mistake, contact ","color":"yellow"},{"text":"@ultroghast","color":"yellow","clickEvent":{"action":"open_url","value":"t.me/ultroghast"}},{"text":" on Telegram","color":"yellow"},{"text":"\n\n"},{"text":"Check ","color":"light_purple"},{"text":"Here","underlined":true,"color":"light_purple","clickEvent":{"action":"open_url","value":"https://datapackcenter.com/projects/k1s-minecraft-plus.140/"},"hoverEvent":{"action":"show_text","value":"Click for open the link"}},{"text":" for more Updates of K1S Minecraft Plus Datapack","color":"light_purple"}])";
    /* Last result: 3 msecs per iteration (total: 89, iterations: 32) */
    QTest::addRow("kminecraft-pre3 k1s:mcplus3/sub/mag_ter") <<
        R"(execute if block ~ ~-0.3 ~ minecraft:magenta_glazed_terracotta[facing=south] if block ~ ~ ~-0.7 #k1s:attraversable if block ~ ~1 ~-0.7 #k1s:attraversable if block ~ ~ ~-0.6 #k1s:attraversable if block ~ ~1 ~-0.6 #k1s:attraversable unless block ~ ~-0.3 ~-0.6 #k1s:attraversable run effect give @s minecraft:slowness 1 2 true)";
    QTest::addRow("Timber timber:acacia_tree/chop") <<
        R"(execute if score tree_size timber < max_tree_size timber positioned ~-1 ~1 ~-1 if block ~ ~ ~ #timber:acacia unless entity @e[type=minecraft:area_effect_cloud,tag=timber_destroy,distance=...9,sort=arbitrary,limit=1] run function timber:acacia_tree/chop)";
    QTest::addRow("Timber timber:acacia_tree/validate") <<
        R"(execute positioned ~ ~ ~-1 unless entity @e[type=minecraft:area_effect_cloud,tag=timber_checked,distance=...9,sort=nearest,limit=1] if block ~ ~ ~ minecraft:acacia_leaves[persistent=false] run summon area_effect_cloud ~ ~ ~ {Tags:["timber_checked"],Duration:1})";
    /* 0.1 msecs per iteration (total: 65, iterations: 512) */
    QTest::addRow("RAINBOW TEXT by syedgamer") <<
        R"(tellraw @p ["",{"text":"R","color":"red","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Red","color":"red"}]}}},{"text":"A","color":"gold","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Orange","color":"gold"}]}}},{"text":"I","color":"yellow","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Yellow","color":"yellow"}]}}},{"text":"N","color":"green","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Green","color":"green"}]}}},{"text":"B","color":"aqua","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Blue","color":"aqua"}]}}},{"text":"O","color":"blue","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Indigo","color":"dark_blue"}]}}},{"text":"W","color":"dark_purple","underlined":true,"hoverEvent":{"action":"show_text","value":{"text":"","extra":[{"text":"Purple","color":"dark_purple"}]}}}])";
    /* 2 msecs per iteration (total: 72, iterations: 32) */
    QTest::addRow("Game crasher") <<
        R"(tellraw @a [{"text":"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"},{"text":"⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊","color":"white"},{"text":"\n These are regular crashes. There are different levels for them. Depending on how strong your computer is, you may need a higher or lower level to crash your game. Click on a level to activate it."},{"text":"\n\n                                  ","color":"white"},{"text":"[","color":"white","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 1"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 1 is for babies. \nAny computer can pass this. You may get a small lag spike, but that's about it.","color":"#93ff54"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"Level 1","color":"#93ff54","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 1"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 1 is for babies. \nAny computer can pass this. You may get a small lag spike, but that's about it.","color":"#93ff54"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 1"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 1 is for babies. \nAny computer can pass this. You may get a small lag spike, but that's about it.","color":"#93ff54"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"\n                                  ","color":"white"},{"text":"[","color":"white","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 2"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 2 ain't so pretty as level 1. \nLow end computer may give out on this one, but most will survive","color":"#fffa5e"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"Level 2","color":"#fffa5e","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 2"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 2 ain't so pretty as level 1. \nLow end computer may give out on this one, but most will survive","color":"#fffa5e"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 2"},"hoverEvent":{"action":"show_text","contents":[{"text":"Level 2 ain't so pretty as level 1. \nLow end computer may give out on this one, but most will survive","color":"#fffa5e"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"\n                                  [","color":"white"},{"text":"Level 3","color":"#ffa340","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 3"},"hoverEvent":{"action":"show_text","contents":[{"text":"Playtime is over. Level 3 ain't no joke.\nMid-range computers will struggle with this, some may not survive. High-end computers will expereince a long lag spike, but won't crash. If you think your computer can get through, be patient. Don't do anything, just wait. It just might survive.","color":"#ffa340"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white"},{"text":"\n                                  [","color":"white"},{"text":"Level 4","color":"#ff3a17","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 4"},"hoverEvent":{"action":"show_text","contents":[{"text":"No more mercy. Level 4 = crash.\nForget about low and mid-range computers, they're long gone. High-end computers will struggle with this, but, it's not impossible... This level is still survivable.","color":"#ff3a17"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white"},{"text":"\n                                  [","color":"white"},{"text":"Level 5","color":"#cf0000","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 5"},"hoverEvent":{"action":"show_text","contents":[{"text":"You're done for. It's over\nThis is where my own computer gave up. It didn't have the strenght to do this. If you survive this level, you will have full bragging rights. But I doubt you will, I doubt.","color":"#cf0000"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white"},{"text":"\n                                  [","color":"white"},{"text":"Level 6","color":"#610000","bold":"true","clickEvent":{"action":"run_command","value":"/trigger CrashAction set 6"},"hoverEvent":{"action":"show_text","contents":[{"text":"The legendary Death Crash\nThe Death Crash is an impossible to survive crash. No one has ever survived it. Legends say that only the chosen one may beat the Death Crash. Many have tried, many thought that they were the chosen one, but all were defeated. We're still waiting for the day when the chosen one will come and defeat the Death Crash. If you dare face the Death Crash, be warned: there is no coming back.","color":"#610000"},{"text":"\n\nDoesn't damage your world","color":"dark_gray"}]}},{"text":"]","color":"white"},{"text":"\n\n [","color":"white","hoverEvent":{"action":"show_text","value":"Brings you back to the crash selection menu"},"clickEvent":{"action":"run_command","value":"/trigger Crash"}},{"text":"Back","color":"red","hoverEvent":{"action":"show_text","value":"Brings you back to the crash selection menu"},"clickEvent":{"action":"run_command","value":"/trigger Crash"}},{"text":"]","color":"white","hoverEvent":{"action":"show_text","value":"Brings you back to the crash selection menu"},"clickEvent":{"action":"run_command","value":"/trigger Crash"}},{"text":"\n⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊⚊","color":"white"}])";
    /* 4 msecs per iteration (total: 64, iterations: 16) */
    QTest::addRow("Kits by theyellowtitan") <<
        R"raw(tellraw @p ["",{"text":"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"},{"text":"x-x-x-xX","color":"gold"},{"text":"Kits","color":"dark_red"},{"text":"Xx-x-x-x\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"1xStock SchÃ¤rfe 1\n3xGoldÃ¤pfel\nGoldrÃ¼stung Schutz 1","color":"dark_green"},{"text":"\n "}]}},{"text":"GApple......................................","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"},{"text":"----------------------------------------\nName:","color":"gold"},{"text":"GApple","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:âš Legendaryâš \nKosten:","color":"gold"},{"text":"10.000Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall...Wenn du dich traust;)","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"1xHolzschwert\n32xSteaks","color":"dark_green"},{"text":"\n"},{"text":"h","obfuscated":true,"color":"dark_red"},{"text":"Hacks","color":"dark_purple"},{"text":"h","obfuscated":true,"color":"dark_red"}]}},{"text":"Hacker.....................................X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"1xStock SchÃ¤rfe 2,Verbrennung 1\nViolette LederrÃ¼stung Schutz 3","color":"dark_green"},{"text":"\n"},{"text":"Schnelligkeit 1","color":"dark_aqua"}]}},{"text":"Medic...........................................","color":"light_purple","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Medic","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Epicâš ","color":"light_purple"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"5000Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"1xEisenrÃ¼stung\n1xEisenschwert\n32xSteaks","color":"dark_green"},{"text":"\n "}]}},{"text":"Assasult................................","color":"light_purple","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Assasult","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Epicâš ","color":"light_purple"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"5000Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":".","color":"red","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Assasult","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Epicâš ","color":"light_purple"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"5000Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv","color":"gold"},{"text":"\n"},{"text":"5xWurftrank Der Langsamkeit 1\n1xTrank Der StÃ¤rke 1\n5xWurftrÃ¤nke Des Schadens 1","color":"dark_green"},{"text":"\n"},{"text":"Schnelligkeit 1","color":"dark_aqua"},{"text":"\n"},{"text":"SchwÃ¤che 1","color":"dark_red"}]}},{"text":"Mage............................................","color":"dark_red","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Mage","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Uncommonâš ","color":"dark_red"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"2500Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"1xBogen Schlag 3,Haltbarkeit 5,\n1xEisenbrustplatte Schutz 3\n2x64 Pfeile","color":"dark_green"},{"text":"\n"},{"text":"Langsamkeit 1","color":"dark_red"}]}},{"text":"Sniper........................................","color":"dark_red","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Sniper","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Uncommonâš ","color":"dark_red"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"2500Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"16xEnderperlen\n1xLederrÃ¼stung Dunkelblau Schutz 1","color":"dark_green"},{"text":"\n "}]}},{"text":"Ender.........................................","color":"gray","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Ender","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"âš Commonâš ","color":"gray"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"1500Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX","color":"gold"},{"text":"[?]","color":"gold","hoverEvent":{"action":"show_text","value":["",{"text":"inv:","color":"gold"},{"text":"\n"},{"text":"32xTNT\n1xGoldrÃ¼stung Explosionsschutz 3","color":"dark_green"},{"text":"\n"},{"text":"Speed 1","color":"dark_aqua"}]}},{"text":"Sprenmeister.....................","color":"gray","hoverEvent":{"action":"show_text","value":["",{"text":"----------------------------------------\nName:","color":"gold"},{"text":"Sprengmeister","color":"yellow"},{"text":"\n"},{"text":"Seltenheit:","color":"gold"},{"text":"Common","color":"gray"},{"text":"\n"},{"text":"Kosten:","color":"gold"},{"text":"1000Punkte","color":"dark_aqua"},{"text":"\n"},{"text":"Aktivirung:","color":"gold"},{"text":"Im Kit-Shop","color":"dark_blue"},{"text":"\n"},{"text":"VerfÃ¼gber:","color":"gold"},{"text":"Ãœberall","color":"dark_blue"},{"text":"\n"},{"text":"----------------------------------------","color":"gold"}]}},{"text":"X\nX|-----","color":"gold"},{"text":"[Ver1.0]","color":"gold","hoverEvent":{"action":"show_text","value":{"text":"Schreib mich an fÃ¼r neue Kits\nWichtig ist:\nâˆ™ Name\nâˆ™ Preis\nâˆ™ Inv\nâˆ™ Seltenheit\nâˆ™ Name von dir(du krigst selbst auf dein Kit 25%Rabatt\nDas Kit Muss sinnvoll sein und nicht zu OP\nIch freue mich auf einsendungen ;-)\n-TheYellowTitan","color":"gold"}}},{"text":"-----|X\nXXXXXXXXXXXXXXXXXXX","color":"gold"}])raw";
}

void TestMinecraftParser::benchmark() {
    benchmarkCommandBoxes();
}

/* QTest::addRow("") << R"()"; */

void TestMinecraftParser::benchmarkCommandBoxes_data() {
    QTest::addColumn<QString>("command");

    /* Last result: 4 msecs per iteration (total: 70, iterations: 16) */
    QTest::addRow("Custom arrows") <<
        R"(summon minecraft:falling_block ~ ~1 ~ {BlockState:{Name:"redstone_block"},Time:1,Passengers:[{id:armor_stand,Health:0,Passengers:[{id:falling_block,BlockState:{Name:"activator_rail"},Time:1,Passengers:[{id:command_block_minecart,Command:"fill ~2 ~-2 ~4 ~-1 ~2 ~7 minecraft:iron_block"},{id:command_block_minecart,Command:"fill ~2 ~-1 ~4 ~-1 ~1 ~7 minecraft:lime_stained_glass"},{id:command_block_minecart,Command:"fill ~1 ~-1 ~5 ~ ~1 ~6 minecraft:air"},{id:command_block_minecart,Command:"setblock ~ ~ ~3 minecraft:oak_wall_sign[facing=north]{Text2:'{\"text\":\"Destroy Sign to\"}',Text3:'{\"text\":\"Destroy Command\"}'}"},{id:command_block_minecart,Command:"setblock ~ ~-1 ~5 minecraft:repeating_command_block[facing=up]{auto:1b,Command:\"execute at @a if block ~ ~-1 ~ minecraft:dropper{Items:[{Slot:0b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:1b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:2b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:3b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:4b,Count:1b,id:\\\"minecraft:creeper_head\\\"},{Slot:5b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:6b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:7b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:8b,Count:4b,id:\\\"minecraft:arrow\\\"}]} run data merge block ~ ~-1 ~ {Items:[{Slot:4b,Count:32b,id:\\\"minecraft:tipped_arrow\\\",tag:{CustomPotionColor:13565952,display:{Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Exploding Arrow\\\\\\\"}\\\",Lore:['[{\\\"text\\\":\\\"Explodes\\\",\\\"italic\\\":false,\\\"color\\\":\\\"red\\\"}]']}}}]}\"}"},{id:command_block_minecart,Command:"setblock ~ ~ ~5 minecraft:chain_command_block[facing=up]{auto:1b,Command:\"execute at @e[type=minecraft:arrow,nbt={Color:13565952,inGround:1b}] run summon minecraft:creeper ~ ~ ~ {Fuse:0}\"}"},{id:command_block_minecart,Command:"setblock ~ ~1 ~5 minecraft:chain_command_block[facing=south]{auto:1b,Command:\"kill @e[type=minecraft:arrow,nbt={Color:13565952,inGround:1b}]\"}"},{id:command_block_minecart,Command:"setblock ~ ~1 ~6 minecraft:chain_command_block[facing=down]{auto:1b,Command:\"execute at @a if block ~ ~-1 ~ minecraft:dropper{Items:[{Slot:0b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:1b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:2b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:3b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:4b,Count:1b,id:\\\"minecraft:enchanted_book\\\",tag:{StoredEnchantments:[{lvl:1s,id:\\\"minecraft:channeling\\\"}]}},{Slot:5b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:6b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:7b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:8b,Count:4b,id:\\\"minecraft:arrow\\\"}]} run data merge block ~ ~-1 ~ {Items:[{Slot:4b,Count:32b,id:\\\"minecraft:tipped_arrow\\\",tag:{CustomPotionColor:65527,display:{Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Lightning Arrow\\\\\\\"}\\\",Lore:['[{\\\"text\\\":\\\"Summons Lightning\\\",\\\"italic\\\":false,\\\"color\\\":\\\"aqua\\\"}]']}}}]}\"}"},{id:command_block_minecart,Command:"setblock ~ ~ ~6 minecraft:chain_command_block[facing=down]{auto:1b,Command:\"execute at @e[type=minecraft:arrow,nbt={Color:65527,inGround:1b}] run summon minecraft:lightning_bolt\"}"},{id:command_block_minecart,Command:"setblock ~ ~-1 ~6 minecraft:chain_command_block[facing=east]{auto:1b,Command:\"kill @e[type=minecraft:arrow,nbt={Color:65527,inGround:1b}]\"}"},{id:command_block_minecart,Command:"setblock ~1 ~-1 ~6 minecraft:chain_command_block[facing=up]{auto:1b,Command:\"execute at @a if block ~ ~-1 ~ minecraft:dropper{Items:[{Slot:0b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:1b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:2b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:3b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:4b,Count:1b,id:\\\"minecraft:totem_of_undying\\\"},{Slot:5b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:6b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:7b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:8b,Count:4b,id:\\\"minecraft:arrow\\\"}]} run data merge block ~ ~-1 ~ {Items:[{Slot:4b,Count:32b,id:\\\"minecraft:tipped_arrow\\\",tag:{CustomPotionColor:9177343,display:{Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Fanged Arrow\\\\\\\"}\\\",Lore:['[{\\\"text\\\":\\\"Devours Your Foes\\\",\\\"italic\\\":false,\\\"color\\\":\\\"dark_purple\\\"}]']}}}]}\"}"},{id:command_block_minecart,Command:"setblock ~1 ~ ~6 minecraft:chain_command_block[facing=up]{auto:1b,Command:\"execute at @e[type=minecraft:arrow,nbt={Color:9177343,inGround:1b}] run summon minecraft:evoker_fangs\"}"},{id:command_block_minecart,Command:"setblock ~1 ~1 ~6 minecraft:chain_command_block[facing=north]{auto:1b,Command:\"kill @e[type=minecraft:arrow,nbt={Color:9177343,inGround:1b}]\"}"},{id:command_block_minecart,Command:"setblock ~1 ~1 ~5 minecraft:chain_command_block[facing=down]{auto:1b,Command:\"execute at @a if block ~ ~-1 ~ minecraft:dropper{Items:[{Slot:0b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:1b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:2b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:3b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:4b,Count:1b,id:\\\"minecraft:shulker_shell\\\"},{Slot:5b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:6b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:7b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:8b,Count:4b,id:\\\"minecraft:arrow\\\"}]} run data merge block ~ ~-1 ~ {Items:[{Slot:4b,Count:32b,id:\\\"minecraft:tipped_arrow\\\",tag:{CustomPotionColor:9408399,display:{Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Levitation Arrow\\\\\\\"}\\\"},CustomPotionEffects:[{Id:25,Duration:60}]}}]}\"}"},{id:command_block_minecart,Command:"setblock ~1 ~ ~5 minecraft:chain_command_block[facing=down]{auto:1b,Command:\"execute at @a if block ~ ~-1 ~ minecraft:dropper{Items:[{Slot:0b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:1b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:2b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:3b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:4b,Count:1b,id:\\\"minecraft:wither_rose\\\"},{Slot:5b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:6b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:7b,Count:4b,id:\\\"minecraft:arrow\\\"},{Slot:8b,Count:4b,id:\\\"minecraft:arrow\\\"}]} run data merge block ~ ~-1 ~ {Items:[{Slot:4b,Count:32b,id:\\\"minecraft:tipped_arrow\\\",tag:{CustomPotionColor:3673169,display:{Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Decaying Arrow\\\\\\\"}\\\"},CustomPotionEffects:[{Id:20,Amplifier:1,Duration:100}]}}]}\"}"},{id:command_block_minecart,Command:"setblock ~1 ~-1 ~5 minecraft:chain_command_block[facing=west]{auto:1b,Command:\"execute if block ~-1 ~1 ~-2 air run fill ~1 ~-1 ~-1 ~-2 ~3 ~2 air\"}"},{id:command_block_minecart,Command:"setblock ~ ~1 ~ command_block{auto:1,Command:'kill @e[type=command_block_minecart,distance=..3]'}"},{id:command_block_minecart,Command:"setblock ~ ~2 ~ command_block{auto:1,Command:'fill ~ ~ ~ ~ ~-3 ~ air'}"}]}]}]})";
    /* Last result: 15 msecs per iteration */
    QTest::addRow("Coral reviver by IoeCmcomc") <<
        R"(summon falling_block ~ ~1 ~ {BlockState:{Name:"stone"},Time:1,Passengers:[{id:falling_block,BlockState:{Name:"redstone_block"},Time:1,Passengers:[{id:falling_block,BlockState:{Name:"activator_rail"},Time:1,Passengers:[{id:command_block_minecart,Command:"gamerule commandBlockOutput false"},{id:command_block_minecart,Command:"fill ~2 ~-3 ~-2 ~8 ~3 ~2 light_blue_glazed_terracotta hollow"},{id:command_block_minecart,Command:"fill ~2 ~-2 ~-2 ~8 ~2 ~2 blue_stained_glass replace light_blue_glazed_terracotta"},{id:command_block_minecart,Command:"tellraw @a {\"text\":\"[Thông báo] \",\"color\":\"gold\",\"extra\":[{\"text\":\"&#272;ã cài &#273;&#7863;t thành công h&#7897;p l&#7879;nh \",\"color\":\"green\"},{\"text\":\"Coral Reviver by IoeCmcomc\",\"color\":\"green\",\"bold\":true,\"italic\":true,\"clickEvent\":{\"action\":\"open_url\",\"value\":\"https://youtu.be/tZEdN4bj-B4\"}}]}"},{id:command_block_minecart,Command:"setblock ~1 ~-1 ~ wall_sign[facing=west]{Text1:\"{\\\"text\\\":\\\"Máy Tái Sinh\\\",\\\"bold\\\":true}\",Text2:\"{\\\"text\\\":\\\"San Hô\\\",\\\"bold\\\":true}\",Text3:\"{\\\"text\\\":\\\"b&#7903;i \\\"}\",Text4:\"{\\\"text\\\":\\\"IoeCmcomc\\\",\\\"color\\\":\\\"green\\\",\\\"italic\\\":true}\"}"},{id:command_block_minecart,Command:"setblock ~1 ~ ~ wall_sign[facing=west]{Text1:\"{\\\"text\\\":\\\"\\\"}\",Text2:\"{\\\"text\\\":\\\"Xóa\\\",\\\"color\\\":\\\"red\\\",\\\"bold\\\":true}\",Text3:\"{\\\"text\\\":\\\"h&#7897;p l&#7879;nh\\\"}\",Text4:\"{\\\"text\\\":\\\"\\\",\\\"clickEvent\\\":{\\\"action\\\":\\\"run_command\\\",\\\"value\\\":\\\"fill ~ ~-3 ~-2 ~7 ~3 ~2 air\\\"}}\"}"},{id:command_block_minecart,Command:"fill ~3 ~-2 ~-1 ~7 ~2 ~1 chain_command_block[facing=east]"},{id:command_block_minecart,Command:"setblock ~7 ~2 ~-1 chain_command_block[facing=south]"},{id:command_block_minecart,Command:"setblock ~3 ~1 ~-1 chain_command_block[facing=up]"},{id:command_block_minecart,Command:"fill ~7 ~1 ~-1 ~4 ~1 ~-1 chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~1 ~ chain_command_block[facing=north]"},{id:command_block_minecart,Command:"setblock ~3 ~1 ~1 chain_command_block[facing=north]"},{id:command_block_minecart,Command:"fill ~7 ~1 ~1 ~4 ~1 ~1 chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~ ~1 chain_command_block[facing=up]"},{id:command_block_minecart,Command:"setblock ~3 ~ ~ chain_command_block[facing=south]"},{id:command_block_minecart,Command:"fill ~7 ~ ~ ~4 ~ ~ chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~ ~-1 chain_command_block[facing=south]"},{id:command_block_minecart,Command:"setblock ~3 ~-1 ~-1 chain_command_block[facing=up]"},{id:command_block_minecart,Command:"fill ~7 ~-1 ~-1 ~4 ~-1 ~-1 chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~-1 ~ chain_command_block[facing=north]"},{id:command_block_minecart,Command:"setblock ~3 ~-1 ~1 chain_command_block[facing=north]"},{id:command_block_minecart,Command:"fill ~7 ~-1 ~1 ~4 ~-1 ~1 chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~-2 ~1 chain_command_block[facing=up]"},{id:command_block_minecart,Command:"setblock ~3 ~-2 ~ chain_command_block[facing=south]"},{id:command_block_minecart,Command:"fill ~7 ~-2 ~ ~4 ~-2 ~ chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~7 ~-2 ~-1 chain_command_block[facing=south]"},{id:command_block_minecart,Command:"setblock ~3 ~-2 ~-1 repeating_command_block[facing=east]"},{id:command_block_minecart,Command:"data merge block ~7 ~2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=north,waterlogged=false] replace minecraft:dead_horn_coral_wall_fan[facing=north,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=south,waterlogged=false] replace minecraft:dead_horn_coral_wall_fan[facing=south,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=west,waterlogged=false] replace minecraft:dead_horn_coral_wall_fan[facing=west,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=east,waterlogged=false] replace minecraft:dead_horn_coral_wall_fan[facing=east,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=north,waterlogged=false] replace minecraft:dead_fire_coral_wall_fan[facing=north,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=south,waterlogged=false] replace minecraft:dead_fire_coral_wall_fan[facing=south,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=west,waterlogged=false] replace minecraft:dead_fire_coral_wall_fan[facing=west,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=east,waterlogged=false] replace minecraft:dead_fire_coral_wall_fan[facing=east,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=north,waterlogged=false] replace minecraft:dead_bubble_coral_wall_fan[facing=north,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=south,waterlogged=false] replace minecraft:dead_bubble_coral_wall_fan[facing=south,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=west,waterlogged=false] replace minecraft:dead_bubble_coral_wall_fan[facing=west,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=east,waterlogged=false] replace minecraft:dead_bubble_coral_wall_fan[facing=east,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=north,waterlogged=false] replace minecraft:dead_brain_coral_wall_fan[facing=north,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=south,waterlogged=false] replace minecraft:dead_brain_coral_wall_fan[facing=south,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=west,waterlogged=false] replace minecraft:dead_brain_coral_wall_fan[facing=west,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=east,waterlogged=false] replace minecraft:dead_brain_coral_wall_fan[facing=east,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=north,waterlogged=false] replace minecraft:dead_tube_coral_wall_fan[facing=north,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=south,waterlogged=false] replace minecraft:dead_tube_coral_wall_fan[facing=south,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=west,waterlogged=false] replace minecraft:dead_tube_coral_wall_fan[facing=west,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=east,waterlogged=false] replace minecraft:dead_tube_coral_wall_fan[facing=east,waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~ ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_fan[waterlogged=false] replace minecraft:dead_horn_coral_fan[waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~ ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_fan[waterlogged=false] replace minecraft:dead_fire_coral_fan[waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~ ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_fan[waterlogged=false] replace minecraft:dead_bubble_coral_fan[waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~ ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_fan[waterlogged=false] replace minecraft:dead_brain_coral_fan[waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~ ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_fan[waterlogged=false] replace minecraft:dead_tube_coral_fan[waterlogged=false]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~ ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=north,waterlogged=true] replace minecraft:dead_horn_coral_wall_fan[facing=north,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~ ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=south,waterlogged=true] replace minecraft:dead_horn_coral_wall_fan[facing=south,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~ ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=west,waterlogged=true] replace minecraft:dead_horn_coral_wall_fan[facing=west,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~ ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_wall_fan[facing=east,waterlogged=true] replace minecraft:dead_horn_coral_wall_fan[facing=east,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~ ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=north,waterlogged=true] replace minecraft:dead_fire_coral_wall_fan[facing=north,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~ ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=south,waterlogged=true] replace minecraft:dead_fire_coral_wall_fan[facing=south,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~ ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=west,waterlogged=true] replace minecraft:dead_fire_coral_wall_fan[facing=west,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~ ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_wall_fan[facing=east,waterlogged=true] replace minecraft:dead_fire_coral_wall_fan[facing=east,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~ ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=north,waterlogged=true] replace minecraft:dead_bubble_coral_wall_fan[facing=north,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~ ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=south,waterlogged=true] replace minecraft:dead_bubble_coral_wall_fan[facing=south,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=west,waterlogged=true] replace minecraft:dead_bubble_coral_wall_fan[facing=west,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_wall_fan[facing=east,waterlogged=true] replace minecraft:dead_bubble_coral_wall_fan[facing=east,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=north,waterlogged=true] replace minecraft:dead_brain_coral_wall_fan[facing=north,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=south,waterlogged=true] replace minecraft:dead_brain_coral_wall_fan[facing=south,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-1 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=west,waterlogged=true] replace minecraft:dead_brain_coral_wall_fan[facing=west,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_wall_fan[facing=east,waterlogged=true] replace minecraft:dead_brain_coral_wall_fan[facing=east,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=north,waterlogged=true] replace minecraft:dead_tube_coral_wall_fan[facing=north,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=south,waterlogged=true] replace minecraft:dead_tube_coral_wall_fan[facing=south,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=west,waterlogged=true] replace minecraft:dead_tube_coral_wall_fan[facing=west,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-1 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_wall_fan[facing=east,waterlogged=true] replace minecraft:dead_tube_coral_wall_fan[facing=east,waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_fan[waterlogged=true] replace minecraft:dead_horn_coral_fan[waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_fan[waterlogged=true] replace minecraft:dead_fire_coral_fan[waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_fan[waterlogged=true] replace minecraft:dead_bubble_coral_fan[waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_fan[waterlogged=true] replace minecraft:dead_brain_coral_fan[waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-1 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_fan[waterlogged=true] replace minecraft:dead_tube_coral_fan[waterlogged=true]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:horn_coral_block replace minecraft:dead_horn_coral_block\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:fire_coral_block replace minecraft:dead_fire_coral_block\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:bubble_coral_block replace minecraft:dead_bubble_coral_block\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:tube_coral_block replace minecraft:dead_tube_coral_block\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~1 {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run fill ~3 ~4 ~3 ~-3 ~-2 ~-3 minecraft:brain_coral_block replace minecraft:dead_brain_coral_block\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s if block ~ ~1 ~ air run kill @s\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s if block ~ ~1 ~ water run kill @s\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s if block ~ ~1 ~ air run data merge entity @e[limit=1,distance=0..4,type=item,nbt={Item:{id:\\\"minecraft:green_stained_glass\\\",Count:1b},}] {Item:{id:\\\"minecraft:squid_spawn_egg\\\",Count:1b,tag:{RepairCost:696969,HideFlags:1,display:{Lore:[\\\"English name: Coral reviver\\\"],Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Máy tái sinh san hô\\\\\\\",\\\\\\\"italic\\\\\\\":true}\\\"},Enchantments:[{lvl:0s,id:\\\"minecraft:luck_of_the_sea\\\"}]}}}\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s if block ~ ~1 ~ water run data merge entity @e[limit=1,distance=0..4,type=item,nbt={Item:{id:\\\"minecraft:green_stained_glass\\\",Count:1b},}] {Item:{id:\\\"minecraft:squid_spawn_egg\\\",Count:1b,tag:{RepairCost:696969,HideFlags:1,display:{Lore:[\\\"English name: Coral reviver\\\"],Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Máy tái sinh san hô\\\\\\\",\\\\\\\"italic\\\\\\\":true}\\\"},Enchantments:[{lvl:0s,id:\\\"minecraft:luck_of_the_sea\\\"}]}}}\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~ {auto:1,Command:\"execute as @e[type=minecraft:armor_stand,name=Coral_reviver] at @s run tp @s ~ ~ ~ facing entity @a[limit=1,distance=..5,sort=nearest]\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:squid,name=\\\"Máy tái sinh san hô\\\",nbt={Health:10.0f,Air:300s}] at @s run tp @s ~ ~-500 ~\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:squid,name=\\\"Máy tái sinh san hô\\\",nbt={Health:10.0f,Air:300s}] at @s if block ~ ~ ~ minecraft:green_stained_glass run summon minecraft:armor_stand ~ ~-0.25 ~ {CustomName:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Coral_reviver\\\\\\\"}\\\",Invisible:1b,ArmorItems:[{},{},{},{id:\\\"minecraft:heart_of_the_sea\\\",Count:1b}],NoGravity:1b,Pose:{Head:[180.0f, 0.0f, 0.0f]}}\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:squid,name=\\\"Máy tái sinh san hô\\\",nbt={Health:10.0f,Air:300s}] at @s if block ~ ~ ~ minecraft:water run setblock ~ ~ ~ minecraft:green_stained_glass\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~-1 {auto:1,Command:\"execute as @e[type=minecraft:squid,name=\\\"Máy tái sinh san hô\\\",nbt={Health:10.0f,Air:300s}] at @s if block ~ ~ ~ minecraft:air if entity @p[gamemode=survival] run summon minecraft:item ~ ~ ~ {Item:{id:\\\"minecraft:squid_spawn_egg\\\",Count:1b,tag:{RepairCost:696969,HideFlags:1,display:{Lore:[\\\"English name: Coral reviver\\\"],Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Máy tái sinh san hô\\\\\\\",\\\\\\\"italic\\\\\\\":true}\\\"},Enchantments:[{lvl:0s,id:\\\"minecraft:luck_of_the_sea\\\"}]}}}\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~-1 {auto:1,Command:\"execute at @a if block ~ ~-2 ~ minecraft:crafting_table if block ~ ~-1 ~ minecraft:dropper[]{ Items: [{Slot: 0b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 1b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 2b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 3b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 4b, id: \\\"minecraft:heart_of_the_sea\\\", Count: 1b}, {Slot: 5b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 6b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 7b, id: \\\"minecraft:prismarine\\\", Count: 1b}, {Slot: 8b, id: \\\"minecraft:prismarine\\\", Count: 1b}], id: \\\"minecraft:dropper\\\"} run data merge block ~ ~-1 ~ {Items: [{Slot: 4b, id: \\\"minecraft:squid_spawn_egg\\\", Count: 1b, tag: {RepairCost: 696969, display: {Name:\\\"{\\\\\\\"text\\\\\\\":\\\\\\\"Máy tái sinh san hô\\\\\\\",\\\\\\\"italic\\\\\\\":true}\\\",Lore:[\\\"English name: Coral reviver\\\"]},HideFlags:1,Enchantments:[{lvl:0s,id:\\\"minecraft:luck_of_the_sea\\\"}]}}],id:\\\"minecraft:dropper\\\"}\"}"},{id:command_block_minecart,Command:"setblock ~ ~ ~1 command_block[]{Command:\"fill ~ ~-3 ~-1 ~ ~ ~ air\"}"},{id:command_block_minecart,Command:"setblock ~ ~-1 ~1 redstone_block"},{id:command_block_minecart,Command:"kill @e[type=command_block_minecart,distance=..1]"}]}]}]})";
    /* Last result: 8 msecs per iteration */
    QTest::addRow("Tree Capitator by BlueCommander") <<
        R"(summon falling_block ~ ~.5 ~ {Motion:[.0,.5,.0],BlockState:{Name:activator_rail},Time:1,Passengers:[{id:falling_block,BlockState:{Name:redstone_block},Time:1},{id:falling_block,BlockState:{Name:activator_rail},Time:1,Passengers:[{id:command_block_minecart,Command:"gamerule commandBlockOutput false"},{id:command_block_minecart,Command:"fill ~2 ~-3 ~-2 ~9 ~ ~3 blue_terracotta hollow"},{id:command_block_minecart,Command:"fill ~2 ~-2 ~-2 ~9 ~-1 ~3 barrier replace blue_terracotta"},{id:command_block_minecart,Command:"tellraw @a [\"\",{\"text\":\"Tree Capitator> \",\"color\":\"blue\",\"bold\":false},{\"text\":\"One command For 1.14 installed!\",\"color\":\"none\",\"bold\":false}]"},{id:command_block_minecart,Command:"tellraw @a [\"\",{\"text\":\"Tree Capitator> \",\"color\":\"blue\",\"bold\":false},{\"text\":\"Created by BlueCommander!\",\"color\":\"none\",\"bold\":false}]"},{id:command_block_minecart,Command:"tellraw @a [\"\",{\"text\":\"Tree Capitator> \",\"color\":\"blue\",\"bold\":false},{\"text\":\"Big thanks to MrGarreto for the one command combiner!\",\"color\":\"none\",\"bold\":false}]"},{id:command_block_minecart,Command:"scoreboard objectives add B_MO minecraft.mined:minecraft.oak_log"},{id:command_block_minecart,Command:"scoreboard objectives add B_MB minecraft.mined:minecraft.birch_log"},{id:command_block_minecart,Command:"scoreboard objectives add B_MS minecraft.mined:minecraft.spruce_log"},{id:command_block_minecart,Command:"scoreboard objectives add B_MJ minecraft.mined:minecraft.jungle_log"},{id:command_block_minecart,Command:"scoreboard objectives add B_MA minecraft.mined:minecraft.acacia_log"},{id:command_block_minecart,Command:"scoreboard objectives add B_MD minecraft.mined:minecraft.dark_oak_log"},{id:command_block_minecart,Command:"forceload add ~ ~ ~10 ~"},{id:command_block_minecart,Command:"setblock ~1 ~-2 ~ oak_wall_sign[facing=west]{Text1:\"{\\\"text\\\":\\\"[Destroy]\\\",\\\"color\\\":\\\"dark_red\\\",\\\"bold\\\":true}\",Text2:\"{\\\"text\\\":\\\"Right Click\\\",\\\"color\\\":\\\"white\\\",\\\"bold\\\":true}\",Text3:\"{\\\"text\\\":\\\"\\\"}\",Text4:\"{\\\"text\\\":\\\"\\\",\\\"clickEvent\\\":{\\\"action\\\":\\\"run_command\\\",\\\"value\\\":\\\"fill ~ ~-1 ~-2 ~8 ~2 ~3 air\\\"}}\"}"},{id:command_block_minecart,Command:"setblock ~1 ~-1 ~ oak_wall_sign[facing=west]{Text1:\"{\\\"text\\\":\\\"Tree Capitator\\\",\\\"color\\\":\\\"gold\\\",\\\"bold\\\":true}\",Text2:\"{\\\"text\\\":\\\"By\\\",\\\"color\\\":\\\"white\\\",\\\"bold\\\":true}\",Text3:\"{\\\"text\\\":\\\"Blue\\\",\\\"color\\\":\\\"blue\\\",\\\"bold\\\":true}\",Text4:\"{\\\"text\\\":\\\"Commander\\\",\\\"color\\\":\\\"blue\\\",\\\"bold\\\":true}\"}"},{id:command_block_minecart,Command:"fill ~3 ~-2 ~-1 ~8 ~-1 ~2 chain_command_block[facing=east]"},{id:command_block_minecart,Command:"setblock ~3 ~-2 ~2 chain_command_block[facing=up]"},{id:command_block_minecart,Command:"fill ~8 ~-2 ~2 ~4 ~-2 ~2 chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~8 ~-2 ~1 chain_command_block[facing=south]"},{id:command_block_minecart,Command:"setblock ~3 ~-2 ~ chain_command_block[facing=south]"},{id:command_block_minecart,Command:"fill ~8 ~-2 ~ ~4 ~-2 ~ chain_command_block[facing=west]"},{id:command_block_minecart,Command:"setblock ~8 ~-2 ~-1 chain_command_block[facing=south]"},{id:command_block_minecart,Command:"setblock ~3 ~-2 ~-1 repeating_command_block[facing=east]"},{id:command_block_minecart,Command:"data merge block ~6 ~-1 ~2 {auto:1,Command:\"scoreboard players reset @a B_MD\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-1 ~2 {auto:1,Command:\"scoreboard players reset @a B_MA\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-1 ~2 {auto:1,Command:\"scoreboard players reset @a B_MJ\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-1 ~2 {auto:1,Command:\"scoreboard players reset @a B_MS\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~2 {auto:1,Command:\"scoreboard players reset @a B_MB\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~2 {auto:1,Command:\"scoreboard players reset @a B_MO\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~2 {auto:1,Command:\"tag @e remove B_TF2\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~2 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~ ~-1 run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~2 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~ ~-1 if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~8 ~-2 ~2 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~ ~1 run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~8 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~ ~1 if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~-1 ~ ~ run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~-1 ~ ~ if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~1 ~ ~ run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~1 ~ ~ if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~1 {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~-1 ~ run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~ {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~-1 ~ if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~ {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~1 ~ run tag @e[distance=..1,type=item,sort=nearest,limit=1,tag=!B_TF2,nbt={Age:0s}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~ {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s positioned ~ ~1 ~ if block ~ ~ ~ #minecraft:logs run setblock ~ ~ ~ air destroy\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~ {auto:1,Command:\"execute as @e[tag=B_TF2,type=item] at @s as @p at @s run playsound minecraft:block.note_block.bell master @s ~ ~ ~ 1 1 1\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~ {auto:1,Command:\"tag @e remove B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~8 ~-2 ~ {auto:1,Command:\"tag @e[tag=B_TF] add B_TF2\"}"},{id:command_block_minecart,Command:"data merge block ~8 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MD=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:dark_oak_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~7 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MA=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:acacia_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~6 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MJ=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:jungle_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~5 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MS=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:spruce_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~4 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MB=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:birch_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"data merge block ~3 ~-2 ~-1 {auto:1,Command:\"execute as @a[scores={B_MO=1..}] at @s run tag @e[distance=..7,type=item,nbt={Age:0s,Item:{id:\\\"minecraft:oak_log\\\"}}] add B_TF\"}"},{id:command_block_minecart,Command:"setblock ~ ~ ~1 command_block{Command:\"fill ~ ~-2 ~-1 ~ ~ ~ air\"}"},{id:command_block_minecart,Command:"setblock ~ ~-1 ~1 redstone_block"},{id:command_block_minecart,Command:"kill @e[type=command_block_minecart,distance=..1]"}]}]})";
    /* Last result: 7 msecs per iteration (total: 58, iterations: 8) */
    QTest::addRow("Power Carpets by electroman") <<
        R"(summon falling_block ~ ~1 ~ {Time:1,BlockState:{Name:redstone_block},Passengers:[ {id:armor_stand,Health:0,Passengers:[ {id:falling_block,Time:1,BlockState:{Name:activator_rail},Passengers:[ {id:command_block_minecart,Command:'gamerule commandBlockOutput false'}, {id:command_block_minecart,Command:'data merge block ~ ~-3 ~ {auto:0}'}, {id:command_block_minecart,Command:'fill ~-1 ~-2 ~2 ~2 ~-2 ~8 white_concrete'}, {id:command_block_minecart,Command:'fill ~-1 ~ ~2 ~2 ~ ~8 white_concrete'}, {id:command_block_minecart,Command:'fill ~-1 ~ ~8 ~2 ~-2 ~8 white_concrete'}, {id:command_block_minecart,Command:'fill ~-1 ~ ~2 ~2 ~-2 ~2 white_concrete'}, {id:command_block_minecart,Command:'setblock ~0 ~-1 ~3 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:gray_carpet run effect give @s minecraft:weakness 1 2"}'}, {id:command_block_minecart,Command:'setblock ~-1 ~-1 ~3 gray_wool'}, {id:command_block_minecart,Command:'setblock ~-2 ~-1 ~3 lever[facing=west]'}, {id:command_block_minecart,Command:'setblock ~-2 ~-2 ~3 gray_carpet'}, {id:command_block_minecart,Command:'setblock ~0 ~-1 ~4 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:blue_carpet run effect give @s minecraft:water_breathing 10 2"}'}, {id:command_block_minecart,Command:'setblock ~-1 ~-1 ~4 blue_wool'}, {id:command_block_minecart,Command:'setblock ~-2 ~-1 ~4 lever[facing=west]'}, {id:command_block_minecart,Command:'setblock ~-2 ~-2 ~4 blue_carpet'}, {id:command_block_minecart,Command:'setblock ~0 ~-1 ~5 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:purple_carpet run effect give @s minecraft:nausea 4 1"}'}, {id:command_block_minecart,Command:'setblock ~-1 ~-1 ~5 purple_wool'}, {id:command_block_minecart,Command:'setblock ~-2 ~-1 ~5 lever[facing=west]'}, {id:command_block_minecart,Command:'setblock ~-2 ~-2 ~5 purple_carpet'}, {id:command_block_minecart,Command:'setblock ~0 ~-1 ~6 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:brown_carpet run effect give @s minecraft:mining_fatigue 1 2"}'}, {id:command_block_minecart,Command:'setblock ~-1 ~-1 ~6 brown_wool'}, {id:command_block_minecart,Command:'setblock ~-2 ~-1 ~6 lever[facing=west]'}, {id:command_block_minecart,Command:'setblock ~-2 ~-2 ~6 brown_carpet'}, {id:command_block_minecart,Command:'setblock ~0 ~-1 ~7 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:yellow_carpet run effect give @s minecraft:haste 1 2"}'}, {id:command_block_minecart,Command:'setblock ~-1 ~-1 ~7 yellow_wool'}, {id:command_block_minecart,Command:'setblock ~-2 ~-1 ~7 lever[facing=west]'}, {id:command_block_minecart,Command:'setblock ~-2 ~-2 ~7 yellow_carpet'}, {id:command_block_minecart,Command:'setblock ~1 ~-1 ~3 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:white_carpet run effect give @s minecraft:levitation 5 2"}'}, {id:command_block_minecart,Command:'setblock ~2 ~-1 ~3 white_wool'}, {id:command_block_minecart,Command:'setblock ~3 ~-1 ~3 lever[facing=east]'}, {id:command_block_minecart,Command:'setblock ~3 ~-2 ~3 white_carpet'}, {id:command_block_minecart,Command:'setblock ~1 ~-1 ~4 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:pink_carpet run effect give @s minecraft:regeneration 1 2"}'}, {id:command_block_minecart,Command:'setblock ~2 ~-1 ~4 pink_wool'}, {id:command_block_minecart,Command:'setblock ~3 ~-1 ~4 lever[facing=east]'}, {id:command_block_minecart,Command:'setblock ~3 ~-2 ~4 pink_carpet'}, {id:command_block_minecart,Command:'setblock ~1 ~-1 ~5 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:light_blue_carpet run effect give @s minecraft:speed 1 2"}'}, {id:command_block_minecart,Command:'setblock ~2 ~-1 ~5 light_blue_wool'}, {id:command_block_minecart,Command:'setblock ~3 ~-1 ~5 lever[facing=east]'}, {id:command_block_minecart,Command:'setblock ~3 ~-2 ~5 light_blue_carpet'}, {id:command_block_minecart,Command:'setblock ~1 ~-1 ~6 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:lime_carpet run effect give @s minecraft:jump_boost 1 2"}'}, {id:command_block_minecart,Command:'setblock ~2 ~-1 ~6 lime_wool'}, {id:command_block_minecart,Command:'setblock ~3 ~-1 ~6 lever[facing=east]'}, {id:command_block_minecart,Command:'setblock ~3 ~-2 ~6 lime_carpet'}, {id:command_block_minecart,Command:'setblock ~1 ~-1 ~7 repeating_command_block{auto:0,Command:"execute as @a at @a if block ~ ~ ~ minecraft:black_carpet run effect give @s minecraft:blindness 2 2"}'}, {id:command_block_minecart,Command:'setblock ~2 ~-1 ~7 black_wool'}, {id:command_block_minecart,Command:'setblock ~3 ~-1 ~7 lever[facing=east]'}, {id:command_block_minecart,Command:'setblock ~3 ~-2 ~7 black_carpet'}, {id:command_block_minecart,Command:'setblock ~ ~-2 ~ air'}, {id:command_block_minecart,Command:'setblock ~ ~1 ~ command_block{auto:1,Command:"fill ~ ~ ~ ~ ~-2 ~ air"}'},{id:command_block_minecart,Command:'kill @e[type=command_block_minecart,distance=..1]'}]}]}]})";
}

void TestMinecraftParser::benchmarkCommandBoxes() {
    QFETCH(QString, command);

    MinecraftParser           parser(this, command);
    QSharedPointer<ParseNode> result = nullptr;
    QBENCHMARK {
        result = parser.parse();
    }
    Q_ASSERT(result);
    if (strlen(parser.lastError().what()) > 0)
        qDebug() << parser.lastError().toLocalizedMessage();
    QVERIFY(result->isVaild());
}

QTEST_MAIN(TestMinecraftParser)

#include "tst_testminecraftparser.moc"
