#include "schemaargumentnode.h"

namespace Command::Schema {
ArgumentNode::ArgumentNode() : Node(Node::Kind::Argument),
    m_parserType{ParserType::Unknown} {
    }

    void ArgumentNode::setName(const QString &newName) {
        m_name = newName;
    }

    QString ArgumentNode::name() const {
        return m_name;
    }

    QVariantMap ArgumentNode::properties() const {
        return m_props;
    }

    ArgumentNode::ParserType ArgumentNode::parserType() const {
        return m_parserType;
    }

    void from_json(const json &j, ArgumentNode *&n) {
        using PT = ArgumentNode::ParserType;
        static const QMap<QString, PT> parserIdMap {
            { QStringLiteral("brigadier:bool"), PT::Bool },
            { QStringLiteral("brigadier:double"), PT::Double },
            { QStringLiteral("brigadier:float"), PT::Float },
            { QStringLiteral("brigadier:integer"), PT::Integer },
            { QStringLiteral("brigadier:literal"), PT::Literal },
            { QStringLiteral("brigadier:string"), PT::String },
            { QStringLiteral("minecraft:angle"), PT::Angle },
            { QStringLiteral("minecraft:block"), PT::BlockState },
            { QStringLiteral("minecraft:block_pos"), PT::BlockPos },
            { QStringLiteral("minecraft:block_state"), PT::BlockState },
            { QStringLiteral("minecraft:block_predicate"), PT::BlockPredicate },
            { QStringLiteral("minecraft:color"), PT::Color },
            { QStringLiteral("minecraft:column_pos"), PT::ColumnPos },
            { QStringLiteral("minecraft:component"), PT::Component },
            { QStringLiteral("minecraft:dimension"), PT::Dimension },
            { QStringLiteral("minecraft:entity"), PT::Entity },
            { QStringLiteral("minecraft:entity_anchor"), PT::EntityAnchor },
            { QStringLiteral("minecraft:entity_summon"), PT::EntitySummon },
            { QStringLiteral("minecraft:float_range"), PT::FloatRange },
            { QStringLiteral("minecraft:function"), PT::Function },
            { QStringLiteral("minecraft:gamemode"), PT::Gamemode },
            { QStringLiteral("minecraft:game_profile"), PT::GameProfile },
            { QStringLiteral("minecraft:heightmap"), PT::Heightmap },
            { QStringLiteral("minecraft:int_range"), PT::IntRange },
            { QStringLiteral("minecraft:item_enchantment"),
              PT::ItemEnchantment },
            { QStringLiteral("minecraft:item"), PT::ItemStack },
            { QStringLiteral("minecraft:item_predicate"), PT::ItemPredicate },
            { QStringLiteral("minecraft:item_slot"), PT::ItemSlot },
            { QStringLiteral("minecraft:item_stack"), PT::ItemStack },
            { QStringLiteral("minecraft:message"), PT::Message },
            { QStringLiteral("minecraft:mob_effect"), PT::MobEffect },
            { QStringLiteral("minecraft:nbt_compound_tag"),
              PT::NbtCompoundTag },
            { QStringLiteral("minecraft:nbt_path"), PT::NbtPath },
            { QStringLiteral("minecraft:nbt_tag"), PT::NbtTag },
            { QStringLiteral("minecraft:objective"), PT::Objective },
            { QStringLiteral("minecraft:objective_criteria"),
              PT::ObjectiveCriteria },
            { QStringLiteral("minecraft:operation"), PT::Operation },
            { QStringLiteral("minecraft:particle"), PT::Particle },
            { QStringLiteral("minecraft:resource"), PT::Resource },
            { QStringLiteral("minecraft:resource_key"), PT::ResourceKey },
            { QStringLiteral("minecraft:resource_location"),
              PT::ResourceLocation },
            { QStringLiteral("minecraft:resource_or_tag"), PT::ResourceOrTag },
            { QStringLiteral("minecraft:resource_or_tag_key"),
              PT::ResourceOrTagKey },
            { QStringLiteral("minecraft:rotation"), PT::Rotation },
            { QStringLiteral("minecraft:score_holder"), PT::ScoreHolder },
            { QStringLiteral("minecraft:scoreboard_slot"), PT::ScoreboardSlot },
            { QStringLiteral("minecraft:swizzle"), PT::Swizzle },
            { QStringLiteral("minecraft:template_mirror"), PT::TemplateMirror },
            { QStringLiteral("minecraft:template_rotation"),
              PT::TemplateRotation },
            { QStringLiteral("minecraft:team"), PT::Team },
            { QStringLiteral("minecraft:time"), PT::Time },
            { QStringLiteral("minecraft:uuid"), PT::Uuid },
            { QStringLiteral("minecraft:vec2"), PT::Vec2 },
            { QStringLiteral("minecraft:vec3"), PT::Vec3 },
            { QStringLiteral("___:greedy_string"), PT::InternalGreedyString },
            { QStringLiteral("___:regex_pattern"), PT::InternalRegexPattern },
        };

        n = new ArgumentNode();
        const auto parserType = parserIdMap.value(j.at("parser").get<QString>(),
                                                  PT::Unknown);
        n->m_parserType = parserType;
        if (parserType == PT::Unknown) {
            qWarning() << "Unknown parser ID:" << j.at("parser").get<QString>();
        }

        if (j.contains("properties")) {
            for (auto& [key, val] : j.at("properties").items()) {
                using Type = json::value_t;

                const QString &mapKey = QString::fromStdString(key);

                switch (val.type()) {
                    case Type::boolean: {
                        n->m_props[mapKey] = val.get<bool>();
                        break;
                    }

                    case Type::string: {
                        n->m_props[mapKey] = val.get<QString>();
                        break;
                    }

                    case Type::number_integer: {
                        n->m_props[mapKey] = val.get<int>();
                        break;
                    }

                    case Type::number_float: {
                        n->m_props[mapKey] = val.get<double>();
                        break;
                    }

                    case Type::number_unsigned: {
                        n->m_props[mapKey] = val.get<unsigned int>();
                        break;
                    }

                    case Type::null: {
                        n->m_props[mapKey] = QVariant();
                        break;
                    }

                    default:
                        n->m_props[mapKey] = QVariant();
                        break;
                }
            }
        }

        n->_from_json(j);
    }
}
