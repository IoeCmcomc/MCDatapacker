#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/similarstringnodes.h"

using namespace Command;

class TestSimilarStringNode : public QObject
{
    Q_OBJECT

public:
    TestSimilarStringNode();
    ~TestSimilarStringNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void colorNode();
};

TestSimilarStringNode::TestSimilarStringNode() {
}

TestSimilarStringNode::~TestSimilarStringNode() {
}

void TestSimilarStringNode::initTestCase() {
}

void TestSimilarStringNode::cleanupTestCase() {
}

void TestSimilarStringNode::colorNode() {
    ColorNode node(this, 1, "green");

    QCOMPARE(node.value(), "green");
}

QTEST_MAIN(TestSimilarStringNode)

#include "tst_testsimilarstringnode.moc"
