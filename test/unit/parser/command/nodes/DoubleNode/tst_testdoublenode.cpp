#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/singlevaluenode.h"

using namespace Command;

class TestDoubleNode : public QObject
{
    Q_OBJECT

public:
    TestDoubleNode();
    ~TestDoubleNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void general();
};

TestDoubleNode::TestDoubleNode() {
}

TestDoubleNode::~TestDoubleNode() {
}

void TestDoubleNode::initTestCase() {
}

void TestDoubleNode::cleanupTestCase() {
}

void TestDoubleNode::general() {
    DoubleNode node("22.4", 22.4, true);

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::Double);
    QCOMPARE(node.text(), "22.4");
    QCOMPARE(node.length(), 4);
    QCOMPARE(node.value(), 22.4);
}

QTEST_GUILESS_MAIN(TestDoubleNode)

#include "tst_testdoublenode.moc"
