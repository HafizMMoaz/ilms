#ifndef ILMS_REPOSITORY_H
#define ILMS_REPOSITORY_H

#include <string>
#include <vector>
#include <fstream>
#include "Utils.h"

// ---------------------------------------------------------------------------
// Generic collection + persistence for one kind of record.
//
// The record type T must expose:
//   * a public  std::string id;  member
//   * std::string toCSV() const;            (only needed if store() is used)
//   * void fromCSV(const std::string&);     (only needed if load()  is used)
//
// This single template replaces the dozen near-identical load*/store* global
// functions and the parallel arrays that backed them.
// ---------------------------------------------------------------------------
template <typename T>
class Repository
{
    std::vector<T> data;
    std::string filePath;
    std::string seedId; // first id handed out when the collection is empty

public:
    Repository(std::string path, std::string seed)
        : filePath(std::move(path)), seedId(std::move(seed)) {}

    // ---- persistence ------------------------------------------------------
    void load()
    {
        data.clear();
        std::ifstream file(filePath);
        std::string record;
        while (std::getline(file, record))
        {
            if (record.empty())
                continue;
            T obj;
            obj.fromCSV(record);
            data.push_back(obj);
        }
    }

    void store() const
    {
        std::ofstream file(filePath);
        for (const auto &obj : data)
            file << obj.toCSV() << std::endl;
    }

    // ---- access -----------------------------------------------------------
    const std::string &path() const { return filePath; }
    int count() const { return static_cast<int>(data.size()); }
    bool empty() const { return data.empty(); }
    T &at(int i) { return data[i]; }
    const T &at(int i) const { return data[i]; }
    std::vector<T> &all() { return data; }

    // ---- mutation ---------------------------------------------------------
    // Adds a record, stamping created/updated automatically.
    void add(T obj)
    {
        if (obj.createdAt.empty())
            obj.createdAt = Utils::now();
        obj.updatedAt = obj.createdAt;
        data.push_back(obj);
    }

    // Marks record i as just-updated and persists the collection.
    void update(int i)
    {
        if (i >= 0 && i < count())
        {
            data[i].updatedAt = Utils::now();
            store();
        }
    }

    int indexOf(const std::string &id) const
    {
        for (int i = 0; i < count(); i++)
            if (data[i].id == id)
                return i;
        return -1;
    }

    void removeAt(int i)
    {
        if (i >= 0 && i < count())
            data.erase(data.begin() + i);
    }

    bool removeById(const std::string &id)
    {
        int i = indexOf(id);
        if (i < 0)
            return false;
        removeAt(i);
        return true;
    }

    // Next id to assign to a new record.
    std::string nextId() const
    {
        if (data.empty())
            return seedId;
        return Utils::nextId(data.back().id);
    }

    // ---- helpers for selection menus -------------------------------------
    std::vector<std::string> names() const
    {
        std::vector<std::string> out;
        for (const auto &obj : data)
            out.push_back(obj.name);
        return out;
    }

    std::vector<std::string> ids() const
    {
        std::vector<std::string> out;
        for (const auto &obj : data)
            out.push_back(obj.id);
        return out;
    }
};

#endif // ILMS_REPOSITORY_H
