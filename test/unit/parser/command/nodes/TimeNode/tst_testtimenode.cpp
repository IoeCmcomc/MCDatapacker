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
    void general();
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

void TestTimeUnit::general() {
    TimeNode node("123s", 123, TimeNode::Unit::Second);

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::Time);
    QCOMPARE(node.unit(), TimeNode::Unit::Second);
    QCOMPARE(node.text(), "123s");
    QCOMPARE(node.value(), 123);
    QCOMPARE(node.length(), 4);
}

void TestTimeUnit::toTick() {
    TimeNode node("6000", 6000);

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
