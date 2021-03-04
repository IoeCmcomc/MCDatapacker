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
    AxesNode node(0, 8);

    node.setX(QSharedPointer<AxisNode>::create(0, 2,
                                               AxisNode::AxisType::Absolute,
                                               36));
    node.setY(QSharedPointer<AxisNode>::create(3, 2,
                                               AxisNode::AxisType::Relative,
                                               1));
    node.setZ(QSharedPointer<AxisNode>::create(6, 2,
                                               AxisNode::AxisType::Absolute,
                                               -6));

    QCOMPARE(node.toString(),
             "AxesNode(x: AxisNode(36), y: AxisNode(~1), z: AxisNode(-6))");
}

void TestAxesNode::isVaild() {
    AxesNode node(0, 8);

    QCOMPARE(node.isVaild(), false);

    node.setX(QSharedPointer<AxisNode>::create(0, 2,
                                               AxisNode::AxisType::Absolute,
                                               36));
    QCOMPARE(node.isVaild(), false);
    node.setZ(QSharedPointer<AxisNode>::create(6, 2,
                                               AxisNode::AxisType::Absolute,
                                               -6));
    QCOMPARE(node.isVaild(), true);
    node.setY(QSharedPointer<AxisNode>::create(3, 2,
                                               AxisNode::AxisType::Relative,
                                               1));
    QCOMPARE(node.isVaild(), true);
}

QTEST_MAIN(TestAxesNode)

#include "tst_testaxesnode.moc"
