#ifndef RANGENODE_H
#define RANGENODE_H

#include "argumentnode.h"

#include <QStringList>

#include <optional>

namespace Command {
    template<typename T>
    class RangeNode : public ArgumentNode {
        enum class PrimaryValueRole : unsigned char {
            ExactValueRole,
            MinValueRole,
            MaxValueRole,
        };
public:
        RangeNode(int pos, int length = 0, const QString &parserId = "")
            : ArgumentNode(pos, length, parserId) {
            setExactValue(QSharedPointer<T>::create(pos, length, 0));
        };
        QString format() const {
            if (hasMinValue() || hasMaxValue()) {
                QStringList parts;
                parts <<
                    ((hasMinValue()) ? QString::number(minValue()->value()) : "");
                parts <<
                    ((hasMaxValue()) ? QString::number(maxValue()->value()) : "");
                return parts.join(QStringLiteral(".."));
            } else {
                return QString::number(exactValue()->value());
            }
        };
        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override {
            if (order == NodeVisitor::Order::Preorder)
                visitor->visit(this);
            m_primary->accept(visitor, order);
            if (m_secondary)
                m_secondary->accept(visitor, order);
            if (order == NodeVisitor::Order::Postorder)
                visitor->visit(this);
        }

        void setExactValue(QSharedPointer<T> value) {
            m_primary        = value;
            m_primaryValRole = PrimaryValueRole::ExactValueRole;
            if (m_secondary)
                m_secondary = nullptr;
        };
        QSharedPointer<T> exactValue() const {
            if (m_primaryValRole == PrimaryValueRole::ExactValueRole) {
                return m_primary;
            } else {
                return nullptr;
            }
        };

        void setMinValue(QSharedPointer<T> value, bool keepMax) {
            if (keepMax) {
                if (!m_secondary)
                    m_secondary = m_primary;
            } else {
                if (m_secondary)
                    m_secondary = nullptr;
            }
            m_primary        = value;
            m_primaryValRole = PrimaryValueRole::MinValueRole;
        };
        QSharedPointer<T> minValue() const {
            if (m_primaryValRole == PrimaryValueRole::MinValueRole) {
                return m_primary;
            } else {
                return nullptr;
            }
        };
        inline bool hasMinValue() const {
            return m_primaryValRole == PrimaryValueRole::MinValueRole;
        };

        void setMaxValue(QSharedPointer<T> value, bool keepMin) {
            if (keepMin) {
                m_secondary      = value;
                m_primaryValRole = PrimaryValueRole::MinValueRole;
            } else {
                if (m_secondary)
                    m_secondary = nullptr;
                m_primary        = value;
                m_primaryValRole = PrimaryValueRole::MaxValueRole;
            }
        };
        QSharedPointer<T> maxValue() const {
            switch (m_primaryValRole) {
            case PrimaryValueRole::MinValueRole:
                return m_secondary;

            case PrimaryValueRole::MaxValueRole:
                return m_primary;

            case PrimaryValueRole::ExactValueRole:
                return nullptr;
            }
            return nullptr;
        };
        inline bool hasMaxValue() const {
            return m_secondary ||
                   (m_primaryValRole == PrimaryValueRole::MaxValueRole);
        };

protected:
        QSharedPointer<T> primary() const {
            return m_primary;
        }
        QSharedPointer<T> secondary() const {
            return m_secondary;
        }
private:
        QSharedPointer<T> m_secondary = nullptr;
        QSharedPointer<T> m_primary   = nullptr;
        PrimaryValueRole m_primaryValRole;
    };
}

#endif /* RANGENODE_H */
