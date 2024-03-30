#ifndef INTERNALREGEXPATTERNNODE_H
#define INTERNALREGEXPATTERNNODE_H

#include "singlevaluenode.h"

#include <QRegularExpression>

/*
 * InternalRegexPatternNode represent a regular expression pattern.
 * Agument nodes of this type must be put at the end of a command branch.
 *
 * Namespaced ID: ___:regex_pattern
 */
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(InternalRegexPattern, QRegularExpression)


#endif // INTERNALREGEXPATTERNNODE_H
