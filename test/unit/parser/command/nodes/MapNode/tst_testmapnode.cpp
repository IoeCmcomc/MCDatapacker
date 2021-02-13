#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/mapnode.h"

using namespace Command;

class TestMapNode : public QObject
{
    Q_OBJECT

public:
    TestMapNode();
    ~TestMapNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestMapNode::TestMapNode() {
}

TestMapNode::~TestMapNode() {
}

void TestMapNode::initTestCase() {
}

void TestMapNode::cleanupTestCase() {
}

void TestMapNode::test_case1() {
    MapNode node(this, 0, 999);

    QCOMPARE(node.size(), 0);
    node.insert("test", new ParseNode(this, 0, 5));
    node["second"] = new ParseNode(this, 23, 6);
    QCOMPARE(node.size(), 2);
    QCOMPARE(node.toString(),
             "MapNode(second: ParseNode(), test: ParseNode())");
}

QTEST_MAIN(TestMapNode)

#include "tst_testmapnode.moc"
