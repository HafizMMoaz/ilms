#ifndef ILMS_DATABASE_H
#define ILMS_DATABASE_H

#include "Models.h"
#include "Repository.h"

// Central data store: owns one Repository per record type and exposes them as
// public members. Replaces every global array/count/load*/store* in the old
// code. Pass a single Database around instead of dozens of globals.
class Database
{
public:
    static const int MAX = 50; // storage cap kept from the original program

    Repository<Specimen> specimens{"DataBase/specimen.txt", "S001"};
    Repository<LabDepartment> labDepartments{"DataBase/labDepartment.txt", "LD001"};
    Repository<LabTest> labTests{"DataBase/labTest.txt", "LT001"};
    Repository<Machine> machines{"DataBase/machine.txt", "M001"};
    Repository<Sop> sops{"DataBase/sops.txt", "SP001"};
    Repository<Company> companies{"DataBase/company.txt", "C001"};

    // In-memory only in the original program (no backing file).
    Repository<Package> packages{"DataBase/package.txt", "P001"};
    Repository<Query> queries{"DataBase/query.txt", "Q001"};
    Repository<User> users{"DataBase/user.txt", "U000"};

    // Patient + billing (relational): a patient orders tests on an invoice and
    // pays it off over one or more payments.
    Repository<Patient> patients{"DataBase/patient.txt", "PAT001"};
    Repository<Invoice> invoices{"DataBase/invoice.txt", "INV001"};
    Repository<PatientTest> patientTests{"DataBase/patienttest.txt", "PTT001"};
    Repository<Payment> payments{"DataBase/payment.txt", "PAY001"};

    // Dynamic roles (persisted; seeded with the built-in roles on first run).
    Repository<Role> roles{"DataBase/role.txt", "R000"};

    // Home-sampling service areas (named + optional coordinates).
    Repository<Area> areas{"DataBase/area.txt", "AR001"};

    // Corporate share/settlement ledger (one entry per referred invoice).
    Repository<Settlement> settlements{"DataBase/settlement.txt", "SET001"};

    // Sample dispatches (collection center -> courier -> lab).
    Repository<Dispatch> dispatches{"DataBase/dispatch.txt", "D001"};

    Database();

    // Loads the persisted collections from disk.
    void loadAll();

    // Paths of every persisted data file (used by backup / restore).
    std::vector<std::string> dataFiles() const;

    // Finds a user index by credentials (active accounts only); -1 if no match.
    int findLogin(const std::string &username, const std::string &password) const;

private:
    void seedRoles(); // writes the built-in roles when role.txt is empty
    void backfillRolePermissions(); // refills empty perms on legacy role.txt
    void seedUsers(); // writes the default accounts when user.txt is empty
    void seedAreas(); // writes a few example areas when area.txt is empty
};

#endif // ILMS_DATABASE_H
