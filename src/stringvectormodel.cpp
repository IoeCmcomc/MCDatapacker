#include "stringvectormodel.h"

StringVectorModel::StringVectorModel(QObject *parent)
    : QAbstractListModel{parent} {
}

StringVectorModel::StringVectorModel(const StringVector &vec, QObject *parent)
    : QAbstractListModel{parent}, m_vector{vec} {
}

int StringVectorModel::rowCount(const QModelIndex &parent) const {
    return m_vector.size();
}

int StringVectorModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

QVariant StringVectorModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_vector.value(index.row(), QString());
    } else {
        return {};
    }
}

StringVector StringVectorModel::vector() const {
    return m_vector;
}

void StringVectorModel::setVector(const StringVector &newVector) {
    beginResetModel();
    m_vector = newVector;
    endResetModel();
}
