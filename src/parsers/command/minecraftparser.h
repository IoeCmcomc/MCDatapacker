#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/blockstatenode.h"
#include "nodes/componentnode.h"
#include "nodes/entitynode.h"
#include "nodes/floatrangenode.h"
#include "nodes/intrangenode.h"
#include "nodes/itemstacknode.h"
#include "nodes/multimapnode.h"
#include "nodes/nbtpathnode.h"
#include "nodes/particlenode.h"
#include "nodes/similaraxesnodes.h"
#include "nodes/similarresourcelocationnodes.h"
#include "nodes/similarstringnodes.h"
#include "nodes/swizzlenode.h"
#include "nodes/timenode.h"

namespace Command {
    class MinecraftParser : public Parser
    {
        Q_OBJECT
public:
        enum class AxisParseOption : unsigned char {
            NoOption    = 0,
            ParseY      = 1,
            OnlyInteger = 2,
            CanBeLocal  = 4,
        };
        Q_DECLARE_FLAGS(AxisParseOptions, AxisParseOption);

        explicit MinecraftParser(QObject *parent      = nullptr,
                                 const QString &input = "");
protected:
        QSharedPointer<ParseNode> QVariantToParseNodeSharedPointer(
            const QVariant &vari);

private:
        QString oneOf(QStringList strArr);

        template<class Container, class Type>
        QSharedPointer<Container> parseMap(const QChar &beginChar,
                                           const QChar &endChar,
                                           const QChar &sepChar,
                                           std::function<QSharedPointer<Type>(const QString &)> func,
                                           bool acceptQuotation      = false,
                                           const QString &keyCharset = R"(0-9a-zA-Z-_)")
        {
            auto obj = QSharedPointer<Container>::create(pos());

            this->eat(beginChar);
            while (this->curChar() != endChar) {
                this->skipWs(false);
                bool    isQuote = false;
                int     KeyPos  = pos();
                QString name;
                if (acceptQuotation &&
                    (curChar() == '"' || curChar() == '\'')) {
                    try {
                        name    = getQuotedString();
                        isQuote = true;
                    } catch (const Command::Parser::Error &err) {
                        qDebug() << "No quotation have been found. Continue.";
                    }
                }
                if (name.isNull())
                    name = this->getWithCharset(keyCharset);
                if (name.isNull())
                    error("Invaild empty key", {}, KeyPos);
                this->skipWs(false);
                this->eat(sepChar);
                this->skipWs(false);
                obj->insert(Command::MapKey{ KeyPos, name, isQuote, false },
                            func(name));
                this->skipWs(false);
                if (this->curChar() != endChar) {
                    this->eat(',');
                    this->skipWs(false);
                }
            }
            this->eat(endChar);
            obj->setLength(pos() - obj->pos());
            return obj;
        }

        QSharedPointer<AxisNode> parseAxis(AxisParseOptions options,
                                           bool &isLocal);
        QSharedPointer<AxesNode> parseAxes(AxisParseOptions options);
        QSharedPointer<Command::NbtCompoundNode> parseCompoundTag();
        QSharedPointer<Command::NbtNode> parseTagValue();
        QSharedPointer<Command::NbtNode> parseNumericTag();

        template<class Container, class Type>
        QSharedPointer<Container> parseArrayTag(const QString &errorMsg) {
            advance();
            if (curChar() == ';') {
                auto ret = QSharedPointer<Container>::create(pos() - 3);
                advance();
                while (curChar() != ']') {
                    skipWs(false);
                    auto elem = qSharedPointerCast<Type>(
                        parseNumericTag());
                    if (elem)
                        ret->append(elem);
                    else
                        error(QString(errorMsg));
                    skipWs(false);
                    if (this->curChar() != ']') {
                        this->eat(',');
                        skipWs(false);
                    }
                }
                eat(']');
                return ret;
            } else {
                error(QString(
                          "Missing the character ';' after array type indicator"));
            }
            return nullptr;
        }
        QSharedPointer<Command::NbtListNode> parseListTag();
        QSharedPointer<Command::MapNode> parseEntityAdvancements();
        QSharedPointer<Command::MultiMapNode> parseEntityArguments();
        QSharedPointer<Command::TargetSelectorNode> parseTargetSelector();
        QSharedPointer<Command::NbtPathStepNode> parseNbtPathStep();

        /* Direct parsing methods */
        Q_INVOKABLE QSharedPointer<Command::AngleNode> minecraft_angle(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::BlockPosNode> minecraft_blockPos(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::BlockStateNode> minecraft_blockState(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::BlockPredicateNode>
        minecraft_blockPredicate(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ColorNode> minecraft_color(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ColumnPosNode> minecraft_columnPos(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ComponentNode> minecraft_component(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::DimensionNode> minecraft_dimension(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::EntityNode> minecraft_entity(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::EntityAnchorNode>
        minecraft_entityAnchor(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::EntitySummonNode>
        minecraft_entitySummon(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::FloatRangeNode> minecraft_floatRange(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::FunctionNode> minecraft_function(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::GameProfileNode>
        minecraft_gameProfile(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::IntRangeNode> minecraft_intRange(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ItemEnchantmentNode>
        minecraft_itemEnchantment(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ItemSlotNode> minecraft_itemSlot(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ItemStackNode> minecraft_itemStack(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ItemPredicateNode>
        minecraft_itemPredicate(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::MessageNode> minecraft_message(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::MobEffectNode> minecraft_mobEffect(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::NbtCompoundNode>
        minecraft_nbtCompoundTag(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::NbtPathNode> minecraft_nbtPath(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::NbtNode> minecraft_nbtTag(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ObjectiveNode> minecraft_objective(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ObjectiveCriteriaNode>
        minecraft_objectiveCriteria(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::OperationNode> minecraft_operation(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ParticleNode> minecraft_particle(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ResourceLocationNode>
        minecraft_resourceLocation(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::RotationNode> minecraft_rotation(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ScoreHolderNode>
        minecraft_scoreHolder(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ScoreboardSlotNode>
        minecraft_scoreboardSlot(const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::SwizzleNode> minecraft_swizzle(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::TeamNode> minecraft_team(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::TimeNode> minecraft_time(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::UuidNode> minecraft_uuid(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::Vec2Node> minecraft_vec2(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::Vec3Node> minecraft_vec3(
            const QVariantMap &props = {});
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Command::MinecraftParser::AxisParseOptions)

#endif /* MINECRAFTPARSER_H */
