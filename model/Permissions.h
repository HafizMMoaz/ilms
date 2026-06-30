#ifndef ILMS_PERMISSIONS_H
#define ILMS_PERMISSIONS_H

#include <string>
#include <vector>
#include <utility>

// The RBAC permission catalogue. A role stores a ';'-joined set of these keys
// (Role::permissions); the controller's can() checks membership. Kept in one
// place so seeding (Database) and the editor (App) agree.
//
// File-based for now; when the data layer moves to a DBMS this same key set
// maps to a role_permissions table.
namespace Permissions
{
    // Ordered (key, human label) catalogue shown in the editor.
    inline std::vector<std::pair<std::string, std::string>> catalog()
    {
        return {
            {"setup", "Setup"},
            {"patient.records", "Patient: Records / Register"},
            {"patient.sample_receiving", "Patient: Sample Receiving"},
            {"patient.result_entry", "Patient: Result Entry"},
            {"patient.summary", "Patient: Summary"},
            {"home_sampling", "Home Sampling"},
            {"receive_samples", "Receive Samples (dispatch)"},
            {"doctor_portal", "Doctor / Corporate Portal"},
            {"courier", "Courier"},
            {"collection_center", "Collection Center"},
            {"reports", "Reports"},
            {"backup", "Backup / Restore"},
            {"users", "Users management"},
            {"settlements", "Settlements"},
            {"logs", "Activity Logs"},
            {"roles", "Roles management"},
        };
    }

    // True if `key` appears in a ';'-joined permission list. The single place
    // everyone (seeding, editor, controller's can()) agrees on the format.
    inline bool has(const std::string &perms, const std::string &key)
    {
        size_t pos = 0;
        while (pos <= perms.size())
        {
            size_t end = perms.find(';', pos);
            if (end == std::string::npos)
                end = perms.size();
            if (perms.compare(pos, end - pos, key) == 0)
                return true;
            pos = end + 1;
        }
        return false;
    }

    inline std::string joinAll()
    {
        std::string s;
        for (const auto &p : catalog())
        {
            if (!s.empty())
                s += ";";
            s += p.first;
        }
        return s;
    }

    // Default permission set for a built-in role (returns a ';'-joined key list).
    // Custom roles default to none (Dashboard only) until a Super Admin grants.
    inline std::string defaultsFor(const std::string &role)
    {
        if (role == "Super Admin")
            return joinAll();
        if (role == "Admin")
        {
            std::string s; // everything except role management
            for (const auto &p : catalog())
            {
                if (p.first == "roles")
                    continue;
                if (!s.empty())
                    s += ";";
                s += p.first;
            }
            return s;
        }
        if (role == "Manager")
            return "setup;patient.records;patient.sample_receiving;patient.result_entry;"
                   "patient.summary;home_sampling;receive_samples;reports;settlements";
        if (role == "Receptionist")
            return "patient.records;patient.summary;home_sampling";
        if (role == "Phlebotomist")
            return "patient.sample_receiving;receive_samples";
        if (role == "Technician")
            return "patient.result_entry;receive_samples";
        if (role == "Companies & Doctors")
            return "doctor_portal";
        if (role == "Courier")
            return "courier";
        if (role == "Home Sampling")
            return "home_sampling";
        if (role == "Collection Center")
            return "collection_center";
        return ""; // custom role -> Dashboard only until granted
    }
}

#endif // ILMS_PERMISSIONS_H
