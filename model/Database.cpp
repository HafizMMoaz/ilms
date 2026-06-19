#include "Database.h"

Database::Database()
{
    // Seed the three built-in accounts (these were hard-coded globals before).
    users.add(User("U000", "Hafiz", "hafiz", "1234", "R000", true));
    users.add(User("U001", "Muhammad", "muhammad", "5678", "R007", true));
    users.add(User("U002", "Moaz", "moaz", "1452", "R009", true));
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
}

std::vector<std::string> Database::dataFiles() const
{
    return {specimens.path(), labDepartments.path(), labTests.path(),
            machines.path(), sops.path(), companies.path(),
            packages.path(), patients.path(), invoices.path(),
            patientTests.path(), payments.path()};
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
