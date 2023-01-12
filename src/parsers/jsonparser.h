#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "parser.h"

class JsonParser : public Parser {
public:
    JsonParser();

protected:
    bool parseImpl() override;
};

#endif // JSONPARSER_H
