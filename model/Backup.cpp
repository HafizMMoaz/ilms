#include "Backup.h"

#include <fstream>
#include <algorithm>
#include <ctime>
#include <direct.h> // _mkdir (MinGW)
#include <dirent.h> // opendir/readdir (MinGW)

namespace
{
    std::string baseName(const std::string &p)
    {
        std::size_t s = p.find_last_of("/\\");
        return (s == std::string::npos) ? p : p.substr(s + 1);
    }

    // Binary copy; returns false (and writes nothing) if the source is missing.
    bool copyFile(const std::string &src, const std::string &dst)
    {
        std::ifstream in(src, std::ios::binary);
        if (!in)
            return false;
        std::ofstream out(dst, std::ios::binary | std::ios::trunc);
        if (!out)
            return false;
        out << in.rdbuf();
        return true;
    }

    // Filesystem-safe timestamp, e.g. "backup_2026-06-19_00-48-30".
    std::string stampedFolderName()
    {
        std::time_t t = std::time(0);
        char buf[40];
        std::strftime(buf, sizeof(buf), "backup_%Y-%m-%d_%H-%M-%S", std::localtime(&t));
        return buf;
    }
}

namespace Backup
{
    std::string create(const std::vector<std::string> &files, const std::string &rootDir)
    {
        _mkdir(rootDir.c_str()); // harmless if it already exists
        std::string sub = stampedFolderName();
        std::string dir = rootDir + "/" + sub;
        _mkdir(dir.c_str());

        int copied = 0;
        for (const auto &f : files)
            if (copyFile(f, dir + "/" + baseName(f)))
                copied++;

        return copied > 0 ? sub : "";
    }

    std::vector<std::string> list(const std::string &rootDir)
    {
        std::vector<std::string> out;
        DIR *d = opendir(rootDir.c_str());
        if (!d)
            return out;
        while (struct dirent *e = readdir(d))
        {
            std::string name = e->d_name;
            if (name.rfind("backup_", 0) == 0) // our backup subfolders
                out.push_back(name);
        }
        closedir(d);
        // Names sort lexically == chronologically; reverse for newest-first.
        std::sort(out.rbegin(), out.rend());
        return out;
    }

    int restore(const std::vector<std::string> &files,
                const std::string &rootDir, const std::string &subfolder)
    {
        std::string dir = rootDir + "/" + subfolder;
        int restored = 0;
        for (const auto &f : files)
            if (copyFile(dir + "/" + baseName(f), f))
                restored++;
        return restored;
    }
}
