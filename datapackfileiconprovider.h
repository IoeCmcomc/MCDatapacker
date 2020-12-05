#ifndef DATAPACKFILEICONPROVIDER_H
#define DATAPACKFILEICONPROVIDER_H

#include <QFileIconProvider>

class DatapackFileIconProvider : public QFileIconProvider
{
public:
    DatapackFileIconProvider();

    QIcon icon(const QFileInfo &info) const;
};

#endif /* DATAPACKFILEICONPROVIDER_H */
