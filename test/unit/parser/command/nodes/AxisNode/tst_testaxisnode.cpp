#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/axisnode.h"

using namespace Command;

class TestAxisNode : public QObject
{
    Q_OBJECT

public:
    TestAxisNode();
    ~TestAxisNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void type();
    void value();
};

TestAxisNode::TestAxisNode() {
}

TestAxisNode::~TestAxisNode() {
}

void TestAxisNode::initTestCase() {
}

void TestAxisNode::cleanupTestCase() {
}

void TestAxisNode::test_case1() {
    AxisNode node(this, 0, 4, AxisNode::AxisType::Relative, 1);


    QCOMPARE(node.type(), AxisNode::AxisType::Relative);
    QCOMPARE(node.toInt(), 1);
    QCOMPARE(node.useInteger(), true);
}

void TestAxisNode::type() {
    AxisNode node(this, 0, 3, AxisNode::AxisType::Absolute, 11);

    QCOMPARE(node.format(), "11");

    node.setType(AxisNode::AxisType::Relative);
    QCOMPARE(node.format(), "~11");

    node.setType(AxisNode::AxisType::Local);
    QCOMPARE(node.format(), "^11");
}

void TestAxisNode::value() {
    AxisNode node(this, 0, 3, AxisNode::AxisType::Relative, 3.14);

    QCOMPARE(node.toFloat(), 3.14f);
    qDebug() << node.toInt();
}

QTEST_MAIN(TestAxisNode)

#include "tst_testaxisnode.moc"
