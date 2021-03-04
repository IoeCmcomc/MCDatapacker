#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/uuidnode.h"

using namespace Command;

class TestUuidNode : public QObject
{
    Q_OBJECT

public:
    TestUuidNode();
    ~TestUuidNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void isVaild();
};

TestUuidNode::TestUuidNode() {
}

TestUuidNode::~TestUuidNode() {
}

void TestUuidNode::initTestCase() {
}

void TestUuidNode::cleanupTestCase() {
}

void TestUuidNode::isVaild() {
    UuidNode node(2021, "xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx");

    QCOMPARE(node.isVaild(), false);
    node.setUuid(QUuid("123e4567-e89b-12d3-a456-426614174000"));
    QCOMPARE(node.isVaild(), true);
}

void TestUuidNode::test_case1() {
    UuidNode node(2, "dd12be42-52a9-4a91-a8a1-11c01849e498");

    QCOMPARE(node.toString(), "UuidNode(dd12be42-52a9-4a91-a8a1-11c01849e498)");
}

QTEST_MAIN(TestUuidNode)

#include "tst_testuuidnode.moc"
