#ifndef PARSENODE_H
#define PARSENODE_H

#include <QSharedPointer>

#include <variant>

namespace Command {
    class NodeVisitor;
    enum VisitOrder: int;
    namespace Schema {
        class Node;
    }

    class ParseNode {
public:
        enum class Kind: signed char {
            Unknown = -1,
            Error,
            Span, // Act as a token, not visitable
            File,
            Macro,
            MacroVariable,
            Root,
            Literal,
            Container, // Contains two or more nodes
            Argument,
        };

        using Span = std::variant<int, QString>;

        bool isValid() const;
        virtual void accept(NodeVisitor *visitor, VisitOrder order);

        Kind kind() const;

        bool hasText() const;
        QString text() const;
        int length() const;

        void setLength(int length);
        void setIsValid(bool newIsValid);

        QString leadingTrivia() const;
        void setLeadingTrivia(const QString &newLeadingTrivia);

        QString trailingTrivia() const;
        void setTrailingTrivia(const QString &newTrailingTrivia);

        QString leftText() const;
        void setLeftText(const QString &newLeft);

        QString rightText() const;
        void setRightText(const QString &newRight);

        const Schema::Node * schemaNode() const;
        void setSchemaNode(const Schema::Node *newSchemaNode);

protected:
        QString m_left;
        QString m_right;
        QString m_leadingTrivia;
        QString m_trailingTrivia;
        Kind m_kind    = Kind::Span;
        bool m_isValid = false;

        explicit ParseNode(Kind kind);
        explicit ParseNode(Kind kind, int length);
        explicit ParseNode(Kind kind, const QString &text);

        void setText(const QString &text);

private:
        Span m_span                      = 0;
        const Schema::Node *m_schemaNode = nullptr;
    };

    using NodePtr = QSharedPointer<ParseNode>;

    class ErrorNode : public ParseNode {
public:
        explicit ErrorNode(const QString &text) : ParseNode(Kind::Error, text) {
        };

        void accept(NodeVisitor *visitor, VisitOrder) final;
    };

    class SpanNode : public ParseNode {
public:
        explicit SpanNode(const QString &text, const bool valid = false)
            : ParseNode(Kind::Span, text) {
            m_isValid = valid;
        };
    };

    using SpanPtr = QSharedPointer<SpanNode>;

    template <class T, typename E>
    constexpr E nodeTypeEnum;

    template <class T>
    constexpr ParseNode::Kind nodeTypeEnum<T, ParseNode::Kind> =
        ParseNode::Kind::Unknown;

    #define DECLARE_TYPE_ENUM_FULL(Class, Enum, EnumValue)              \
            template <>                                                 \
            constexpr Enum nodeTypeEnum<Class, Enum> = Enum::EnumValue; \

#define DECLARE_TYPE_ENUM(Enum, EnumValue) \
        DECLARE_TYPE_ENUM_FULL(EnumValue ## Node, Enum, EnumValue)

    DECLARE_TYPE_ENUM(ParseNode::Kind, Error)
    DECLARE_TYPE_ENUM(ParseNode::Kind, Span)
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);
#ifdef PARSENODE_REPRPRINTER
QDebug operator<<(QDebug debug, Command::ParseNode *node);
#endif

#define DEFINE_ACCEPT_METHOD(Type)                             \
        void Type::accept(NodeVisitor * visitor, VisitOrder) { \
            visitor->visit(this);                              \
        }                                                      \

#endif /* PARSENODE_H */
