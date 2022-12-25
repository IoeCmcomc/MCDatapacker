#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/nbtnodes.h"

using namespace Command;

class TestNbtNodes : public QObject
{
    Q_OBJECT

public:
    TestNbtNodes();
    ~TestNbtNodes();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void NbtIntNode();
    void NbtByteArrayNode();
    void NbtListNode();
    void NbtCompoundNode();
};

TestNbtNodes::TestNbtNodes() {
}

TestNbtNodes::~TestNbtNodes() {
}

void TestNbtNodes::initTestCase() {
}

void TestNbtNodes::cleanupTestCase() {
}

void TestNbtNodes::test_case1() {
    class NbtIntNode node("123", 123);

    QVERIFY(node.isValid());
    QCOMPARE(node.text(), "123");
}

void TestNbtNodes::NbtIntNode() {
    class NbtIntNode node("-45645245", -45645245);

    QVERIFY(node.isValid());
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::NbtTag);
    QCOMPARE(node.tagType(), NbtNode::TagType::Int);
    QCOMPARE(node.value(), -45645245);
    QCOMPARE(node.text(), "-45645245");
    QCOMPARE(node.length(), 9);
}

void TestNbtNodes::NbtByteArrayNode() {
    class NbtByteArrayNode array(0);

    QVERIFY(array.isValid() == false);

    array.append(QSharedPointer<NbtByteNode>::create("255", (int8_t)255));
    array.append(QSharedPointer<NbtByteNode>::create("45", (int8_t)45));
    const auto &&third =
        QSharedPointer<NbtByteNode>::create("-6", (int8_t)-6);
    array.append(third);

    QCOMPARE(array.kind(), ParseNode::Kind::Argument);
    QCOMPARE(array.parserType(), ArgumentNode::ParserType::NbtTag);
    QCOMPARE(array.tagType(), NbtNode::TagType::ByteArray);
    QCOMPARE(array.length(), 0);
    QCOMPARE(array.size(), 3);
    QCOMPARE(array[2], third);
}

void TestNbtNodes::NbtListNode() {
    class NbtListNode array(0);

    QVERIFY(array.isValid() == false);

    array.append(QSharedPointer<NbtStringNode>::create("first string"));
    auto element = QSharedPointer<NbtStringNode>::create("mcdatapacker:ok");
    array.append(element);

    QCOMPARE(array.kind(), ParseNode::Kind::Argument);
    QCOMPARE(array.parserType(), ArgumentNode::ParserType::NbtTag);
    QCOMPARE(array.tagType(), NbtNode::TagType::List);
    QCOMPARE(array.length(), 0);

    QCOMPARE(array.size(), 2);
    QCOMPARE(array[1], element);
    QCOMPARE(array.prefix(), NbtNode::TagType::String);
}

void TestNbtNodes::NbtCompoundNode() {
    class NbtCompoundNode compound(0);

    QVERIFY(compound.isValid() == false);

    compound.insert(KeyPtr::create("CustomName"),
                    QSharedPointer<NbtStringNode>::create("IoeCmcomc"));
    compound.insert(KeyPtr::create("Air"),
                    QSharedPointer<NbtShortNode>::create("300", 300));

    QCOMPARE(compound.kind(), ParseNode::Kind::Argument);
    QCOMPARE(compound.parserType(), ArgumentNode::ParserType::NbtCompoundTag);
    QCOMPARE(compound.tagType(), NbtNode::TagType::Compound);
    QCOMPARE(compound.length(), 0);

    QCOMPARE(compound.size(), 2);
    QCOMPARE(compound.contains("Air"), true);
}

QTEST_MAIN(TestNbtNodes)

#include "tst_testnbtnodes.moc"
