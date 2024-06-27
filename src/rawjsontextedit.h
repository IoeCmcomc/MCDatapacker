#ifndef RAWJSONTEXTEDIT_H
#define RAWJSONTEXTEDIT_H

#include "lru/lru.hpp"

#include <QTextEdit>
#include <QTextFragment>
#include <QPainterPath>
#include <QTimer>
#include <QAbstractTextDocumentLayout>
#include <QRubberBand>

struct Subfragment {
    QRectF  rect;
    QString text;
};
static_assert(qIsRelocatable<Subfragment>() == false);
Q_DECLARE_TYPEINFO(Subfragment, Q_RELOCATABLE_TYPE);

using Subfragments = QVector<Subfragment>;

struct FragmentRegion {
    QTextFragment fragment;
    Subfragments  subfragments;

    inline QVariant formatProperty(int propId) const {
        return fragment.charFormat().penProperty(propId);
    }
};

struct BorderPath {
    QPainterPath path;
    QPen         pen;

    bool inline operator==(const BorderPath &o) const {
        return (path == o.path) && (pen == o.pen);
    }
};

inline const int boldCharMask = 32768;

struct RenderChar {
    QChar ch;
    bool  bold;

    bool inline operator==(const RenderChar &o) const {
        return (ch == o.ch) && (bold == o.bold);
    }
};

using FormatRange = QTextLayout::FormatRange;
Q_DECLARE_METATYPE(QTextLayout::FormatRange)

QDataStream &operator<<(QDataStream & out,
                        const QTextLayout::FormatRange & obj);
QDataStream &operator>>(QDataStream &in, QTextLayout::FormatRange &obj);

namespace std {
//    template<> struct hash<QChar> {
//        using argument_type = QChar;
//        using result_type   = std::size_t;

//        result_type operator()(argument_type const& key) const {
//            return key.unicode();
//        }
//    };

    template<> struct hash<RenderChar> {
        using argument_type = RenderChar;
        using result_type   = std::size_t;

        result_type operator()(argument_type const& key) const {
            return key.ch.unicode() | (boldCharMask * key.bold);
        }
    };
}

class RawJsonTextEdit : public QTextEdit {
    Q_OBJECT
public:
    enum FormatProperty {
        BorderProperty = QTextCharFormat::UserProperty + 0x10,
    };

    enum TextObject {
        _begin = QTextFormat::UserObject + 1,
        Translate,
        Scoreboard,
        EntityNames,
        Keybind,
        Nbt,
        _end,
    };

    using FragmentRegions = QVector<FragmentRegion>;
    using BorderPaths     = QVector<BorderPath>;

    explicit RawJsonTextEdit(QWidget *parent = nullptr);
    bool joinBorders() const;
    void setJoinBorders(bool newJoinBorders);

    bool renderObfuscation() const;
    void setRenderObfuscation(bool newRenderObfuscation);

    bool editTextObject(QTextCharFormat &fmt, QTextCursor cursor);
    bool isSelectingObject(const QTextCursor &cursor) const;

signals:
    void textObjectSelected();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    QMimeData * createMimeDataFromSelection() const override;
    void insertFromMimeData(const QMimeData *source) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    friend class RawJsonTextObjectInterface;

    const QStringView m_obfuscatedCharset =
        u"(!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~£ƒªº¬«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀∅∈≡±≥≤⌠⌡÷≈°∙·√ⁿ²■";

    FragmentRegions m_fragRegions;
    BorderPaths m_borderPaths;
    LRU::Cache<RenderChar, int> m_charWidths;
    QMap<int, QString> m_similarWidthCharGroups;
    QTimer *m_obfuscatedTimer     = new QTimer(this);
    QTimer *m_updateRegionsTimer  = new QTimer(this);
    QTimer *m_blinkCursorTimer    = new QTimer(this);
    QTextDocument *m_tempDoc      = nullptr;
    QTextDocument *m_renderingDoc = nullptr;
    bool m_joinBorders            = true;
    bool m_drawTextCursor         = true;
    bool m_renderObfuscation      = true;
    bool m_isMouseDragging        = false;

    template<class T>
    void registerInterface(const TextObject objectType) {
        auto *interface = new T(this);

        document()->documentLayout()->registerHandler(objectType, interface);
        m_tempDoc->documentLayout()->registerHandler(objectType, interface);
    }

    void initSimilarWidthCharGroups(const QFont &font, bool isBold);
    void scheduleRegionsUpdate();
    void updateFragmentRegions();
    void obfuscateString(QString &string, const bool isBold,
                         const QFontMetrics &metrics);
    void updateTempDoc();
    void onCursorPositionChanged();
    void ensureCursorVisible();
};

#endif // RAWJSONTEXTEDIT_H
