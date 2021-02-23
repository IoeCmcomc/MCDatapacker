#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/blockstatenode.h"
#include "nodes/componentnode.h"
#include "nodes/entitynode.h"
#include "nodes/floatrangenode.h"
#include "nodes/intrangenode.h"
#include "nodes/itemstacknode.h"
#include "nodes/mapnode.h"
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
private:
        QString oneOf(QStringList strArr);

        template<class Container, class Type>
        Container *parseMap(QObject *parent, const QChar &beginChar,
                            const QChar &endChar, const QChar &sepChar,
                            std::function<Type*(const QString &)> func,
                            bool acceptQuotation      = false,
                            const QString &keyCharset = R"(0-9a-zA-Z-_)") {
            QScopedPointer<Container> obj(new Container(parent, pos()));

            this->eat(beginChar);
            int KeyPos = pos();
            while (this->curChar() != endChar) {
                QString name;
                this->skipWs(false);
                if (acceptQuotation &&
                    (curChar() == '"' || curChar() == '\'')) {
                    try {
                        name = getQuotedString();
                    } catch (const Command::Parser::ParsingError &err) {
                        qDebug() << "No quotation have been found. Continue.";
                    }
                }
                if (name.isNull())
                    name = this->getWithCharset(keyCharset);
                this->skipWs(false);
                this->eat(sepChar);
                this->skipWs(false);
                obj->insert(Command::MapKey{ KeyPos, name }, func(name));
                this->skipWs(false);
                if (this->curChar() != endChar) {
                    this->eat(',');
                    this->skipWs(false);
                }
            }
            this->eat(endChar);
            return obj.take();
        }

        AxisNode *parseAxis(QObject *parent,
                            AxisParseOptions options,
                            bool &isLocal);
        AxesNode *parseAxes(QObject *parent, AxisParseOptions options);
        Command::NbtCompoundNode *parseCompoundTag(QObject *parent);
        Command::NbtNode *parseTagValue(QObject *parent);
        Command::NbtNode *parseNumericTag(QObject *parent);

        template<class Container, class Type>
        Container *parseArrayTag(QObject *parent,
                                 const QString &errorMsg) {
            advance();
            if (curChar() == ';') {
                QScopedPointer<Container> ret(new Container(parent, pos() - 3));
                advance();
                while (curChar() != ']') {
                    skipWs(false);
                    QScopedPointer<Type> elem(qobject_cast<Type*>(
                                                  parseNumericTag(parent)));
                    if (elem)
                        ret->append(elem.take());
                    else
                        error(QString(errorMsg));
                    skipWs(false);
                    if (this->curChar() != ']') {
                        this->eat(',');
                        skipWs(false);
                    }
                }
                eat(']');
            } else {
                error(QString(
                          "Missing the character ';' after array type indicator"));
            }
        }
        Command::NbtListNode *parseListTag(QObject *parent);
        Command::MapNode *parseEntityAdvancements(QObject *parent);
        Command::MapNode *parseEntityArguments(QObject *parent);
        Command::TargetSelectorNode *parseTargetSelector(QObject *parent);
        Command::NbtPathStepNode *parseNbtPathStep(QObject *parent);

        /* Direct parsing methods */
        Q_INVOKABLE Command::BlockPosNode *minecraft_blockPos(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::BlockStateNode *minecraft_blockState(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::BlockPredicateNode *minecraft_blockPredicate(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::ColorNode *minecraft_color(QObject *parent,
                                                        const QVariantMap &props = {});
        Q_INVOKABLE Command::ColumnPosNode *minecraft_columnPos(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ComponentNode *minecraft_component(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::DimensionNode *minecraft_dimension(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::EntityNode *minecraft_entity(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::EntityAnchorNode *minecraft_entityAnchor(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::EntitySummonNode *minecraft_entitySummon(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::FloatRangeNode *minecraft_floatRange(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::FunctionNode *minecraft_function(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::GameProfileNode *minecraft_gameProfile(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::IntRangeNode *minecraft_intRange(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ItemEnchantmentNode *minecraft_itemEnchantment(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::ItemSlotNode *minecraft_itemSlot(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ItemStackNode *minecraft_itemStack(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::ItemPredicateNode *minecraft_itemPredicate(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::MessageNode *minecraft_message(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::MobEffectNode *minecraft_mobEffect(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::NbtCompoundNode *minecraft_nbtCompoundTag(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::NbtPathNode *minecraft_nbtPath(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::NbtNode *minecraft_nbtTag(QObject *parent,
                                                       const QVariantMap &props = {});
        Q_INVOKABLE Command::ObjectiveNode *minecraft_objective(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ObjectiveCriteriaNode *minecraft_objectiveCriteria(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::OperationNode *minecraft_operation(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ParticleNode *minecraft_particle(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ResourceLocationNode *minecraft_resourceLocation(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::RotationNode *minecraft_rotation(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ScoreHolderNode *minecraft_scoreHolder(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::ScoreboardSlotNode *minecraft_scoreboardSlot(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::SwizzleNode *minecraft_swizzle(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::TeamNode *minecraft_team(QObject *parent,
                                                      const QVariantMap &props = {});
        Q_INVOKABLE Command::TimeNode *minecraft_time(QObject *parent,
                                                      const QVariantMap &props = {});
        Q_INVOKABLE Command::UuidNode *minecraft_uuid(QObject *parent,
                                                      const QVariantMap &props = {});
        Q_INVOKABLE Command::Vec2Node *minecraft_vec2(QObject *parent,
                                                      const QVariantMap &props = {});
        Q_INVOKABLE Command::Vec3Node *minecraft_vec3(QObject *parent,
                                                      const QVariantMap &props = {});
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Command::MinecraftParser::AxisParseOptions)

#endif /* MINECRAFTPARSER_H */
