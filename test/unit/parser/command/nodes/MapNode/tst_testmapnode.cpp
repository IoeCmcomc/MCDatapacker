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
    MapNode node(0);

    QVERIFY(node.isValid() == true);

    QCOMPARE(node.kind(), ParseNode::Kind::Container);
    QCOMPARE(node.size(), 0);

    node.insert(KeyPtr::create("same"),
                QSharedPointer<SpanNode>::create("first"));

    QVERIFY(node.contains("same"));

    node.insert(KeyPtr::create("key"),
                QSharedPointer<SpanNode>::create("value"));
    node.insert(KeyPtr::create("same"),
                QSharedPointer<SpanNode>::create("second"));

    QCOMPARE(node.size(), 3);
    QCOMPARE(node.find("same")->get()->second->text(), "first");
}

QTEST_MAIN(TestMapNode)

#include "tst_testmapnode.moc"
