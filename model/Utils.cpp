#include "Utils.h"

#include <ctime>

namespace Utils
{
    std::string now()
    {
        std::time_t t = std::time(0);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return buf;
    }

    std::string field(const std::string &record, int index)
    {
        int commaCount = 0;
        std::string item;
        for (char c : record)
        {
            if (c == ',')
            {
                commaCount++;
                continue;
            }
            if (commaCount == index)
                item += c;
        }
        return item;
    }

    std::string nextId(const std::string &lastId)
    {
        std::string id = lastId;
        int idx = static_cast<int>(id.length()) - 1;
        while (idx >= 1)
        {
            if (id[idx] < '9')
            {
                id[idx] += 1;
                break;
            }
            else
            {
                id[idx] = '0';
                idx--;
            }
        }
        return id;
    }

    std::string join(const std::vector<std::string> &items, char sep)
    {
        std::string out;
        for (size_t i = 0; i < items.size(); i++)
        {
            if (i > 0)
                out += sep;
            out += items[i];
        }
        return out;
    }

    std::vector<std::string> split(const std::string &text, char sep)
    {
        std::vector<std::string> parts;
        std::string current;
        for (char c : text)
        {
            if (c == sep)
            {
                if (!current.empty())
                    parts.push_back(current);
                current.clear();
            }
            else
                current += c;
        }
        if (!current.empty())
            parts.push_back(current);
        return parts;
    }
}
