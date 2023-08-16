#include "argumentnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    void ArgumentNode::accept(Command::NodeVisitor *visitor,
                              Command::VisitOrder) {
        visitor->visit(this);
    }

    ArgumentNode::ParserType ArgumentNode::parserType() const {
        return m_parserType;
    }

    QVector<QString> toStringVec(const QLatin1StringVector &vector) {
        QVector<QString> newVec{ vector.size() };

        std::transform(vector.cbegin(), vector.cend(), newVec.begin(),
                       [](const auto &str) {
            return str;
        });
        return newVec;
    }
}
