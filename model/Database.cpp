#include "Database.h"

Database::Database() {}

void Database::seedUsers()
{
    // The three built-in accounts (created once when user.txt is empty).
    users.add(User("U000", "Hafiz", "hafiz", "1234", "R000", true));
    users.add(User("U001", "Muhammad", "muhammad", "5678", "R007", true));
    users.add(User("U002", "Moaz", "moaz", "1452", "R009", true));
    users.store();
}

void Database::seedAreas()
{
    static const char *names[] = {"Nabipura", "Mughalpura", "Amir Town"};
    for (int i = 0; i < 3; i++)
    {
        Area a;
        a.id = areas.nextId();
        a.name = names[i];
        areas.add(a);
    }
    areas.store();
}

void Database::loadAll()
{
    specimens.load();
    labDepartments.load();
    labTests.load();
    machines.load();
    sops.load();
    companies.load();
    packages.load();
    patients.load();
    invoices.load();
    patientTests.load();
    payments.load();
    roles.load();
    if (roles.empty())
        seedRoles();
    users.load();
    if (users.empty())
        seedUsers();
    areas.load();
    if (areas.empty())
        seedAreas();
    settlements.load();
}

void Database::seedRoles()
{
    static const char *names[] = {
        "Receptionist", "Phlebotomist", "Technician", "Companies & Doctors",
        "Courier", "Home Sampling", "Collection Center", "Manager", "Admin", "Super Admin"};
    for (int i = 0; i < 10; i++)
    {
        Role r;
        r.id = "R00" + std::to_string(i); // R000 .. R009
        r.name = names[i];
        r.fixed = "Y";
        roles.add(r);
    }
    roles.store();
}

std::vector<std::string> Database::dataFiles() const
{
    return {specimens.path(), labDepartments.path(), labTests.path(),
            machines.path(), sops.path(), companies.path(),
            packages.path(), patients.path(), invoices.path(),
            patientTests.path(), payments.path(), roles.path(),
            users.path(), areas.path(), settlements.path()};
}

int Database::findLogin(const std::string &username, const std::string &password) const
{
    for (int i = 0; i < users.count(); i++)
    {
        const User &u = users.at(i);
        if (u.uname == username && u.password == password && u.active)
            return i;
    }
    return -1;
}
