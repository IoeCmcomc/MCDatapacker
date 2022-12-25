#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/axesnode.h"
#include "nodes/blockstatenode.h"
#include "nodes/componentnode.h"
#include "nodes/entitynode.h"
#include "nodes/floatrangenode.h"
#include "nodes/intrangenode.h"
#include "nodes/itemstacknode.h"
#include "nodes/nbtpathnode.h"
#include "nodes/particlenode.h"
#include "nodes/swizzlenode.h"
#include "nodes/timenode.h"

#include <QVersionNumber>

namespace Command {
    class MinecraftParser : public Parser
    {
        Q_OBJECT
public:
        enum class AxisParseOption : unsigned char {
            NoOption    = 0,
            OnlyInteger = 1,
            CanBeLocal  = 2,
        };
        Q_DECLARE_FLAGS(AxisParseOptions, AxisParseOption);

        explicit MinecraftParser(QObject *parent      = nullptr,
                                 const QString &input = "");

        static void setGameVer(const QVersionNumber &newGameVer);

private:
        static inline QVersionNumber gameVer = QVersionNumber();

        QString oneOf(const QStringList &strArr);

        template<class Container, class Type>
        QSharedPointer<Container> parseMap(QChar beginChar,
                                           QChar endChar,
                                           QChar sepChar,
                                           std::function<QSharedPointer<Type>(const QString &)> func,
                                           bool acceptQuotation      = false,
                                           const QString &keyCharset = R"(0-9a-zA-Z-_.+)")
        {
            const auto &&obj   = QSharedPointer<Container>::create(0);
            const int    start = pos();

            obj->setLeftText(this->eat(beginChar));
            while (this->curChar() != endChar) {
                const auto &&trivia = this->skipWs(false);
                const int    keyPos = pos();
                QString      name;
                if (acceptQuotation &&
                    (curChar() == '"' || curChar() == '\'')) {
                    try {
                        name = getQuotedString();
                    } catch (const Parser::Error &err) {
                        qDebug() << "No quotation have been found. Continue.";
                    }
                }
                if (name.isNull())
                    name = this->getWithCharset(keyCharset);
                if (name.isNull())
                    error("Invaild empty key", {}, keyPos);
                const auto &&key = KeyPtr::create(spanText(name));
                key->setLeadingTrivia(trivia);
                int triviaStart = pos();
                this->skipWs(false);
                this->eat(sepChar);
                key->setTrailingTrivia(spanText(triviaStart));
                const auto &&valueTrivia = skipWs(false);
                const int    valueStart  = pos();
                const auto &&value       = func(name);
                value->setLength(pos() - valueStart);
                value->setLeadingTrivia(spanText(valueTrivia));
                obj->insert(key, value);
                value->setTrailingTrivia(spanText(this->skipWs(false)));
                if (this->curChar() != endChar) {
                    triviaStart = pos();
                    this->eat(',');
                    this->skipWs(false);
                    obj->constLast()->setTrailingTrivia(spanText(triviaStart));
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
        QSharedPointer<Container> parseArrayTag(const QString &errorMsg) {
            const int start = pos() - 2;

            advance();
            if (curChar() == ';') {
                const auto &&ret = QSharedPointer<Container>::create(0);
                advance();
                ret->setLeftText(spanText(start));

                while (curChar() != ']') {
                    const auto &&trivia = skipWs(false);
                    const auto &&elem   = qSharedPointerCast<Type>(
                        parseNumericTag());
                    if (!elem) {
                        error(QString(errorMsg));
                    }
                    elem->setLeadingTrivia(trivia);
                    ret->append(elem);
                    const int triviaStart = pos();
                    if (this->curChar() != ']') {
                        this->eat(',');
                        skipWs(false);
                    }
                    elem->setTrailingTrivia(spanText(triviaStart));
                }
                ret->setRightText(eat(']'));
                return ret;
            } else {
                error(QString(
                          "Missing the character ';' after array type indicator"));
            }
            return nullptr;
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

        /* Direct parsing methods */
        Q_INVOKABLE QSharedPointer<AngleNode> minecraft_angle();
        Q_INVOKABLE QSharedPointer<BlockPosNode> minecraft_blockPos();
        Q_INVOKABLE QSharedPointer<BlockStateNode> minecraft_blockState();
        Q_INVOKABLE QSharedPointer<BlockPredicateNode>
        minecraft_blockPredicate();
        Q_INVOKABLE QSharedPointer<ColorNode> minecraft_color();
        Q_INVOKABLE QSharedPointer<ColumnPosNode> minecraft_columnPos();
        Q_INVOKABLE QSharedPointer<ComponentNode> minecraft_component();
        Q_INVOKABLE QSharedPointer<DimensionNode> minecraft_dimension();
        Q_INVOKABLE QSharedPointer<EntityNode> minecraft_entity(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<EntityAnchorNode>
        minecraft_entityAnchor();
        Q_INVOKABLE QSharedPointer<EntitySummonNode>
        minecraft_entitySummon();
        Q_INVOKABLE QSharedPointer<FloatRangeNode> minecraft_floatRange(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<FunctionNode> minecraft_function();
        Q_INVOKABLE QSharedPointer<GameProfileNode>
        minecraft_gameProfile(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<IntRangeNode> minecraft_intRange(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<ItemEnchantmentNode>
        minecraft_itemEnchantment();
        Q_INVOKABLE QSharedPointer<ItemSlotNode> minecraft_itemSlot();
        Q_INVOKABLE QSharedPointer<ItemStackNode> minecraft_itemStack();
        Q_INVOKABLE QSharedPointer<ItemPredicateNode>
        minecraft_itemPredicate();
        Q_INVOKABLE QSharedPointer<MessageNode> minecraft_message();
        Q_INVOKABLE QSharedPointer<MobEffectNode> minecraft_mobEffect();
        Q_INVOKABLE QSharedPointer<NbtCompoundNode>
        minecraft_nbtCompoundTag();
        Q_INVOKABLE QSharedPointer<NbtPathNode> minecraft_nbtPath();
        Q_INVOKABLE QSharedPointer<NbtNode> minecraft_nbtTag();
        Q_INVOKABLE QSharedPointer<ObjectiveNode> minecraft_objective();
        Q_INVOKABLE QSharedPointer<ObjectiveCriteriaNode>
        minecraft_objectiveCriteria();
        Q_INVOKABLE QSharedPointer<OperationNode> minecraft_operation();
        Q_INVOKABLE QSharedPointer<ParticleNode> minecraft_particle();
        Q_INVOKABLE QSharedPointer<ResourceNode>
        minecraft_resource(const QVariantMap &props);
        Q_INVOKABLE QSharedPointer<ResourceOrTagNode>
        minecraft_resourceOrTag(const QVariantMap &props);
        Q_INVOKABLE QSharedPointer<ResourceLocationNode>
        minecraft_resourceLocation();
        Q_INVOKABLE QSharedPointer<RotationNode> minecraft_rotation();
        Q_INVOKABLE QSharedPointer<ScoreHolderNode>
        minecraft_scoreHolder(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<ScoreboardSlotNode>
        minecraft_scoreboardSlot();
        Q_INVOKABLE QSharedPointer<SwizzleNode> minecraft_swizzle();
        Q_INVOKABLE QSharedPointer<TeamNode> minecraft_team();
        Q_INVOKABLE QSharedPointer<TimeNode> minecraft_time();
        Q_INVOKABLE QSharedPointer<UuidNode> minecraft_uuid();
        Q_INVOKABLE QSharedPointer<Vec2Node> minecraft_vec2();
        Q_INVOKABLE QSharedPointer<Vec3Node> minecraft_vec3();
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Command::MinecraftParser::AxisParseOptions)

#endif /* MINECRAFTPARSER_H */
