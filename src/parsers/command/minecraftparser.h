#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "schemaparser.h"
#include "nodes/axesnode.h"
#include "nodes/blockstatenode.h"
#include "nodes/componentnode.h"
#include "nodes/gamemodenode.h"
#include "nodes/entitynode.h"
#include "nodes/floatrangenode.h"
#include "nodes/intrangenode.h"
#include "nodes/itemstacknode.h"
#include "nodes/nbtpathnode.h"
#include "nodes/particlenode.h"
#include "nodes/swizzlenode.h"
#include "nodes/timenode.h"
#include "nodes/targetselectornode.h"

#include <QVersionNumber>

template<typename T>
T strWithExpToDec(QStringView v, bool &ok) {
    constexpr std::make_unsigned_t<T> maxLimit = std::numeric_limits<T>::max();

    if (v.isEmpty()) {
        ok = false;
        return 0;
    }

    std::make_unsigned_t<T> value = 0;
    int                     sign  = 1;

    switch (v.at(0).toLatin1()) {
        case '-': {
            sign = -1;
            v    = v.mid(1);
            break;
        }
        case '+': {
            sign = 1;
            v    = v.mid(1);
            break;
        }
    }

    ok = false;
    for (int i = 0; i < v.length(); ++i) {
        if (value > maxLimit / 10) {
            ok = false;
            return 0;
        }
        value *= 10;
        const char ch = v[i].toLatin1();
        switch (ch) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                const uchar digit = ch - '0';
                value += digit;
                ok     = true;
                break;
            }
            case 'E':
            case 'e': {
                if (i > 0) {
                    bool        expOk;
                    QStringView expView = v.mid(i + 1);
                    const int   exp     = strToDec<int>(expView, expOk);
                    if (!expOk) {
                        ok = false;
                        return 0;
                    }
                    i += expView.length();
                    if (exp == 0 || value == 0) {
                        ok = true;
                        break;
                    } else if (exp > 0) {
                        for (int j = 0; j < abs(exp); ++j) {
                            if (value > maxLimit / 10) {
                                ok = false;
                                return 0;
                            }
                            value *= 10;
                        }
                    } else {
                        for (int j = 0; j < abs(exp); ++j) {
                            if (value > std::numeric_limits<T>::min() * 10) {
                                ok = false;
                                return 0;
                            }
                            value /= 10;
                        }
                    }
                    break;
                } else {
                    ok = false;
                    return 0;
                }
            }
            default: {
                ok = false;
                return 0;
            }
        }
        if (((sign == 1) && (value > maxLimit))
            || ((sign == -1) && (value > -std::numeric_limits<T>::min()))) {
            ok = false;
            return 0;
        }
    }
    return value * sign;
}

namespace Command {
    class MinecraftParser final : public SchemaParser  {
        Q_GADGET;
public:
        enum class AxisParseOption : unsigned char {
            NoOption    = 0,
            OnlyInteger = 1,
            CanBeLocal  = 2,
        };
        Q_DECLARE_FLAGS(AxisParseOptions, AxisParseOption);

        MinecraftParser();
        using SchemaParser::SchemaParser;

        static void setGameVer(const QVersionNumber &newGameVer,
                               const bool autoLoadSchema = true);

private:
        friend class McfunctionParser;

        static inline QVersionNumber gameVer = QVersionNumber();

        template<typename T, size_t N>
        QString oneOf(const std::array<T, N> &strArr) {
            const int         start   = pos();
            const QStringView curText = peekRest();

            for (const auto &str: strArr) {
                if (curText.startsWith(str)) {
                    advance(str.size());
                    return str;
                }
            }
            reportError(QT_TR_NOOP("Only accept one of the following: %1"),
                        { join(strArr) }, start,
                        peekLiteral().length());
            return QString();
        }

        template<typename T, size_t N>
        QString join(const std::array<T, N> &array) {
            QString ret;

            for (auto it = array.cbegin(); it != array.cend();) {
                ret += *it;
                ++it;
                if (it != array.cend()) {
                    ret += ", "_QL1;
                }
            }
            return ret;
        }

        QString eatListSep(QChar sepChr, QChar endChr);

        template<class Container, class Type>
        QSharedPointer<Container> parseMap(QChar beginChar,
                                           QChar endChar,
                                           QChar sepChar,
                                           std::function<QSharedPointer<Type>(const QString &)> func,
                                           bool acceptQuotation            = false,
                                           const QLatin1String &keyCharset = QLatin1String())
        {
            auto    &&obj   = QSharedPointer<Container>::create(0);
            const int start = pos();

            obj->setLeftText(this->eat(beginChar));
            while (this->curChar() != endChar) {
                const auto &&trivia = this->skipWs(false);
                const int    keyPos = pos();
                QString      name;
                if (acceptQuotation &&
                    (curChar() == '"' || curChar() == '\'')) {
                    try {
                        name = getQuotedString();
                    } catch (const SchemaParser::Error &) {
                        qDebug() << "No quotation have been found. Continue.";
                    }
                }
                if (name.isEmpty()) {
                    setPos(keyPos);
                    if (keyCharset.isNull()) {
                        name = getLiteralString().toString();
                    } else {
                        name = getWithCharset(keyCharset).toString();
                    }
                }
                if (name.isEmpty())
                    reportError("Invalid empty key", {}, keyPos);
                const auto &&key = KeyPtr::create(spanText(keyPos), name,
                                                  !name.isEmpty());
                key->setLeadingTrivia(trivia);
                key->setTrailingTrivia(spanText(eat(sepChar,
                                                    "Unexpected %1, expecting %2 separator between a key and a value",
                                                    SkipLeftWs)));
                const auto &&valueTrivia = skipWs(false);
                //const int    valueStart  = pos();
                const auto &&value = func(name);
                //value->setLength(pos() - valueStart);
                value->setLeadingTrivia(spanText(valueTrivia));
                value->setTrailingTrivia(this->skipWs(false));
                obj->insert(key, value);
                if (this->curChar() != endChar) {
                    obj->constLast()->setTrailingTrivia(eat(',', nullptr,
                                                            SkipRightWs));
                }
            }
            obj->setRightText(this->eat(endChar));
            obj->setLength(pos() - start);
            return obj;
        }

        QSharedPointer<AngleNode> parseAxis(AxisParseOptions options,
                                            bool &isLocal);
        void parseAxes(ArgumentNode *node, AxisParseOptions options);
        QSharedPointer<XyzNode> parseXyzAxes(AxisParseOptions options);
        QSharedPointer<NbtCompoundNode> parseCompoundTag();
        QSharedPointer<NbtNode> parseTagValue();
        QSharedPointer<NbtNode> parseNumericTag();

        template<class Container, class Type>
        QSharedPointer<Container> parseArrayTag(const char *errorMsg) {
            const int start = pos() - 1;

            advance();
            const auto &&ret = QSharedPointer<Container>::create(0);
            advance();
            ret->setLeftText(spanText(start));

            while (curChar() != ']') {
                const auto &&trivia = skipWs(false);
                const auto &&numTag = parseNumericTag();
                const auto &&elem   = qSharedPointerCast<Type>(numTag);
                if (!elem) {
                    reportError(errorMsg);
                }
                elem->setLeadingTrivia(trivia);
                ret->append(elem);
                elem->setTrailingTrivia(eatListSep(',', ']'));
            }
            ret->setRightText(eat(']'));
            return ret;
        }
        QSharedPointer<NbtListNode> parseListTag();
        QSharedPointer<MapNode> parseEntityAdvancements();
        QSharedPointer<MapNode> parseEntityArguments();
        QSharedPointer<TargetSelectorNode> parseTargetSelector();
        QSharedPointer<NbtPathStepNode> parseNbtPathStep();
        QSharedPointer<ParticleColorNode> parseParticleColor();
        QSharedPointer<EntityArgumentValueNode> parseNegEntityArg();
        void parseResourceLocation(ResourceLocationNode *node,
                                   bool acceptTag = false);
        void parseBlock(BlockStateNode *node, bool acceptTag);
        void parseEntity(EntityNode *node, bool allowFakePlayer);

        NodePtr invokeMethod(ArgumentNode::ParserType parserType,
                             const QVariantMap &props) final;

        /* Direct parsing methods */
        QSharedPointer<AngleNode> minecraft_angle();
        QSharedPointer<BlockPosNode> minecraft_blockPos();
        QSharedPointer<BlockStateNode> minecraft_blockState();
        QSharedPointer<BlockPredicateNode> minecraft_blockPredicate();
        QSharedPointer<ColorNode> minecraft_color();
        QSharedPointer<ColumnPosNode> minecraft_columnPos();
        QSharedPointer<ComponentNode> minecraft_component();
        QSharedPointer<DimensionNode> minecraft_dimension();
        QSharedPointer<EntityNode> minecraft_entity(
            const QVariantMap &props = {});
        QSharedPointer<EntityAnchorNode> minecraft_entityAnchor();
        QSharedPointer<EntitySummonNode>
        minecraft_entitySummon();
        QSharedPointer<FloatRangeNode> minecraft_floatRange(
            const QVariantMap &props = {});
        QSharedPointer<FunctionNode> minecraft_function();
        QSharedPointer<GamemodeNode> minecraft_gamemode();
        QSharedPointer<GameProfileNode> minecraft_gameProfile();
        QSharedPointer<HeightmapNode> minecraft_heightmap();
        QSharedPointer<IntRangeNode> minecraft_intRange(
            const QVariantMap &props = {});
        QSharedPointer<ItemEnchantmentNode> minecraft_itemEnchantment();
        QSharedPointer<ItemSlotNode> minecraft_itemSlot();
        QSharedPointer<ItemStackNode> minecraft_itemStack();
        QSharedPointer<ItemPredicateNode> minecraft_itemPredicate();
        QSharedPointer<MessageNode> minecraft_message();
        QSharedPointer<MobEffectNode> minecraft_mobEffect();
        QSharedPointer<NbtCompoundNode> minecraft_nbtCompoundTag();
        QSharedPointer<NbtPathNode> minecraft_nbtPath();
        QSharedPointer<NbtNode> minecraft_nbtTag();
        QSharedPointer<ObjectiveNode> minecraft_objective();
        QSharedPointer<ObjectiveCriteriaNode> minecraft_objectiveCriteria();
        QSharedPointer<OperationNode> minecraft_operation();
        QSharedPointer<ParticleNode> minecraft_particle();
        QSharedPointer<ResourceNode> minecraft_resource(
            const QVariantMap &props);
        QSharedPointer<ResourceKeyNode> minecraft_resourceKey(
            const QVariantMap &props);
        QSharedPointer<ResourceOrTagNode> minecraft_resourceOrTag(
            const QVariantMap &props);
        QSharedPointer<ResourceOrTagKeyNode> minecraft_resourceOrTagKey(
            const QVariantMap &props);
        QSharedPointer<ResourceLocationNode> minecraft_resourceLocation();
        QSharedPointer<RotationNode> minecraft_rotation();
        QSharedPointer<ScoreHolderNode> minecraft_scoreHolder(
            const QVariantMap &props = {});
        QSharedPointer<ScoreboardSlotNode> minecraft_scoreboardSlot();
        QSharedPointer<SwizzleNode> minecraft_swizzle();
        QSharedPointer<TeamNode> minecraft_team();
        QSharedPointer<TimeNode> minecraft_time();
        QSharedPointer<TemplateMirrorNode> minecraft_templateMirror();
        QSharedPointer<TemplateRotationNode> minecraft_templateRotation();
        QSharedPointer<UuidNode> minecraft_uuid();
        QSharedPointer<Vec2Node> minecraft_vec2();
        QSharedPointer<Vec3Node> minecraft_vec3();

        // MCDatapacker-specific parser methods
        QSharedPointer<InternalGreedyStringNode> parseGreedyString();
        QSharedPointer<InternalRegexPatternNode> parseRegexPattern();
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Command::MinecraftParser::AxisParseOptions)

#endif /* MINECRAFTPARSER_H */
