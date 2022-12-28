#include "stringnode.h"
#include "../visitors/nodevisitor.h"

const static bool _ = TypeRegister<Command::StringNode>::init();

namespace Command {
    DEFINE_ACCEPT_METHOD(StringNode)
}
