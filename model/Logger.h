#ifndef ILMS_LOGGER_H
#define ILMS_LOGGER_H

#include <string>
#include <vector>

// Appends a timestamped activity trail to a log file (default logs.txt):
//   [YYYY-MM-DD HH:MM:SS] <user>: <action>
// Records who did what - logins, screen opens, and every add/update/delete.
class Logger
{
    std::string file;

public:
    explicit Logger(std::string filePath = "Database/logs.txt") : file(std::move(filePath)) {}

    void log(const std::string &user, const std::string &action);

    // Returns every line currently in the log file (for the admin viewer).
    std::vector<std::string> read() const;

    // Empties the log file (the deletion itself is then logged by the caller).
    void clear();
};

#endif // ILMS_LOGGER_H
