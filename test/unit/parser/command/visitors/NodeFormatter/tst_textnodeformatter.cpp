#include <QtTest>
#include <QCoreApplication>

#include "../../../../../../src/parsers/command/minecraftparser.h"
#include "../../../../../../src/parsers/command/visitors/nodeformatter.h"

using namespace Command;

class TextNodeFormatter : public QObject
{
    Q_OBJECT

public:
    TextNodeFormatter();
    ~TextNodeFormatter();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

TextNodeFormatter::TextNodeFormatter() {
}

TextNodeFormatter::~TextNodeFormatter() {
}

void TextNodeFormatter::initTestCase() {
    MinecraftParser::setGameVer(QVersionNumber(1, 15));
}

void TextNodeFormatter::cleanupTestCase() {
}

void TextNodeFormatter::test_case1() {
/*
      MinecraftParser parser(this,
                             "execute as @p at @s if entity @e[type=zombie] if block ~1 ~ ~2 water anchored eyes unless score @s test = @s test run clear @s minecraft:apple 13");
 */
    MinecraftParser parser(this,
                           "execute as @p run clear");
    QSharedPointer<ParseNode> result = nullptr;

    result = parser.parse();

    qDebug() << *result;

    auto *root = static_cast<RootNode *>(result.get());

    Q_ASSERT(root);
    qDebug() << *root;

    auto *execute = static_cast<LiteralNode *>((*root)[0].get());

    Q_ASSERT(execute);
    qDebug() << *execute;

    auto *player = dynamic_cast<ComponentNode *>((*root)[2].get());

    if (player) {
        qDebug() << *player;
        player->setValue("test");
    }

    NodeFormatter formatter;
    formatter.startVisiting(result.get());
    auto ranges = formatter.formatRanges();
    qDebug() << ranges.length();
}

QTEST_MAIN(TextNodeFormatter)

#include "tst_textnodeformatter.moc"
