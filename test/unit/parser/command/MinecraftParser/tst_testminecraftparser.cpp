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
    void functionCmd();
    void forceloadCmd();
    void tpCmd();
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

void TestMinecraftParser::functionCmd() {
    MinecraftParser parser(this, "function custom:example/test");

    auto *result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[2](LiteralNode(function), FunctionNode(custom:example/test))");
}

void TestMinecraftParser::forceloadCmd() {
    MinecraftParser parser(this, "forceload add 0 -1 31 47");

    auto *result = parser.parse();

    QCOMPARE(
        result->toString(),
        "RootNode[4](LiteralNode(forceload), LiteralNode(add), ColumnPosNode(x: AxisNode(0), z: AxisNode(-1)), ColumnPosNode(x: AxisNode(31), z: AxisNode(47)))");

    parser.setText("forceload query 3 5");

    result = parser.parse();
    qDebug() << *result;
    QCOMPARE(
        result->toString(),
        "RootNode[3](LiteralNode(forceload), LiteralNode(query), ColumnPosNode(x: AxisNode(3), z: AxisNode(5)))");
}

void TestMinecraftParser::tpCmd() {
    MinecraftParser parser(this, "tp ~ ~1 ~");

    auto *result = parser.parse();

    qDebug() << *result;
}

QTEST_MAIN(TestMinecraftParser)

#include "tst_testminecraftparser.moc"
