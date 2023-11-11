#include "jmchighlighter.h"


JmcHighlighter::JmcHighlighter(QTextDocument *parent) : Highlighter{parent} {
    m_singleCommentCharset += QStringLiteral("#");
    m_quoteDelimiters      += QStringLiteral("'");
    bracketPairs.append({ '(', ')' });

    const QRegularExpression commandRegex{
        QStringLiteral(
            R"((?<=^| )(?>(?>a)(?>dvancement|ttribute)|b(?>an(?>-ip|list|)|ossbar)|cl(?>ear|one)|d(?>ata(?:pack)?|e(?>bug|faultgamemode|op)|ifficulty)|e(?>chant|ffect|x(?>ecute|perience))|f(?>ill|orceload|unction)|g(?>ame(?>mode|rule)|ive)|help|item|jfr|ki(?>ck|ll)|l(?>ist|o(?>cate(?:biome)?|ot))|m(?>e|sg)|op|p(?>ar(?>don(?:-ip)?|ticle)|erf|laysound|ublish)|re(?>cipe|load|placeitem)|s(?>a(?>ve-(?>all|o(?>ff|n))|y)|c(?>hedule|oreboard)|e(?>ed|t(?>block|idletimeout|worldspawn))|p(?>awnpoint|ectate|readplayer)|top(?:sound)?|ummon)|t(?>ag|e(?>am(?:msg)?|l(?>eport|l(?:raw)?))|i(?>me|tle)|m|p|rigger)|w(?>eather|hitelist|orldborder|)|xp)(?= |$|;))") };
    m_rules.append({ commandRegex, CodePalette::CmdLiteral });

    const QRegularExpression directiveRegex{
        QStringLiteral(
            R"(^#(?>bind|c(?>ommand|redit)|de(?>fine|l)|include|nometa|override|static|uninstall)\b)")
    };
    m_rules.append({ directiveRegex, CodePalette::Directive });

    const QRegularExpression operatorRegex{
        QStringLiteral(
            R"((?>!|%=|\&\&|\*=|\+(?>\+|=)|\-(?>\-|=|>)|\/=|<(?:=)?|=|>(?>|<|=)|\?(?>=|\?=)|\|\|))")
    };
    m_rules.append({ operatorRegex, CodePalette::Operator });

    const QRegularExpression numberRegex{
        QStringLiteral(R"((?<!\w)-?\d+(?:\.\d+)?[bBsSlLfFdD]?(?!\w))") };
    m_rules.append({ numberRegex, CodePalette::Number });

    const QRegularExpression keywordRegex{
        QStringLiteral(
            R"(\b(?>c(?>ase|lass)|do|else|f(?>or|unction)|i(?>f|mport)|new|s(?>chedule|witch)|while)\b)")
    };
    m_rules.append({ keywordRegex, CodePalette::Keyword });

    m_rules.append({
        QRegularExpression(R"(false)"), CodePalette::Bool_False });
    m_rules.append({
        QRegularExpression(R"(true)"), CodePalette::Bool_True });
    m_rules.append({
        QRegularExpression(QStringLiteral(R"(\$[\w$~^#@.\\]+)"),
                           QRegularExpression::UseUnicodePropertiesOption),
        CodePalette::String });
    m_rules.append({
        QRegularExpression(QStringLiteral(R"(\B@(?>a|e|p|r|s)\b)")),
        CodePalette::TargetSelector_Variable });
    m_rules.append({
        QRegularExpression(QStringLiteral(R"(function +(\w+)\()")),
        CodePalette::Function });
    m_rules.append({
        QRegularExpression(QStringLiteral(R"(\/\/.+)")),
        CodePalette::Comment });
}

void JmcHighlighter::highlightBlock(const QString &text) {
    highlightUsingRules(text, m_rules);
    Highlighter::highlightBlock(text);

    static char multilineStringDelimiter{ '`' };

    setCurrentBlockState(Normal);

    int startIndex = 0;
    if (previousBlockState() != MultilineString)
        startIndex = text.indexOf(multilineStringDelimiter);

    while (startIndex >= 0) {
        const int endIndex = text.indexOf(multilineStringDelimiter,
                                          startIndex + 1);
        int stringLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(MultilineString);
            stringLength = text.length() - startIndex;
        } else {
            stringLength = endIndex - startIndex + 1;
        }
        setFormat(startIndex, stringLength,
                  m_palette[CodePalette::QuotedString]);
        startIndex = text.indexOf(multilineStringDelimiter,
                                  startIndex + stringLength);
    }
}
