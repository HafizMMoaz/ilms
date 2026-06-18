#ifndef ILMS_SESSION_H
#define ILMS_SESSION_H

#include <string>

// Holds information about the currently logged-in user.
// Replaces the old global _SESSION[4] array + session()/sessionStart() funcs.
class Session
{
    std::string userId;
    std::string fname;
    std::string uname;
    std::string role;
    bool active = false;

public:
    // Begins a session if none is active. Returns false if already logged in.
    bool start(const std::string &id, const std::string &fullName,
               const std::string &userName, const std::string &userRole);

    // Ends the current session. Returns false if none was active.
    bool end();

    bool isActive() const { return active; }

    // Accessors mirroring the old session("id"/"fname"/"uname"/"role") calls.
    std::string id() const { return userId; }
    std::string fullName() const { return fname; }
    std::string userName() const { return uname; }
    std::string userRole() const { return role; }
};

#endif // ILMS_SESSION_H
