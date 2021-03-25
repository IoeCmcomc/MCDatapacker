TEMPLATE = subdirs

SUBDIRS += unit/parser/command/nodes/ArgumentNode \
    unit/parser/command/nodes/AxesNode \
    unit/parser/command/nodes/AxisNode \
    unit/parser/command/nodes/BoolNode \
    unit/parser/command/nodes/DoubleNode \
    unit/parser/command/nodes/FloatNode \
    unit/parser/command/nodes/IntRangeNode \
    unit/parser/command/nodes/IntegerNode \
    unit/parser/command/nodes/LiteralNode \
    unit/parser/command/nodes/MapNode \
    unit/parser/command/nodes/NbtNodes \
    unit/parser/command/nodes/ParseNode \
    unit/parser/command/nodes/ResourceLocationNode \
    unit/parser/command/nodes/RootNode \
    unit/parser/command/nodes/SimilarAxesNodes \
    unit/parser/command/nodes/SimilarResourceLocationNodes \
    unit/parser/command/nodes/SimilarStringNode \
    unit/parser/command/nodes/StringNode \
    unit/parser/command/nodes/SwizzleNode \
    unit/parser/command/nodes/TimeNode \
    unit/parser/command/nodes/UuidNode \
    unit/parser/command/Parser \
    unit/parser/command/MinecraftParser \
    unit/parser/command/visitors/NodeFormatter
