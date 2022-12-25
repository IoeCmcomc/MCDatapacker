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
    RootNode root(0);

    QVERIFY(root.isValid() == false);
    QCOMPARE(root.kind(), ParseNode::Kind::Root);
    QCOMPARE(root.length(), 0);
    QCOMPARE(root.size(), 0);
}

void TestRootNode::append() {
    RootNode root(0);
    auto     node = QSharedPointer<SpanNode>::create("span");

    root.append(node);
    QCOMPARE(root.size(), 1);

    root.append(QSharedPointer<SpanNode>::create("span"));

    QCOMPARE(root.size(), 2);

    root.append(QSharedPointer<SpanNode>::create(""));

    QCOMPARE(root.size(), 3);
}

void TestRootNode::isEmpty() {
    RootNode root(0);

    QCOMPARE(root.isEmpty(), true);

    root.append(QSharedPointer<SpanNode>::create(""));
    root.append(QSharedPointer<SpanNode>::create(""));
    root.append(QSharedPointer<SpanNode>::create(""));

    QCOMPARE(root.isEmpty(), false);
}

void TestRootNode::size() {
    RootNode root(0);

    QCOMPARE(root.size(), 0);

    root.append(QSharedPointer<SpanNode>::create(""));
    QCOMPARE(root.size(), 1);

    root.append(QSharedPointer<SpanNode>::create(""));
    root.append(QSharedPointer<SpanNode>::create(""));
    root.append(QSharedPointer<SpanNode>::create(""));
    QCOMPARE(root.size(), 4);
}


QTEST_MAIN(TestRootNode)

#include "tst_testrootnode.moc"
