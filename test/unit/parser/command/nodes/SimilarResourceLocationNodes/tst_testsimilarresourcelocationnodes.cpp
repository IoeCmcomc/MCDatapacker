#include <QtTest>
#include <QCoreApplication>

#include \
    "../../../../../../src/parsers/command/nodes/similarresourcelocationnodes.h"

using namespace Command;

class TestSimilarResourceLocationNodes : public QObject
{
    Q_OBJECT

public:
    TestSimilarResourceLocationNodes();
    ~TestSimilarResourceLocationNodes();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void dimensionNode();
};

TestSimilarResourceLocationNodes::TestSimilarResourceLocationNodes() {
}

TestSimilarResourceLocationNodes::~TestSimilarResourceLocationNodes() {
}

void TestSimilarResourceLocationNodes::initTestCase() {
}

void TestSimilarResourceLocationNodes::cleanupTestCase() {
}

void TestSimilarResourceLocationNodes::dimensionNode() {
    DimensionNode node(this, 99, "minecraft", "nether");

    QCOMPARE(node.fullId(), "minecraft:nether");
}

QTEST_MAIN(TestSimilarResourceLocationNodes)

#include "tst_testsimilarresourcelocationnodes.moc"
