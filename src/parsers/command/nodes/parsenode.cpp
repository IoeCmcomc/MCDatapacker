#include "parsenode.h"
#include "../visitors/nodevisitor.h"

#include <QDebug>

namespace Command {
    bool ParseNode::isValid() const {
        return m_isValid;
    }

    DEFINE_ACCEPT_METHOD(ParseNode)

    ParseNode::Kind ParseNode::kind() const {
        return m_kind;
    }

    bool ParseNode::hasText() const {
        return std::holds_alternative<QString>(m_span);
    }

    QString ParseNode::text() const {
        return (hasText()) ? std::get<QString>(m_span) : QString();
    }

    int ParseNode::length() const {
        return (hasText()) ? std::get<QString>(m_span).length() : std::get<int>(
            m_span);
    }

    void ParseNode::setText(const QString &text) {
        m_span = text;
    }

    const Schema::Node * ParseNode::schemaNode() const {
        return m_schemaNode;
    }

    void ParseNode::setSchemaNode(const Schema::Node *newSchemaNode) {
        m_schemaNode = newSchemaNode;
    }

    void ParseNode::setLength(int length) {
        m_span = length;
    }

    void ParseNode::setIsValid(bool newIsValid) {
        m_isValid = newIsValid;
    }

    QString ParseNode::leadingTrivia() const {
        return m_leadingTrivia;
    }

    void ParseNode::setLeadingTrivia(const QString &newLeadingTrivia) {
        m_leadingTrivia = newLeadingTrivia;
    }

    QString ParseNode::trailingTrivia() const {
        return m_trailingTrivia;
    }

    void ParseNode::setTrailingTrivia(const QString &newTrailingTrivia) {
        m_trailingTrivia = newTrailingTrivia;
    }

    QString ParseNode::leftText() const {
        return m_left;
    }

    void ParseNode::setLeftText(const QString &newLeft) {
        m_left = newLeft;
    }

    QString ParseNode::rightText() const {
        return m_right;
    }

    void ParseNode::setRightText(const QString &newRight) {
        m_right = newRight;
    }

    ParseNode::ParseNode(Kind kind) : m_kind(kind), m_span{0} {
    }

    ParseNode::ParseNode(Kind kind, int length) : m_kind(kind),
        m_span{length} {
    }

    ParseNode::ParseNode(Kind kind, const QString &text) : m_kind(kind),
        m_span{text} {
    }

    DEFINE_ACCEPT_METHOD(ErrorNode)
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "ParseNode(" << node.length() << ")";
    return debug;
}
