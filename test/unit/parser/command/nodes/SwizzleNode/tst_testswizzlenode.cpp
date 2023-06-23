#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/swizzlenode.h"

using namespace Command;

class TestSwizzleNode : public QObject
{
    Q_OBJECT

public:
    TestSwizzleNode();
    ~TestSwizzleNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void isValid();
    void length();
};

TestSwizzleNode::TestSwizzleNode() {
}

TestSwizzleNode::~TestSwizzleNode() {
}

void TestSwizzleNode::initTestCase() {
}

void TestSwizzleNode::cleanupTestCase() {
}

void TestSwizzleNode::test_case1() {
    SwizzleNode node("xyz", true, true, true);

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::Swizzle);
    QCOMPARE(node.text(), "xyz");
    QCOMPARE(node.length(), 3);
}

void TestSwizzleNode::isValid() {
    SwizzleNode node("xy", false, false, false);

    QCOMPARE(node.isValid(), false);
    node.setAxes(SwizzleNode::Axis::X | SwizzleNode::Axis::Y);
    QCOMPARE(node.isValid(), true);
}

void TestSwizzleNode::length() {
    SwizzleNode node("zy", false, true, true);

    QCOMPARE(node.length(), 2);
}

QTEST_MAIN(TestSwizzleNode)

#include "tst_testswizzlenode.moc"
