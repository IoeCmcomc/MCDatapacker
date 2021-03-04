#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/resourcelocationnode.h"

using namespace Command;

class TestResourceLocationNode : public QObject
{
    Q_OBJECT

public:
    TestResourceLocationNode();
    ~TestResourceLocationNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void fullId();
    void nspace();
    void id();
    void isTag();
};

TestResourceLocationNode::TestResourceLocationNode() {
}

TestResourceLocationNode::~TestResourceLocationNode() {
}

void TestResourceLocationNode::initTestCase() {
}

void TestResourceLocationNode::cleanupTestCase() {
}

void TestResourceLocationNode::test_case1() {
    ResourceLocationNode node(5, "minecraft", "apple");

    QCOMPARE(node.toString(), "ResourceLocationNode(minecraft:apple)");
}

void TestResourceLocationNode::fullId() {
    ResourceLocationNode node(5, "test", "first/second");

    QCOMPARE(node.fullId(), "test:first/second");
}

void TestResourceLocationNode::nspace() {
    ResourceLocationNode node(5, "test", "first/second");

    QCOMPARE(node.nspace(), "test");

    node.setNspace("mod");
    QCOMPARE(node.nspace(), "mod");
}

void TestResourceLocationNode::id() {
    ResourceLocationNode node(5, "minecraft", "diamond_pickaxe");

    QCOMPARE(node.id(), "diamond_pickaxe");

    node.setId("netherite_pickaxe");
    QCOMPARE(node.id(), "netherite_pickaxe");
}

void TestResourceLocationNode::isTag() {
    ResourceLocationNode node(5, "qwerty", "test");

    QCOMPARE(node.isTag(), false);

    node.setIsTag(true);
    QCOMPARE(node.isTag(), true);
}

QTEST_MAIN(TestResourceLocationNode)

#include "tst_testresourcelocationnode.moc"
