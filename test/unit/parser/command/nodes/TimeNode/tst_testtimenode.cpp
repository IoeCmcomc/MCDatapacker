#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/timenode.h"

using namespace Command;

class TestTimeUnit : public QObject
{
    Q_OBJECT

public:
    TestTimeUnit();
    ~TestTimeUnit();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void toTick();
};

TestTimeUnit::TestTimeUnit() {
}

TestTimeUnit::~TestTimeUnit() {
}

void TestTimeUnit::initTestCase() {
}

void TestTimeUnit::cleanupTestCase() {
}

void TestTimeUnit::test_case1() {
    TimeNode node(64, 4, 6000);

    QCOMPARE(node.toString(), "TimeNode(6000)");
}

void TestTimeUnit::toTick() {
    TimeNode node(64, 4, 6000);

    QCOMPARE(node.toTick(), 6000);
    node.setUnit(TimeNode::Unit::Second);
    node.setValue(59);
    QCOMPARE(node.toTick(), 1180);
    node.setUnit(TimeNode::Unit::Day);
    node.setValue(.5);
    QCOMPARE(node.toTick(), 12000);
}

QTEST_MAIN(TestTimeUnit)

#include "tst_testtimenode.moc"
