#ifndef TRUEFALSEBOX_H
#define TRUEFALSEBOX_H

#include <QCheckBox>
#include <QJsonObject>

class TrueFalseBox : public QCheckBox
{
    Q_OBJECT
public:
    TrueFalseBox(QWidget *parent = nullptr);

    void insertToJsonObject(QJsonObject &obj, const QString &key);
    void setupFromJsonObject(QJsonObject &obj, const QString &key);

protected:
    void nextCheckState() override;
};

#endif /* TRUEFALSEBOX_H */
