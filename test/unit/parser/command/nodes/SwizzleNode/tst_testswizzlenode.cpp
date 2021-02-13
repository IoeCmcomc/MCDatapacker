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
    void isVaild();
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
    SwizzleNode node(this, 0, true, true, true);

    QCOMPARE(node.toString(), "SwizzleNode(xyz)");
}

void TestSwizzleNode::isVaild() {
    SwizzleNode node(this, 0, false, false, false);

    QCOMPARE(node.isVaild(), false);
    node.setAxes(SwizzleNode::Axis::X | SwizzleNode::Axis::Y);
    QCOMPARE(node.isVaild(), true);
}

void TestSwizzleNode::length() {
    SwizzleNode node(this, 0, false, false, true);

    QCOMPARE(node.length(), 1);
    node.setAxes(SwizzleNode::Axis::X | SwizzleNode::Axis::Y);
    QCOMPARE(node.length(), 2);
    qDebug() << node;
}

QTEST_MAIN(TestSwizzleNode)

#include "tst_testswizzlenode.moc"
