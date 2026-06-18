#ifndef ILMS_VALIDATOR_H
#define ILMS_VALIDATOR_H

#include <string>
#include <vector>

// Pure input-validation helpers (no program state required).
// The data-dependent checks (e.g. "does this lab-test id exist?") live on the
// App class, since they need the database.
class Validator
{
public:
    // Username may contain only digits, lowercase letters, '_' or '.'.
    static bool username(const std::string &content);

    // Exactly one digit in range 2..5 (number of tests allowed in a package).
    static bool packageTestCount(const std::string &content);

    // Exactly two digits, not starting with 0 (a discount percentage 10..99).
    static bool packageDiscount(const std::string &content);

    // Returns "00000" sentinel if `id` already appears in the first `count`
    // entries of `tests`, otherwise returns `id` unchanged.
    static std::string uniqueIdOrSentinel(const std::vector<std::string> &tests,
                                          const std::string &id, int count);
};

#endif // ILMS_VALIDATOR_H
