#ifndef MCDATAPACKER_PCH_H
#define MCDATAPACKER_PCH_H

// Add C includes here


#if defined __cplusplus
// Add C++ includes here

// Qt includes
#include "QtCore/qsystemdetection.h"
#include "QtCore/qatomic.h"
#include <QtGlobal>
#include <QString>
#include <QStringView>
#include <QVector>
#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QHash>
#include <QSharedPointer>

//#include <QObject>

// Third-party includes
#include "nlohmann/json.hpp"
#include "lru/lru.hpp"

#endif

#endif // MCDATAPACKER_PCH_H
