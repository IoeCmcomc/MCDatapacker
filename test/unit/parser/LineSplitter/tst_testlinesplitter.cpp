#include <QtTest>

#include "../../../../src/parsers/linesplitter.h"

class TestLineSplitter : public QObject {
    Q_OBJECT

public:
    TestLineSplitter();
    ~TestLineSplitter();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void helloWorld();
    void helloWorld_logical();
    void lineContinuation_single();
    void lineContinuation_single_ignoreSpaces();
    void lineContinuation_multiple();
    void lineContinuation_multiple_ignoreSpaces();
    void actuallyShort();
};

TestLineSplitter::TestLineSplitter() {
}

TestLineSplitter::~TestLineSplitter() {
}

void TestLineSplitter::initTestCase() {
}

void TestLineSplitter::cleanupTestCase() {
}

void TestLineSplitter::helloWorld() {
    LineSplitter splitter("Hello\nWorld");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"Hello");
    QCOMPARE(splitter.getCurrLine(), "Hello");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"World");
    QCOMPARE(splitter.getCurrLine(), "World");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), QStringView());
    QCOMPARE(splitter.getCurrLine(), QString());
}

void TestLineSplitter::helloWorld_logical() {
    LineSplitter splitter("Hello\nWorld");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), "Hello");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), "World");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

void TestLineSplitter::lineContinuation_single() {
    LineSplitter splitter(R"(execute at @p \
as @s \
say hi)");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), "execute at @p as @s say hi");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

void TestLineSplitter::lineContinuation_single_ignoreSpaces() {
    LineSplitter splitter(
        R"(q wertyuiop\
            a sdfghjkl\
            z xcvbnm)");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), "q wertyuiopa sdfghjklz xcvbnm");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

void TestLineSplitter::lineContinuation_multiple() {
    LineSplitter splitter(R"(qwerty\
uiop
asdf\
ghjkl
zx\
cvbnm)");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"qwerty\\");
    QCOMPARE(splitter.nextLogicalLine(), "qwertyuiop");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"asdf\\");
    QCOMPARE(splitter.nextLogicalLine(), "asdfghjkl");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"zx\\");
    QCOMPARE(splitter.nextLogicalLine(), "zxcvbnm");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

void TestLineSplitter::lineContinuation_multiple_ignoreSpaces() {
    LineSplitter splitter(
        R"(qwerty\
                                uiop
asdf\
                                ghjkl
zx\
                            cvbnm)");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"qwerty\\");
    QCOMPARE(splitter.nextLogicalLine(), "qwertyuiop");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"asdf\\");
    QCOMPARE(splitter.nextLogicalLine(), "asdfghjkl");
    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.peekCurrLineView(), u"zx\\");
    QCOMPARE(splitter.nextLogicalLine(), "zxcvbnm");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

void TestLineSplitter::actuallyShort() {
    LineSplitter splitter(
        R"(foo\
\
\
\
\
\
\
\
\
\
bar)");

    QVERIFY(splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), "foobar");
    QVERIFY(!splitter.hasNextLine());
    QCOMPARE(splitter.nextLogicalLine(), QString());
}

QTEST_APPLESS_MAIN(TestLineSplitter)

#include "tst_testlinesplitter.moc"
