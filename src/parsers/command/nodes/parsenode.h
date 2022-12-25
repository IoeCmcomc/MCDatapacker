#ifndef PARSENODE_H
#define PARSENODE_H

#include <QDebug>

namespace Command {
    class NodeVisitor;
    enum VisitOrder: int;

    class ParseNode {
public:
        enum class Kind: signed char {
            Error = -1,
            Span, // Act as a token, not visitable
            File,
            Root,
            Literal,
            Argument,
            Container, // Contains two or more nodes
        };

        using Span = std::variant<int, QString>;

        virtual bool isValid() const;
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

protected:
        QString m_left;
        QString m_right;
        QString m_leadingTrivia;
        QString m_trailingTrivia;
        Kind m_kind    = Kind::Span;
        bool m_isValid = false;

        explicit ParseNode(Kind kind) : m_kind(kind), m_span{0} {
        }
        explicit ParseNode(Kind kind, int length) : m_kind(kind),
            m_span{length} {
        }
        explicit ParseNode(Kind kind, const QString &text) : m_kind(kind),
            m_span{text} {
        }

        void setText(const QString &text);

private:
        Span m_span = 0;
    };

    using NodePtr = QSharedPointer<ParseNode>;

    class ErrorNode : public ParseNode {
public:
        explicit ErrorNode(const QString &text) : ParseNode(Kind::Error, text) {
        };

        void accept(NodeVisitor *visitor, VisitOrder) override;
    };

    class SpanNode : public ParseNode {
public:
        explicit SpanNode(const QString &text) : ParseNode(Kind::Span, text) {
        };
    };
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);

Q_DECLARE_METATYPE(Command::NodePtr);
Q_DECLARE_METATYPE(QSharedPointer<Command::ErrorNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::SpanNode>);

template <typename T>
struct TypeRegister {
    static bool init() {
        qRegisterMetaType<QSharedPointer<T> >();
        QMetaType::registerConverter<QSharedPointer<T>, Command::NodePtr >();
        return true;
    };
};

#define DEFINE_ACCEPT_METHOD(Type)                             \
        void Type::accept(NodeVisitor * visitor, VisitOrder) { \
            visitor->visit(this);                              \
        }                                                      \

#endif /* PARSENODE_H */
