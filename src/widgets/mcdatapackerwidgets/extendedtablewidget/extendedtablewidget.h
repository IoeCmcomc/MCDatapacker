#ifdef _MSC_VER
#define __clang_major__    15
#endif

#ifndef EXTENDEDTABLEWIDGET_H
#define EXTENDEDTABLEWIDGET_H

#include <QWidget>
#include <QVersionNumber>


QT_BEGIN_NAMESPACE
class QTableWidget;
class QFrame;
class QToolButton;
class QVBoxLayout;
class QComboBox;
QT_END_NAMESPACE

namespace Ui {
    class ExtendedTableWidget;
}

class ExtendedTableWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        JsonMode jsonMode READ jsonMode WRITE setJsonMode NOTIFY jsonModeChanged)
    Q_PROPERTY(QTableWidget * tableWidget
               READ tableWidget WRITE setTableWidget NOTIFY tableWidgetChanged)
    Q_PROPERTY(QFrame * container
               READ container WRITE setContainer NOTIFY containerChanged)
    Q_PROPERTY(bool isAddingItem
               READ isAddingItem WRITE setAddingItem NOTIFY addingItemChanged STORED false)
    Q_PROPERTY(QStringList columnTitles
               READ columnTitles WRITE setColumnTitles NOTIFY columnTitlesChanged)

public:
    enum class JsonMode {
        SimpleMap,
        ComplexMap,
        List,
    };
    Q_ENUM(JsonMode)
    enum Enum_JsonMode { // Make JsonMode enum accessible to moc compiler
        SimpleMap  = (int)JsonMode::SimpleMap,
        ComplexMap = (int)JsonMode::ComplexMap,
        List       = (int)JsonMode::List,
    };

    enum class EditorClass {
        Unknown = 0,
        NumberProvider,
        QSpinBox,
        QLineEdit,
        QComboBox,
        QCheckBox,
    };

    enum ItemRole {
        ComboboxDataRole = Qt::UserRole + 1,
        ComboboxIndexRole,
        NumberProviderRole,
    };

    using VersionPair = QPair<QVersionNumber, QVersionNumber>;

    struct ColumnMapping {
        QString     jsonKey;
        QWidget    *editor        = nullptr;
        EditorClass editorClass   = EditorClass::Unknown;
        VersionPair gameVerLimits = {};
    };

    ExtendedTableWidget(QWidget *parent = 0);

    QTableWidget * tableWidget() const;
    QFrame * container() const;

    bool isAddingItem() const;
    QStringList columnTitles() const;
    int rowCount() const;

    void appendColumnMapping(const QString &jsonKey, QWidget *editor,
                             VersionPair gameVerLim = {});

    JsonMode jsonMode() const;
    QJsonObject toJsonObject() const;
    QJsonArray toJsonArray() const;
    void fromJson(const QJsonObject &root);
    void fromJson(const QJsonArray &root);

    /*static void initComboBoxData(QComboBox *widget, ) */

signals:
    void jsonModeChanged();
    void tableWidgetChanged();
    void containerChanged();
    void addingItemChanged();
    void columnTitlesChanged();

public slots:
    void setContainer(QFrame *widget);
    void setTableWidget(QTableWidget *widget);
    void setAddingItem(bool isAddingItem);
    void setColumnTitles(const QStringList &columnTitles);
    void setJsonMode(const ExtendedTableWidget::JsonMode &jsonMode);
    void setJsonMode(const Enum_JsonMode &mode) {
        setJsonMode(static_cast<JsonMode>(mode));
    }
    void setGameVersion(const QVersionNumber &version);
    void clear();

protected:
    void changeEvent(QEvent *e) override;

private:
    Ui::ExtendedTableWidget *ui;

    QStringList m_columnTitles;
    QVector<ColumnMapping> m_columnMappings;
    QVersionNumber m_gameVersion;
    QVBoxLayout *m_layout = nullptr;
    JsonMode m_jsonMode   = JsonMode::SimpleMap;
    bool m_isAddingItem   = false;

    void retranslateUi();
    QJsonValue itemDataToJson(int row, int col) const;
    int indexOfKeyInMapping(const QString &key) const;
    void loadItemFromJson(int row, int col, const QJsonValue &value);

private slots:
    void onAddBtn();
    void onRemoveBtn();
    void onApplyBtn();
    void onCancelBtn();
    void updateRemoveBtn();
};

typedef ExtendedTableWidget::ItemRole ExtendedRole;

#endif /* EXTENDEDTABLEWIDGET_H */
