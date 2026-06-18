#include "Logger.h"
#include "Utils.h"

#include <fstream>

void Logger::log(const std::string &user, const std::string &action)
{
    std::ofstream out(file, std::ios::app);
    if (!out)
        return;
    out << "[" << Utils::now() << "] " << (user.empty() ? "system" : user)
        << ": " << action << "\n";
}

std::vector<std::string> Logger::read() const
{
    std::vector<std::string> lines;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
        if (!line.empty())
            lines.push_back(line);
    return lines;
}

void Logger::clear()
{
    std::ofstream out(file, std::ios::trunc); // open + truncate empties the file
}
