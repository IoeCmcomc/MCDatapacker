#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/similaraxesnodes.h"

using namespace Command;

class TestSimilarAxesNodes : public QObject
{
    Q_OBJECT

public:
    TestSimilarAxesNodes();
    ~TestSimilarAxesNodes();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void columnPosNode();
};

TestSimilarAxesNodes::TestSimilarAxesNodes() {
}

TestSimilarAxesNodes::~TestSimilarAxesNodes() {
}

void TestSimilarAxesNodes::initTestCase() {
}

void TestSimilarAxesNodes::cleanupTestCase() {
}

void TestSimilarAxesNodes::columnPosNode() {
    ColumnPosNode node(0, 8);

    node.setX(QSharedPointer<AxisNode>::create(0, 2,
                                               AxisNode::AxisType::Absolute,
                                               12));
    node.setZ(QSharedPointer<AxisNode>::create(6, 2,
                                               AxisNode::AxisType::Absolute,
                                               56));

    QCOMPARE(node.toString(),
             "ColumnPosNode(x: AxisNode(12), z: AxisNode(56))");
}

QTEST_MAIN(TestSimilarAxesNodes)

#include "tst_testsimilaraxesnodes.moc"
