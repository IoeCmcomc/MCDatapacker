#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/rootnode.h"

using namespace Command;

class TestRootNode : public QObject
{
    Q_OBJECT

public:
    TestRootNode();
    ~TestRootNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void append();
    void remove();
    void isEmpty();
    void size();
};

TestRootNode::TestRootNode() {
}

TestRootNode::~TestRootNode() {
}

void TestRootNode::initTestCase() {
}

void TestRootNode::cleanupTestCase() {
}

void TestRootNode::test_case1() {
    RootNode root(this);

    QVERIFY(root.toString() == "RootNode[0]()");
}

void TestRootNode::append() {
    RootNode root(this);
    auto    *node = new ParseNode(this);

    root.append(node);
    QCOMPARE(root.toString(), "RootNode[1](ParseNode())");

    root << new ParseNode(this);

    QCOMPARE(root.toString(), "RootNode[2](ParseNode(), ParseNode())");

    root.append(new ParseNode(this));

    QCOMPARE(root.toString(),
             "RootNode[3](ParseNode(), ParseNode(), ParseNode())");

    QCOMPARE(root[0]->toString(), "ParseNode()");
}

void TestRootNode::remove() {
    RootNode root(this);

    root << new ParseNode(this) << new ParseNode(this) << new ParseNode(this);

    QCOMPARE(root.toString(),
             "RootNode[3](ParseNode(), ParseNode(), ParseNode())");

    root.remove(1);

    QCOMPARE(root.toString(), "RootNode[2](ParseNode(), ParseNode())");

    root.remove(0);

    QCOMPARE(root.toString(), "RootNode[1](ParseNode())");
}

void TestRootNode::isEmpty() {
    RootNode root(this);

    QCOMPARE(root.isEmpty(), true);

    root << new ParseNode(this) << new ParseNode(this) << new ParseNode(this);

    QCOMPARE(root.isEmpty(), false);
}

void TestRootNode::size() {
    RootNode root(this);

    QCOMPARE(root.size(), 0);

    root << new ParseNode(this);
    QCOMPARE(root.size(), 1);

    root << new ParseNode(this) << new ParseNode(this) << new ParseNode(this);
    QCOMPARE(root.size(), 4);

    root.remove(3);
    QCOMPARE(root.size(), 3);
}


QTEST_MAIN(TestRootNode)

#include "tst_testrootnode.moc"
