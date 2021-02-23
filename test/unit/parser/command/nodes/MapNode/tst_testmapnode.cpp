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

    QCOMPARE(node.size(), 0);;
    node.insert(MapKey{ 2, "test" }, new ParseNode(this, 0, 5));
    node.insert(MapKey{ 1, "first" }, new ParseNode(this, 0, 5));

    auto map = node.toMap();
    auto it  = map.cbegin();

    while (it != map.cend()) {
        qDebug() << it.key().text << *it.value();
        ++it;
    }
    QCOMPARE(map.size(), 2);
    QCOMPARE(node.toString(),
             "MapNode(first: ParseNode(), test: ParseNode())");
}

QTEST_MAIN(TestMapNode)

#include "tst_testmapnode.moc"
