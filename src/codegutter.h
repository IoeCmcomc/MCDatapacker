#ifndef CODEGUTTER_H
#define CODEGUTTER_H

#include "codeeditor.h"

#include <QWidget>

class CodeEditor;
class LineNumberArea;

class CodeGutter : public QWidget
{
    Q_OBJECT
public:
    explicit CodeGutter(CodeEditor *parent = nullptr);

    CodeEditor *editor() const;

    void updateChildrenGeometries();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    CodeEditor *m_editor             = nullptr;
    LineNumberArea *m_lineNumberArea = nullptr;
};

#endif /* CODEGUTTER_H */
