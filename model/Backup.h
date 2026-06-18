#ifndef ILMS_BACKUP_H
#define ILMS_BACKUP_H

#include <string>
#include <vector>

// Timestamped, multi-slot backup / restore for the CSV data files.
// Each backup is a subfolder of `rootDir` named "backup_YYYY-MM-DD_HH-MM-SS".
namespace Backup
{
    // Creates a new timestamped subfolder under `rootDir` and copies every file
    // into it. Returns the new subfolder name, or "" if nothing was backed up.
    std::string create(const std::vector<std::string> &files, const std::string &rootDir);

    // Existing backup subfolder names under `rootDir`, newest first.
    std::vector<std::string> list(const std::string &rootDir);

    // Restores files from rootDir/subfolder back to their original paths.
    // Returns the number of files restored.
    int restore(const std::vector<std::string> &files,
                const std::string &rootDir, const std::string &subfolder);
}

#endif // ILMS_BACKUP_H
