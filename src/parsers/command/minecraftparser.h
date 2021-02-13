#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/floatrangenode.h"
#include "nodes/intrangenode.h"
#include "nodes/mapnode.h"
#include "nodes/similaraxesnodes.h"
#include "nodes/similarresourcelocationnodes.h"
#include "nodes/similarstringnodes.h"
#include "nodes/swizzlenode.h"
#include "nodes/timenode.h"
#include "nodes/uuidnode.h"

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
        Command::MapNode *parseMap(QObject *parent,
                                   const QChar &beginChar,
                                   const QChar &endChar,
                                   const QChar &sepChar,
                                   std::function<ParseNode*(const QString &)> func,
                                   bool acceptQuotation = false);
        Command::MapNode *blockStates(QObject *parent);

        AxisNode *parseAxis(QObject *parent,
                            AxisParseOptions options,
                            bool &isLocal);
        AxesNode *parseAxes(QObject *parent, AxisParseOptions options);

        Q_INVOKABLE Command::BlockPosNode *minecraft_blockPos(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ColorNode *minecraft_color(QObject *parent,
                                                        const QVariantMap &props = {});
        Q_INVOKABLE Command::ColumnPosNode *minecraft_columnPos(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::DimensionNode *minecraft_dimension(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::EntityAnchorNode *minecraft_entityAnchor(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::EntitySummonNode *minecraft_entitySummon(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::FloatRangeNode *minecraft_floatRange(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::FunctionNode *minecraft_function(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::IntRangeNode *minecraft_intRange(QObject *parent,
                                                              const QVariantMap &props = {});
        Q_INVOKABLE Command::ItemEnchantmentNode *minecraft_itemEnchantment(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::MessageNode *minecraft_message(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::MobEffectNode *minecraft_mobEffect(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ObjectiveNode *minecraft_objective(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ObjectiveCriteriaNode *minecraft_objectiveCriteria(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::OperationNode *minecraft_operation(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::ResourceLocationNode *minecraft_resourceLocation(
            QObject *parent, const QVariantMap &props = {});
        Q_INVOKABLE Command::RotationNode *minecraft_rotation(QObject *parent,
                                                              const QVariantMap &props = {});
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
