#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/stringnode.h"

using namespace Command;

class TestStringNode : public QObject
{
    Q_OBJECT

public:
    TestStringNode();
    ~TestStringNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TestStringNode::TestStringNode() {
}

TestStringNode::~TestStringNode() {
}

void TestStringNode::initTestCase() {
}

void TestStringNode::cleanupTestCase() {
}

void TestStringNode::test_case1() {
    StringNode node(this, 51,
                    "Although a long argument type is present in brigadier, it is not used by Minecraft.");

    QCOMPARE(
        node.toString(),
        "StringNode(\"Although a long argument type is present in brigadier, it is not used by Minecraft.\")");
}

QTEST_MAIN(TestStringNode)

#include "tst_teststringnode.moc"
