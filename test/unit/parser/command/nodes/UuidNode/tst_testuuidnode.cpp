#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/singlevaluenode.h"

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
    void general();
    void invalid();
};

TestUuidNode::TestUuidNode() {
}

TestUuidNode::~TestUuidNode() {
}

void TestUuidNode::initTestCase() {
}

void TestUuidNode::cleanupTestCase() {
}

void TestUuidNode::invalid() {
    UuidNode node("xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx", QUuid("xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx"));

    QCOMPARE(node.isValid(), true);
    QCOMPARE(node.value().isNull(), true);

    node.setIsValid(false);
    QCOMPARE(node.isValid(), false);
}

void TestUuidNode::general() {
    UuidNode node("dd12be42-52a9-4a91-a8a1-11c01849e498", QUuid("dd12be42-52a9-4a91-a8a1-11c01849e498"));

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::Uuid);
    QCOMPARE(node.value(), QUuid("dd12be42-52a9-4a91-a8a1-11c01849e498"));
    QCOMPARE(node.text(), "dd12be42-52a9-4a91-a8a1-11c01849e498");
    QCOMPARE(node.length(), 36);
}

QTEST_MAIN(TestUuidNode)

#include "tst_testuuidnode.moc"
