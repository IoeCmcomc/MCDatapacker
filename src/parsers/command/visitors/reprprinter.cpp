#include "reprprinter.h"

namespace Command {
    ReprPrinter::ReprPrinter() : NodeVisitor(LetTheVisitorDecide) {
    }

    QString ReprPrinter::repr() const {
        return m_repr;
    }

    void ReprPrinter::reprResourceLocation(ResourceLocationNode *node,
                                           bool autoClose) {
        m_repr += '(';

        if (node->isTag())
            m_repr += '#';
        if (node->nspace() && !node->nspace()->text().isEmpty()) {
            m_repr += node->nspace()->text();
            m_repr += ':';
            m_repr += node->id()->text();
        } else {
            if (node->id()->text().isEmpty()) {
                m_repr += ':';
            } else {
                m_repr += node->id()->text();
            }
        }

        if (autoClose)
            m_repr += ')';
    }

    void ReprPrinter::reprEntityNode(EntityNode *node) {
        m_repr += '[';
        if (node->singleOnly() && node->playerOnly()) {
            m_repr += "single, player";
        } else if (node->playerOnly()) {
            m_repr += "player";
        } else if (node->singleOnly()) {
            m_repr += "single";
        }
        m_repr += ']';
        m_repr += '(';
        node->getNode()->accept(this, m_order);
        m_repr += ')';
    }
}
