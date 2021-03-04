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
    class NbtIntNode node(0, 3, 123);
    QCOMPARE(node.toString(), "NbtIntNode(123)");
}

void TestNbtNodes::NbtIntNode() {
    class NbtIntNode node(20, 9, -45645245);
    QCOMPARE(node.id(), nbt::tag_id::tag_int);
    QCOMPARE(node.value(), -45645245);
    node.setValue(14022021);
    QCOMPARE(node.value(), 14022021);
}

void TestNbtNodes::NbtByteArrayNode() {
    class NbtByteArrayNode array(5);
    array.append(QSharedPointer<NbtByteNode>::create(5, 3, (char)255));
    array.append(QSharedPointer<NbtByteNode>::create(1, 2, (char)45));
    auto third = QSharedPointer<NbtByteNode>::create(52, 2, (char)-6);
    qDebug() << array;
    array.append(third);
    QCOMPARE(array.size(), 3);
    QCOMPARE(array[2], third);
}

void TestNbtNodes::NbtListNode() {
    class NbtListNode array(5);
    array.append(QSharedPointer<NbtStringNode>::create(5, "first string"));
    auto third = QSharedPointer<NbtStringNode>::create(52, "mcdatapacker:ok");
    array.append(third);
    QCOMPARE(array.size(), 2);
    QCOMPARE(array[1], third);
    QCOMPARE(array.prefix(), nbt::tag_id::tag_string);
    qDebug() << array;
}

void TestNbtNodes::NbtCompoundNode() {
    class NbtCompoundNode compound(21);
    compound.insert(MapKey{ 1, "CustomName" },
                    QSharedPointer<NbtStringNode>::create(12, "IoeCmcomc"));
    compound.insert(MapKey{ 14, "Air" },
                    QSharedPointer<NbtShortNode>::create(18, 4, 300));
    QCOMPARE(compound.size(), 2);
    QCOMPARE(compound.contains("Air"), true);
    qDebug() << compound;
}

QTEST_MAIN(TestNbtNodes)

#include "tst_testnbtnodes.moc"
