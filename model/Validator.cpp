#include "Validator.h"

bool Validator::username(const std::string &content)
{
    for (char c : content)
    {
        bool ok = (c >= '0' && c <= '9') ||
                  (c >= 'a' && c <= 'z') ||
                  c == '_' || c == '.';
        if (!ok)
            return false;
    }
    return true;
}

bool Validator::packageTestCount(const std::string &content)
{
    int idx = 0;
    while (idx < static_cast<int>(content.length()))
    {
        char c = content[idx];
        if (!(c >= '2' && c <= '5'))
            return false;
        if (c == ' ')
            return false;
        idx++;
    }
    if (idx != 1)
        return false;
    return true;
}

bool Validator::packageDiscount(const std::string &content)
{
    int idx = 0;
    while (idx < static_cast<int>(content.length()))
    {
        char c = content[idx];
        if (!(c >= '0' && c <= '9'))
            return false;
        if (c == ' ')
            return false;
        idx++;
    }
    if (idx != 2 || content[0] == '0')
        return false;
    return true;
}

std::string Validator::uniqueIdOrSentinel(const std::vector<std::string> &tests,
                                          const std::string &id, int count)
{
    for (int x = 0; x < count; x++)
    {
        if (id == tests[x])
            return "00000";
    }
    return id;
}
