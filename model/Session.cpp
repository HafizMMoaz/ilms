#include "Session.h"

bool Session::start(const std::string &id, const std::string &fullName,
                    const std::string &userName, const std::string &userRole)
{
    if (active)
        return false;
    userId = id;
    fname = fullName;
    uname = userName;
    role = userRole;
    active = true;
    return true;
}

bool Session::end()
{
    if (!active)
        return false;
    userId.clear();
    fname.clear();
    uname.clear();
    role.clear();
    active = false;
    return true;
}
