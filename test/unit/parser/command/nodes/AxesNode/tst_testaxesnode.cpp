#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/axesnode.h"

using namespace Command;

class TestAxesNode : public QObject
{
    Q_OBJECT

public:
    TestAxesNode();
    ~TestAxesNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void isVaild();
};

TestAxesNode::TestAxesNode() {
}

TestAxesNode::~TestAxesNode() {
}

void TestAxesNode::initTestCase() {
}

void TestAxesNode::cleanupTestCase() {
}

void TestAxesNode::test_case1() {
    AxesNode node(this, 0, 8);

    node.setX(new AxisNode(this, 0, 2, AxisNode::AxisType::Absolute, 36));
    node.setY(new AxisNode(this, 3, 2, AxisNode::AxisType::Relative, 1));
    node.setZ(new AxisNode(this, 6, 2, AxisNode::AxisType::Absolute, -6));

    QCOMPARE(node.toString(),
             "AxesNode(x: AxisNode(36), y: AxisNode(~1), z: AxisNode(-6))");
}

void TestAxesNode::isVaild() {
    AxesNode node(this, 0, 8);

    QCOMPARE(node.isVaild(), false);

    node.setX(new AxisNode(this, 0, 2, AxisNode::AxisType::Absolute, 36));
    QCOMPARE(node.isVaild(), false);
    node.setZ(new AxisNode(this, 6, 2, AxisNode::AxisType::Absolute, -6));
    QCOMPARE(node.isVaild(), true);
    node.setY(new AxisNode(this, 3, 2, AxisNode::AxisType::Relative, 1));
    QCOMPARE(node.isVaild(), true);
}

QTEST_MAIN(TestAxesNode)

#include "tst_testaxesnode.moc"
