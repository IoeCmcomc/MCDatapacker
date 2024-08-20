#include "mcdatapackerwidgets.h"

#include "extendedtablewidget/extendedtablewidgetplugin.h"
#include "numberprovider/numberproviderplugin.h"
#include "dialogdatabutton/dialogdatabuttonplugin.h"
#include "multipagewidget/multipagewidgetplugin.h"
#include "idtagselector/idtagselectorplugin.h"

MCDatapackerWidgets::MCDatapackerWidgets(QObject *parent)
    : QObject(parent) {
    m_widgets << new ExtendedTableWidgetPlugin(this);
    m_widgets << new NumberProviderPlugin(this);
    m_widgets << new DialogDataButtonPlugin(this);
    m_widgets << new MultiPageWidgetPlugin(this);
    m_widgets << new IdTagSelectorPlugin(this);
}

QList<QDesignerCustomWidgetInterface*> MCDatapackerWidgets::customWidgets()
const {
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(mcdatapackerwidgetsplugin, MCDatapackerWidgets)
#endif /* QT_VERSION < 0x050000 */
