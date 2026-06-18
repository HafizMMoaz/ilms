#ifndef ILMS_UTILS_H
#define ILMS_UTILS_H

#include <string>
#include <vector>

// Small free helpers shared across the project (no state of their own).
namespace Utils
{
    // Returns the `index`-th comma separated field (0-based) of a CSV record.
    // e.g. field("S001,Blood,desc", 1) == "Blood"
    std::string field(const std::string &record, int index);

    // Generates the next ID from the last one by incrementing the trailing
    // digits while keeping the alphabetic prefix. e.g. "S009" -> "S010".
    std::string nextId(const std::string &lastId);

    // Joins the items with `sep` (e.g. {"a","b"} , ';' -> "a;b").
    std::string join(const std::vector<std::string> &items, char sep);

    // Splits `text` on `sep`, skipping empty pieces ("a;;b" -> {"a","b"}).
    std::vector<std::string> split(const std::string &text, char sep);

    // Current date + time as "YYYY-MM-DD HH:MM:SS" (used as created/updated stamp).
    std::string now();
}

#endif // ILMS_UTILS_H
