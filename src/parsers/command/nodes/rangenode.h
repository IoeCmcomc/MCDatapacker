#ifndef RANGENODE_H
#define RANGENODE_H

#include "argumentnode.h"

#include <QStringList>

#include <optional>

namespace Command {
    template<typename T>
    class RangeNode : public ArgumentNode {
        enum class PrimaryValueRole : uint8_t {
            ExactValueRole,
            MinValueRole,
            MaxValueRole,
        };

public:
        QString format() const {
            if (hasMinValue() || hasMaxValue()) {
                QStringList parts;
                parts <<
                    ((hasMinValue()) ? QString::number(minValue()->value()) :
                     QString());
                parts <<
                    ((hasMaxValue()) ? QString::number(maxValue()->value()) :
                     QString());
                return parts.join(QStringLiteral(".."));
            } else {
                return QString::number(exactValue()->value());
            }
        };

        template <typename _T = T>
        static constexpr bool is_same_pointer_type = std::is_same_v<std::decay_t<_T>,
                                                                    QSharedPointer<T> >;

        template <typename _T = T>
        typename std::enable_if_t<is_same_pointer_type<_T> >
        setExactValue(_T &&value) {
            m_isValid        = value->isValid();
            m_primary        = std::forward<_T>(value);
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

        template <typename _T = T>
        typename std::enable_if_t<is_same_pointer_type<_T> >
        setMinValue(_T &&value, bool keepMax) {
            if (m_secondary) {
                m_isValid &= value->isValid();
            } else {
                m_isValid = value->isValid();
            }
            if (keepMax) {
                if (!m_secondary) {
                    m_secondary = std::move(m_primary);
                }
            } else {
                if (m_secondary) {
                    m_secondary = nullptr;
                }
            }
            m_primary        = std::forward<_T>(value);
            m_primaryValRole = PrimaryValueRole::MinValueRole;
        };
        QSharedPointer<T> minValue() const {
            return hasMinValue() ? m_primary : nullptr;
        };
        inline bool hasMinValue() const {
            return m_primaryValRole == PrimaryValueRole::MinValueRole;
        };

        template <typename _T = T>
        typename std::enable_if_t<is_same_pointer_type<_T> >
        setMaxValue(_T &&value, bool keepMin) {
            if (m_secondary) {
                m_isValid &= value->isValid();
            } else {
                m_isValid = value->isValid();
            }
            if (keepMin) {
                m_secondary      = std::forward<_T>(value);
                m_primaryValRole = PrimaryValueRole::MinValueRole;
            } else {
                if (m_secondary) {
                    m_secondary = nullptr;
                }
                m_primary        = std::forward<_T>(value);
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
        using ArgumentNode::ArgumentNode;

        QSharedPointer<T> primary() const {
            return m_primary;
        }
        QSharedPointer<T> secondary() const {
            return m_secondary;
        }

        RangeNode(ParserType parserType, int length)
            : ArgumentNode(parserType, length) {
            setExactValue(QSharedPointer<T>::create(QString(), 0));
            m_primary->setIsValid(false);
        };
private:
        QSharedPointer<T> m_secondary;
        QSharedPointer<T> m_primary;
        PrimaryValueRole m_primaryValRole;
    };
}

#endif /* RANGENODE_H */
