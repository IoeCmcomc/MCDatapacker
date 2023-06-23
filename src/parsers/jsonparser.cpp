#include "jsonparser.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

JsonParser::JsonParser() {
}

bool JsonParser::parseImpl() {
    try {
        const json &&j = json::parse(text().toStdString(),
                                     nullptr, true, true);
        return true;
    }  catch (const json::parse_error &err) {
        m_errors <<
            Error(err.what(), qBound(0, (int)err.byte, text().length()));
        return false;
    }
}
