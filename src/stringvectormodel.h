#ifndef STRINGVECTORMODEL_H
#define STRINGVECTORMODEL_H

#include <QAbstractListModel>

using StringVector = QVector<QString>;

class StringVectorModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit StringVectorModel(
        QObject *parent = nullptr);
    explicit StringVectorModel(const StringVector &vec,
                               QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent    = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const final;

    StringVector vector() const;
    void setVector(const StringVector &newVector);

private:
    StringVector m_vector;
};

#endif // STRINGVECTORMODEL_H
