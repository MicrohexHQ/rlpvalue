// Copyright 2014 BitPay Inc.
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <iomanip>
#include <stdio.h>
#include "rlpvalue.h"
#include "rlpvalue_escapes.h"

static std::string json_escape(const std::string& inS)
{
    std::string outS;
    outS.reserve(inS.size() * 2);

    for (unsigned int i = 0; i < inS.size(); i++) {
        unsigned char ch = inS[i];
        const char *escStr = escapes[ch];

        if (escStr)
            outS += escStr;
        else
            outS += ch;
    }

    return outS;
}

std::string RLPValue::write(unsigned int prettyIndent,
                            unsigned int indentLevel) const
{
    std::string s;
    s.reserve(1024);

    unsigned int modIndent = indentLevel;
    if (modIndent == 0)
        modIndent = 1;

    switch (typ) {
    case VNULL:
        s += "null";
        break;
    case VOBJ:
        writeObject(prettyIndent, modIndent, s);
        break;
    case VARR:
        writeArray(prettyIndent, modIndent, s);
        break;
    case VSTR:
        s += "\"" + json_escape(val) + "\"";
        break;
    case VNUM:
        s += val;
        break;
    case VBOOL:
        s += (val == "1" ? "true" : "false");
        break;
    }

    return s;
}

static void indentStr(unsigned int prettyIndent, unsigned int indentLevel, std::string& s)
{
    s.append(prettyIndent * indentLevel, ' ');
}

void RLPValue::writeArray(unsigned int prettyIndent, unsigned int indentLevel, std::string& s) const
{
    s += "[";
    if (prettyIndent)
        s += "\n";

    for (unsigned int i = 0; i < values.size(); i++) {
        if (prettyIndent)
            indentStr(prettyIndent, indentLevel, s);
        s += values[i].write(prettyIndent, indentLevel + 1);
        if (i != (values.size() - 1)) {
            s += ",";
        }
        if (prettyIndent)
            s += "\n";
    }

    if (prettyIndent)
        indentStr(prettyIndent, indentLevel - 1, s);
    s += "]";
}

void RLPValue::writeObject(unsigned int prettyIndent, unsigned int indentLevel, std::string& s) const
{
    s += "{";
    if (prettyIndent)
        s += "\n";

    for (unsigned int i = 0; i < keys.size(); i++) {
        if (prettyIndent)
            indentStr(prettyIndent, indentLevel, s);
        s += "\"" + json_escape(keys[i]) + "\":";
        if (prettyIndent)
            s += " ";
        s += values.at(i).write(prettyIndent, indentLevel + 1);
        if (i != (values.size() - 1))
            s += ",";
        if (prettyIndent)
            s += "\n";
    }

    if (prettyIndent)
        indentStr(prettyIndent, indentLevel - 1, s);
    s += "}";
}
