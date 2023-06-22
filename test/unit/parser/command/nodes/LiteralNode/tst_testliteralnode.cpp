#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/literalnode.h"

using namespace Command;

class TestLiteralNode : public QObject
{
    Q_OBJECT

public:
    TestLiteralNode();
    ~TestLiteralNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void general();
    void invalid();
};

TestLiteralNode::TestLiteralNode() {
}

TestLiteralNode::~TestLiteralNode() {
}

void TestLiteralNode::initTestCase() {
}

void TestLiteralNode::cleanupTestCase() {
}

void TestLiteralNode::general() {
    LiteralNode node("test_case1");

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Literal);
    QCOMPARE(node.hasText(), true);
    QCOMPARE(node.text(), "test_case1");
    QCOMPARE(node.length(), 10);
}

void TestLiteralNode::invalid() {
    LiteralNode node("invalid");

    node.setIsValid(false);

    QVERIFY(node.isValid() == false);
}

QTEST_GUILESS_MAIN(TestLiteralNode)

#include "tst_testliteralnode.moc"
