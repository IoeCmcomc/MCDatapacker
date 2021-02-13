#ifndef RANGENODE_H
#define RANGENODE_H

#include <QStringList>

#include <optional>

namespace Command {
    template<typename T>
    class RangeNode {
        enum class PrimaryValueRole : unsigned char {
            ExactValueRole,
            MinValueRole,
            MaxValueRole,
        };
public:
        RangeNode() = default;
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

        void setExactValue(T *value) {
            m_primary        = value;
            m_primaryValRole = PrimaryValueRole::ExactValueRole;
            if (m_secondary)
                delete m_secondary;
        };
        T *exactValue() const {
            if (m_primaryValRole == PrimaryValueRole::ExactValueRole) {
                return m_primary;
            } else {
                return nullptr;
            }
        };

        void setMinValue(T *value, bool keepMax) {
            if (keepMax) {
                if (!m_secondary)
                    m_secondary = m_primary;
            } else {
                if (m_secondary)
                    delete m_secondary;
            }
            m_primary        = value;
            m_primaryValRole = PrimaryValueRole::MinValueRole;
        };
        T *minValue() const {
            if (m_primaryValRole == PrimaryValueRole::MinValueRole) {
                return m_primary;
            } else {
                return nullptr;
            }
        };
        inline bool hasMinValue() const {
            return m_primaryValRole == PrimaryValueRole::MinValueRole;
        };

        void setMaxValue(T *value, bool keepMin) {
            if (keepMin) {
                m_secondary      = value;
                m_primaryValRole = PrimaryValueRole::MinValueRole;
            } else {
                if (m_secondary)
                    delete m_secondary;
                m_primary        = value;
                m_primaryValRole = PrimaryValueRole::MaxValueRole;
            }
        };
        T *maxValue() const {
            switch (m_primaryValRole) {
            case PrimaryValueRole::MinValueRole:
                return m_secondary;

            case PrimaryValueRole::MaxValueRole:
                return m_primary;

            case PrimaryValueRole::ExactValueRole:
                return nullptr;
            }
        };
        inline bool hasMaxValue() const {
            return m_secondary ||
                   (m_primaryValRole == PrimaryValueRole::MaxValueRole);
        };
private:
        T *m_secondary = nullptr;
        T *m_primary   = nullptr;
        PrimaryValueRole m_primaryValRole;
    };
}

#endif /* RANGENODE_H */
