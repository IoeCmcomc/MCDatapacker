#include "mcbuildhighlighter.h"

McbuildHighlighter::McbuildHighlighter(QTextDocument *parent,
                                       const bool isMacroFile)
    : Highlighter{parent} {
    bracketPairs.append({ '(', ')' });
    initBracketCharset();
    m_singleCommentChar = QLatin1Char('#');

    const QRegularExpression commandRegex{
        QStringLiteral(
            R"((?<=^| )(?>(?>a)(?>dvancement|ttribute)|b(?>an(?>-ip|list|)|ossbar)|cl(?>ear|one)|d(?>ata(?:pack)?|e(?>bug|faultgamemode|op)|ifficulty)|e(?>chant|ffect|x(?>ecute|perience))|f(?>ill|orceload|unction)|g(?>ame(?>mode|rule)|ive)|help|item|jfr|ki(?>ck|ll)|l(?>ist|o(?>cate(?:biome)?|ot))|m(?>e|sg)|op|p(?>ar(?>don(?:-ip)?|ticle)|erf|laysound|ublish)|re(?>cipe|load|placeitem)|s(?>a(?>ve-(?>all|o(?>ff|n))|y)|c(?>hedule|oreboard)|e(?>ed|t(?>block|idletimeout|worldspawn))|p(?>awnpoint|ectate|readplayer)|top(?:sound)?|ummon)|t(?>ag|e(?>am(?:msg)?|l(?>eport|l(?:raw)?))|i(?>me|tle)|m|p|rigger)|w(?>eather|hitelist|orldborder|)|xp)(?= |$|;))") };
    m_rules.append({ commandRegex, CodePalette::CmdLiteral });

    const QRegularExpression numberRegex{
        QStringLiteral(R"((?<!\w)-?\d+(?:\.\d+)?[bBsSlLfFdD]?(?!\w))") };
    m_rules.append({ numberRegex, CodePalette::Number });

    const QRegularExpression keywordRegex{
        QStringLiteral(
            R"(\b(?>!IF|LOOP|async|block|clock|dir|else|f(?>inally|unction)|import|macro|until|while)\b)")
    };
    m_rules.append({ keywordRegex, CodePalette::Keyword });

    m_rules.append({
        QRegularExpression(QStringLiteral(R"(\B@(?>a|e|p|r|s)\b)")),
        CodePalette::TargetSelector_Variable });
    if (isMacroFile) {
        m_rules.append({
            QRegularExpression(QStringLiteral(R"(\$\$\d+)")),
            CodePalette::String });
    }
    m_rules.append({
        QRegularExpression(QStringLiteral(R"(function +(\w+) +{)")),
        CodePalette::Function });
    m_rules.append({ QRegularExpression(QStringLiteral(R"(<%.+?%>)")),
                     CodePalette::Script });
}

void McbuildHighlighter::highlightBlock(const QString &text) {
    highlightUsingRules(text, m_rules);
    Highlighter::highlightBlock(text);

    static auto multilineCommentDelimiter = QStringLiteral("###");

    setCurrentBlockState(Normal);

    // -2: Multiline
    // -1: Not found
    int startIndex = -2;
    if (previousBlockState() != MultilineComment)
        startIndex = text.indexOf(multilineCommentDelimiter);

    while (startIndex != -1) {
        startIndex = (startIndex == -2) ? 0 : startIndex + 3;
        const int endIndex = text.indexOf(multilineCommentDelimiter,
                                          startIndex);
        int stringLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(MultilineComment);
            stringLength = text.length() - startIndex;
        } else {
            stringLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, stringLength,
                  m_palette[CodePalette::Comment]);
        startIndex = text.indexOf(multilineCommentDelimiter,
                                  startIndex + stringLength);
    }
}
