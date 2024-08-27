#ifndef IDTAGSELECTOR_H
#define IDTAGSELECTOR_H

#include "dialogdatabutton.h"

#include <QFrame>
#include <QMenu>
#include <QJsonValue>
#include <QVersionNumber>

QT_BEGIN_HEADER
class QAbstractListModel;
QT_END_HEADER

namespace Ui {
    class IdTagSelector;
}

class IdTagSelector : public QFrame {
    Q_OBJECT

    Q_PROPERTY(Modes modes READ getModes WRITE setModes NOTIFY modesChanged)
    Q_PROPERTY(
        Mode currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(
        QString idDisplayName READ idDisplayName WRITE setIdDisplayName NOTIFY idDisplayNameChanged)

public:
    explicit IdTagSelector(QWidget *parent = nullptr);
    ~IdTagSelector();

    enum Mode: uint8_t {
        Id        = 1,
        Tag       = 2,
        ListOfIds = 4,
    };
    Q_ENUM(Mode);
    Q_DECLARE_FLAGS(Modes, Mode)
    Q_FLAG(Modes)

    void fromJson(const QJsonValue &value);
    QJsonValue toJson();

    Modes getModes() const;
    void setModes(const Modes &value);
    Mode currentMode() const;
    void setCurrentMode(const Mode &value);

    QString idDisplayName() const;
    void setIdDisplayName(const QString &newIdDisplayName);

    void setIdModel(QAbstractListModel *model);
    void setTagModel(QAbstractListModel *model);

    bool hasData() const;

    QString id() const;
    void setId(const QString &id);

    QString tag() const;
    void setTag(const QString &tag);

    QJsonArray ids() const;
    void setIds(const QJsonArray &ids);

signals:
    void editingFinished();
    void modesChanged(Modes modes);
    void currentModeChanged(Mode mode);
    void idDisplayNameChanged(const QString &name);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void dataBtnClicked();
    void listDataChanged(const QVariant &value);
    void onTypeComboIndexChanged(const int index);
    void onTextEdited(const QString &text);

private:
    friend class IdTagSelectorDelegate;

    Ui::IdTagSelector *ui;
    QString m_idDisplayName;
    QAbstractListModel *m_idModel;
    QAbstractListModel *m_tagModel;
    Modes m_modes;
    Mode m_currentMode = Id;

    DialogDataButton * dataBtn();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IdTagSelector::Modes)

#endif /* IDTAGSELECTOR_H */
