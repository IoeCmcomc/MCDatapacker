#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/nodes/resourcelocationnode.h"

using namespace Command;

class TestResourceLocationNode : public QObject
{
    Q_OBJECT

public:
    TestResourceLocationNode();
    ~TestResourceLocationNode();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void general();
    void length();
    void nspace();
    void id();
    void isTag();
};

TestResourceLocationNode::TestResourceLocationNode() {
}

TestResourceLocationNode::~TestResourceLocationNode() {
}

void TestResourceLocationNode::initTestCase() {
}

void TestResourceLocationNode::cleanupTestCase() {
}

void TestResourceLocationNode::general() {
    ResourceLocationNode node(15, SpanPtr::create("minecraft"), SpanPtr::create("apple"));

    QVERIFY(node.isValid() == false);
    QCOMPARE(node.kind(), ParseNode::Kind::Argument);
    QCOMPARE(node.parserType(), ArgumentNode::ParserType::ResourceLocation);
    QCOMPARE(node.hasText(), false);
}

void TestResourceLocationNode::length() {
    ResourceLocationNode node(17, SpanPtr::create("test"), SpanPtr::create("first/second"));

    QCOMPARE(node.length(), 17);
}

void TestResourceLocationNode::nspace() {
    ResourceLocationNode node(17, SpanPtr::create("test"), SpanPtr::create("first/second"));

    QCOMPARE(node.nspace()->text(), SpanPtr::create("test")->text());
}

void TestResourceLocationNode::id() {
    ResourceLocationNode node(25, SpanPtr::create("minecraft"), SpanPtr::create("diamond_pickaxe"));

    QCOMPARE(node.id()->text(), SpanPtr::create("diamond_pickaxe")->text());
}

void TestResourceLocationNode::isTag() {
    ResourceLocationNode node(12, SpanPtr::create("qwerty"), SpanPtr::create("test"));

    QCOMPARE(node.isTag(), false);

    node.setIsTag(true);
    QCOMPARE(node.isTag(), true);
}

QTEST_MAIN(TestResourceLocationNode)

#include "tst_testresourcelocationnode.moc"
