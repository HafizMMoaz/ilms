#include "App.h"
#include "Console.h"
#include "Validator.h"
#include "Backup.h"

#include <vector>
#include <string>

using namespace std;

void App::run()
{
    view.init();
    db.loadAll();

    while (true)
    {
        int choice = view.splashScreen(); // 0 = LOGIN, 1 = EXIT
        if (choice == 1)
        {
            if (view.confirm("Are you sure you want to exit ILMS?"))
                break;
            continue;
        }
        if (login())
            sessionLoop();
    }
}

bool App::login()
{
    Credentials cr = view.loginScreen();
    int idx = db.findLogin(cr.username, cr.password);
    if (idx < 0)
    {
        logger.log(cr.username, "failed login attempt");
        view.message("Invalid credentials. Please try again.");
        return false;
    }
    User &u = db.users.at(idx);
    session.start(u.id, u.fname, u.uname, u.role);
    loginTime = Console::date() + " " + Console::time();
    logAction("logged in");
    return true;
}

void App::sessionLoop()
{
    while (session.isActive())
    {
        db.loadAll(); // refresh counts shown on the dashboard

        DashboardStats stats;
        stats.patients = db.patients.count();
        stats.specimens = db.specimens.count();
        stats.labDepartments = db.labDepartments.count();
        stats.labTests = db.labTests.count();
        stats.machines = db.machines.count();
        stats.packages = db.packages.count();
        stats.companies = db.companies.count();

        // The menu is built dynamically (admins get an extra entry), so we
        // dispatch on the chosen label rather than a fixed index.
        std::vector<std::string> menu = {"Dashboard", "Setup", "Patient", "Reports", "Backup"};
        if (isAdmin())
            menu.push_back("Activity Logs");
        menu.push_back("Logout");

        int c = view.dashboardScreen(session.fullName(), stats, menu);
        std::string choice = (c >= 0 && c < (int)menu.size()) ? menu[c] : "Logout";

        if (choice == "Dashboard")
            continue; // redraw
        else if (choice == "Setup")
            setupModule();
        else if (choice == "Patient")
            patientModule();
        else if (choice == "Reports")
        {
            view.clear();
            view.message("Reports module - coming soon.");
        }
        else if (choice == "Backup")
            backupModule();
        else if (choice == "Activity Logs")
            logsModule();
        else if (choice == "Logout")
        {
            view.clear();
            logAction("logged out");
            view.message("Logging out. We will miss you, " + session.fullName() + "!");
            session.end();
        }
    }
}

// ---------------------------------------------------------------------------
// Activity log viewer (admins only): read-only, searchable, with a "Delete
// All" that records who cleared it.
// ---------------------------------------------------------------------------
void App::logsModule()
{
    logAction("opened Activity Logs");
    while (true)
    {
        std::vector<std::string> lines = logger.read();
        bool deleteAll = view.logViewer("ACTIVITY LOGS  (admin: view / search / delete)", lines);
        if (!deleteAll)
            return;

        if (view.confirm("Delete ALL activity logs? This cannot be undone."))
        {
            std::string who = session.userName();
            logger.clear();
            // First line of the fresh log records who deleted the history.
            logger.log(who, "cleared all activity logs");
            view.clear();
            view.message("All logs deleted (recorded as cleared by " + who + ").");
        }
    }
}

// ---------------------------------------------------------------------------
// SETUP menu. Specimen is fully implemented as the reference module; the rest
// are placeholders that will be filled in by copying the specimen pattern.
// ---------------------------------------------------------------------------
void App::setupModule()
{
    logAction("opened Setup");
    while (true)
    {
        view.clear();
        int c = view.menu("SETUP",
                          {"Specimen", "Lab Departments", "Lab Tests", "Packages",
                           "Test Rate List", "Machines", "SOPs", "Corporate", "Back"});
        switch (c)
        {
        case 0: specimenModule(); break;
        case 1: labDepartmentModule(); break;
        case 2: labTestModule(); break;
        case 3: packageModule(); break;
        case 4: rateListView(); break;
        case 5: machineModule(); break;
        case 6: sopModule(); break;
        case 7: companyModule(); break;
        case 8: return;
        default:
            view.message("This section is coming soon.");
        }
    }
}

// ---------------------------------------------------------------------------
// BACKUP / RESTORE : each backup is a timestamped folder under Backup/, so the
// user can keep many and choose which one to restore.
// ---------------------------------------------------------------------------
void App::backupModule()
{
    logAction("opened Backup / Restore");
    const std::string ROOT = "Backup";

    while (true)
    {
        view.clear();
        int c = view.menu("BACKUP / RESTORE", {"Create Backup", "Restore Backup", "Back"});
        if (c == 0)
        {
            std::string name = Backup::create(db.dataFiles(), ROOT);
            if (name.empty())
                view.message("Nothing to back up yet.");
            else
            {
                logAction("created backup " + name);
                view.message("Backup created: " + ROOT + "/" + name);
            }
        }
        else if (c == 1)
        {
            std::vector<std::string> backups = Backup::list(ROOT);
            if (backups.empty())
            {
                view.message("No backups found. Create one first.");
                continue;
            }
            view.clear();
            int sel = view.select("Choose a backup to restore (newest first)", backups);
            if (sel < 0)
                continue;
            if (!view.confirm("Restore '" + backups[sel] + "'? This OVERWRITES current data."))
                continue;

            int n = Backup::restore(db.dataFiles(), ROOT, backups[sel]);
            db.loadAll();
            logAction("restored backup " + backups[sel] + " (" + to_string(n) + " files)");
            view.message("Restored " + to_string(n) + " file(s) from " + backups[sel] + ".");
        }
        else
            return;
    }
}

// ---------------------------------------------------------------------------
// PATIENT area : records (register/manage) and a read-only summary.
// ---------------------------------------------------------------------------
void App::patientModule()
{
    while (true)
    {
        view.clear();
        int c = view.menu("PATIENT", {"Patient Records", "Patient Summary", "Back"});
        if (c == 0)
            patientRecords();
        else if (c == 1)
            patientSummary();
        else
            return;
    }
}

void App::patientRecords()
{
    logAction("opened Patient Records");
    while (true)
    {
        db.patients.load();

        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.patients.count(); i++)
        {
            Patient &p = db.patients.at(i);
            rows.push_back({to_string(i + 1), p.id, p.name, p.contact,
                            p.testCount, p.price, p.balance, p.status});
        }

        // [Add New] = register; row [Edit] = manage that patient; [Delete] = remove.
        RowAction a = view.entityTable(
            "PATIENTS  (Edit a row to add tests / collect specimens)",
            {"Sr", "ID", "Name", "Contact", "Tests", "Price", "Balance", "Status"}, rows);
        switch (a.type)
        {
        case RowAction::Add: registerPatient(); break;
        case RowAction::Edit: managePatient(a.index); break;
        case RowAction::Delete: deletePatient(a.index); break;
        case RowAction::Back: return;
        }
    }
}

// Read-only aggregate view over all registered patients.
void App::patientSummary()
{
    logAction("opened Patient Summary");
    db.patients.load();

    int totalPatients = db.patients.count();
    int totalTests = 0, collected = 0, pending = 0;
    long long billed = 0, received = 0, balance = 0;
    int registered = 0, sampled = 0;

    for (int i = 0; i < totalPatients; i++)
    {
        Patient &p = db.patients.at(i);
        int n = p.testTotal();
        totalTests += n;
        for (int t = 0; t < n; t++)
            (p.specimenTaken[t] == "Y") ? collected++ : pending++;
        try { billed += std::stoll(p.price); } catch (...) {}
        try { received += std::stoll(p.receivedAmount); } catch (...) {}
        try { balance += std::stoll(p.balance); } catch (...) {}
        if (p.status == "SAMPLED") sampled++;
        else registered++;
    }

    vector<vector<string>> rows = {
        {"Total patients", to_string(totalPatients)},
        {"Tests ordered", to_string(totalTests)},
        {"Specimens collected", to_string(collected)},
        {"Specimens pending", to_string(pending)},
        {"Status: REGISTERED", to_string(registered)},
        {"Status: SAMPLED", to_string(sampled)},
        {"Total billed", to_string(billed)},
        {"Total received", to_string(received)},
        {"Total balance due", to_string(balance)},
    };

    view.clear();
    view.table("PATIENT SUMMARY", {"Metric", "Value"}, rows);
    view.message("Summary of " + to_string(totalPatients) + " registered patient(s).");
}

void App::registerPatient()
{
    view.clear();
    if (db.patients.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    // 1) Basic details.
    auto v = view.form("ADD NEW PATIENT",
                       {"Patient Name", "CNIC", "Phone No.", "Gender (M/F/O)",
                        "Age", "Blood Group", "Address"});
    Patient p;
    p.name = v[0];
    p.cnic = v[1];
    p.contact = v[2];
    p.gender = v[3];
    p.age = v[4];
    p.bloodGroup = v[5];
    p.address = v[6];
    p.regDate = Console::date();

    // 2) Sample location.
    int locIdx = view.select("Sample Location",
                             {"LAB", "Home", "Collection Center", "Hospital", "Doctor"});
    std::vector<std::string> locations = {"LAB", "Home", "Collection Center", "Hospital", "Doctor"};
    p.sampleLocation = (locIdx >= 0) ? locations[locIdx] : "LAB";

    // 3) Reference (company / doctor) or self.
    p.reference = "SELF";
    p.discount = "0";
    if (view.confirm("Is this a reference patient (company/doctor)?"))
    {
        if (db.companies.count() > 0)
        {
            int ci = view.select("Select Reference", db.companies.names());
            if (ci >= 0)
            {
                p.reference = db.companies.at(ci).name;
                p.discount = db.companies.at(ci).discount;
            }
        }
        else
            view.message("No company/doctor records found - registering as SELF.");
    }

    // 4) Tests (package or individual). Bails out if nothing exists to add.
    double price = 0;
    if (!choosePatientTests(p, price))
    {
        view.message("No packages or lab tests exist yet. Add some first.");
        return;
    }

    // 5) Apply reference discount to individually-priced tests.
    if (p.reference != "SELF")
    {
        double disc = 0;
        try { disc = std::stod(p.discount); } catch (...) { disc = 0; }
        price = price - (disc * price / 100.0);
    }

    // 6) Billing.
    view.clear();
    view.message("Bill total for this patient: " + to_string(static_cast<int>(price + 0.5)));
    std::string recvStr = view.ask("Amount received");
    int received = 0;
    try { received = std::stoi(recvStr); } catch (...) { received = 0; }
    int balance = static_cast<int>(price + 0.5) - received;

    // 7) Finalise + persist.
    p.id = db.patients.nextId();
    p.price = to_string(static_cast<int>(price + 0.5));
    p.receivedAmount = to_string(received);
    p.balance = to_string(balance > 0 ? balance : 0);
    p.status = "REGISTERED";
    int n = p.testTotal();
    for (int i = 0; i < n; i++)
    {
        p.testStatus[i] = "PEND";    // result pending
        p.specimenTaken[i] = "N";    // specimen not collected yet (do it now or later)
    }

    db.patients.add(p);
    db.patients.store();
    logAction("registered patient " + p.id + " (" + p.name + ")");

    std::string note = "Patient registered: " + p.id + ".  ";
    note += (balance > 0) ? ("Balance due: " + to_string(balance))
                          : ("Returnable: " + to_string(-balance));
    view.message(note);
}

// Returns false only when there is nothing in the system to register at all.
bool App::choosePatientTests(Patient &p, double &price)
{
    price = 0;

    bool wantPackage = view.confirm("Attach a package?");
    if (wantPackage && db.packages.count() > 0)
    {
        int pi = view.select("Select Package", db.packages.names());
        if (pi >= 0)
        {
            Package &pack = db.packages.at(pi);
            p.testCount = pack.testCount;
            int n = p.testTotal();
            for (int i = 0; i < n && i < (int)pack.tests.size(); i++)
                p.tests[i] = pack.tests[i];
            try { price = std::stod(pack.rate); } catch (...) { price = 0; }
            p.discount = pack.disc;
            return true;
        }
    }

    // Individual tests.
    if (db.labTests.count() == 0)
        return false;

    std::string numStr = view.ask("How many tests to add");
    int num = 0;
    try { num = std::stoi(numStr); } catch (...) { num = 0; }
    if (num < 1) num = 1;
    if (num > 20) num = 20;

    // Build a readable "ID - Name" picker list once.
    std::vector<std::string> options;
    for (int i = 0; i < db.labTests.count(); i++)
        options.push_back(db.labTests.at(i).id + " - " + db.labTests.at(i).name);

    int added = 0;
    for (int a = 0; a < num; a++)
    {
        int ti = view.select("Select test " + to_string(a + 1), options);
        if (ti < 0)
            break;
        p.tests[added] = db.labTests.at(ti).id;
        try { price += std::stod(db.labTests.at(ti).rate); } catch (...) {}
        added++;
    }
    p.testCount = to_string(added);
    return true;
}

// ---------------------------------------------------------------------------
// Manage an already-registered patient: view their tests, add a new test, or
// collect a specimen (now or later) for each test.
// ---------------------------------------------------------------------------
void App::managePatient(int i)
{
    if (i < 0 || i >= db.patients.count())
        return;

    while (true)
    {
        Patient &p = db.patients.at(i);
        int n = p.testTotal();

        vector<vector<string>> rows;
        for (int t = 0; t < n; t++)
        {
            string tid = p.tests[t], tname = "?", spec = "?";
            int k = db.labTests.indexOf(tid);
            if (k >= 0)
            {
                tname = db.labTests.at(k).name;
                spec = db.labTests.at(k).specimen;
            }
            string taken = (p.specimenTaken[t] == "Y") ? "YES" : "no";
            rows.push_back({to_string(t + 1), tid, tname, spec, taken, p.testStatus[t]});
        }

        view.clear();
        view.table("PATIENT " + p.id + " - " + p.name + "   (Price " + p.price +
                       ", Balance " + p.balance + ")",
                   {"Sr", "Test", "Name", "Specimen", "Specimen Taken", "Result"}, rows);

        int c = view.menu("Patient actions",
                          {"Add Test", "Collect Specimen", "Back"});
        if (c == 0)
            addTestToPatient(i);
        else if (c == 1)
            collectSpecimen(i);
        else
            return;
    }
}

void App::addTestToPatient(int i)
{
    view.clear();
    Patient &p = db.patients.at(i);

    if (db.labTests.count() == 0)
    {
        view.message("No lab tests defined yet.");
        return;
    }
    int n = p.testTotal();
    if (n >= 20)
    {
        view.message("This patient already has the maximum of 20 tests.");
        return;
    }

    // Offer only tests the patient does not already have.
    vector<string> opts;
    vector<int> mapIdx;
    for (int k = 0; k < db.labTests.count(); k++)
    {
        bool have = false;
        for (int t = 0; t < n; t++)
            if (p.tests[t] == db.labTests.at(k).id)
                have = true;
        if (!have)
        {
            opts.push_back(db.labTests.at(k).id + " - " + db.labTests.at(k).name);
            mapIdx.push_back(k);
        }
    }
    if (opts.empty())
    {
        view.message("All available tests are already added to this patient.");
        return;
    }

    int sel = view.select("Select a test to add", opts);
    if (sel < 0)
        return;
    LabTest &lt = db.labTests.at(mapIdx[sel]);

    // Append the test and re-bill.
    p.tests[n] = lt.id;
    p.testStatus[n] = "PEND";
    p.specimenTaken[n] = "N"; // collect its specimen now or later
    p.testCount = to_string(n + 1);

    double price = 0, rate = 0;
    try { price = std::stod(p.price); } catch (...) {}
    try { rate = std::stod(lt.rate); } catch (...) {}
    price += rate;
    p.price = to_string(static_cast<int>(price + 0.5));

    int received = 0;
    try { received = std::stoi(p.receivedAmount); } catch (...) {}
    int balance = static_cast<int>(price + 0.5) - received;
    p.balance = to_string(balance > 0 ? balance : 0);
    if (p.status == "SAMPLED")
        p.status = "REGISTERED"; // a freshly added test still needs its specimen

    db.patients.update(i);
    logAction("added test " + lt.id + " to patient " + p.id);
    view.message("Added test " + lt.id + " (" + lt.name + ").  New price " +
                 p.price + ", balance " + p.balance + ".");
}

void App::collectSpecimen(int i)
{
    view.clear();
    Patient &p = db.patients.at(i);
    int n = p.testTotal();
    if (n == 0)
    {
        view.message("This patient has no tests.");
        return;
    }

    // Pick which test's specimen to collect.
    vector<string> opts;
    for (int t = 0; t < n; t++)
    {
        string spec = "?";
        int k = db.labTests.indexOf(p.tests[t]);
        if (k >= 0)
            spec = db.labTests.at(k).specimen;
        string st = (p.specimenTaken[t] == "Y") ? "[COLLECTED]" : "[pending]";
        opts.push_back(p.tests[t] + "   specimen: " + spec + "   " + st);
    }
    int sel = view.select("Pick a test to collect its specimen", opts);
    if (sel < 0)
        return;
    if (p.specimenTaken[sel] == "Y")
    {
        view.message("Specimen already collected for this test.");
        return;
    }

    if (!view.confirm("Has the specimen been collected for this test?"))
    {
        view.message("No change - you can collect it later.");
        return;
    }

    // Show the SOP checklist for this specimen and require the technician to
    // confirm they followed it before marking the specimen as collected.
    string spec = "";
    int k = db.labTests.indexOf(p.tests[sel]);
    if (k >= 0)
        spec = db.labTests.at(k).specimen;

    db.sops.load();
    vector<vector<string>> sopRows;
    for (int q = 0; q < db.sops.count(); q++)
        if (db.sops.at(q).specimen == spec)
            sopRows.push_back({db.sops.at(q).id, db.sops.at(q).sop});

    view.clear();
    if (sopRows.empty())
    {
        view.message("No SOPs defined for specimen '" + spec + "'. Marking as collected.");
    }
    else
    {
        view.table("SOPs to follow for specimen '" + spec + "'", {"ID", "Procedure"}, sopRows);
        if (!view.confirm("Did you follow ALL of the above SOPs?"))
        {
            view.message("Specimen NOT marked - please follow the SOPs first.");
            return;
        }
    }

    p.specimenTaken[sel] = "Y";
    bool allDone = true;
    for (int t = 0; t < n; t++)
        if (p.specimenTaken[t] != "Y")
            allDone = false;
    if (allDone)
        p.status = "SAMPLED";

    db.patients.update(i);
    logAction("collected specimen '" + spec + "' for test " + p.tests[sel] +
              " of patient " + p.id);
    view.message("Specimen collected for " + p.tests[sel] + ".");
}

void App::deletePatient(int i)
{
    if (i < 0 || i >= db.patients.count())
        return;
    view.clear();
    string id = db.patients.at(i).id;
    if (!view.confirm("Delete patient " + id + "?"))
        return;
    db.patients.removeAt(i);
    db.patients.store();
    logAction("deleted patient " + id);
    view.message("Patient deleted.");
}

// ---------------------------------------------------------------------------
// Specimen module : list + Add / Edit / Delete.
// ---------------------------------------------------------------------------
void App::specimenModule()
{
    logAction("opened Specimens");
    while (true)
    {
        db.specimens.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.specimens.count(); i++)
        {
            Specimen &s = db.specimens.at(i);
            rows.push_back({to_string(i + 1), s.id, s.name, s.description});
        }

        RowAction a = view.entityTable("SPECIMENS", {"Sr", "ID", "Name", "Description"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addSpecimen(); break;
        case RowAction::Edit: editSpecimen(a.index); break;
        case RowAction::Delete: deleteSpecimen(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addSpecimen()
{
    view.clear();
    if (db.specimens.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    auto v = view.form("ADD NEW SPECIMEN", {"Specimen Name", "Specimen Description"});

    Specimen s;
    s.id = db.specimens.nextId();
    s.name = v[0];
    s.description = v[1];
    db.specimens.add(s);
    db.specimens.store();
    logAction("added specimen " + s.id);
    view.message("Specimen added successfully (" + s.id + ").");
}

void App::editSpecimen(int i)
{
    if (i < 0 || i >= db.specimens.count())
        return;
    view.clear();
    Specimen &s = db.specimens.at(i);
    auto v = view.form("EDIT SPECIMEN " + s.id +
                           "  (current: " + s.name + " / " + s.description + ")",
                       {"New Name", "New Description"});
    s.name = v[0];
    s.description = v[1];
    db.specimens.update(i);
    logAction("updated specimen " + s.id);
    view.message("Specimen updated successfully.");
}

void App::deleteSpecimen(int i)
{
    if (i < 0 || i >= db.specimens.count())
        return;
    view.clear();
    string id = db.specimens.at(i).id;
    if (!view.confirm("Delete specimen " + id + "?"))
        return;
    db.specimens.removeAt(i);
    db.specimens.store();
    logAction("deleted specimen " + id);
    view.message("Specimen deleted successfully.");
}

// ---------------------------------------------------------------------------
// LAB DEPARTMENT module.
// ---------------------------------------------------------------------------
void App::labDepartmentModule()
{
    logAction("opened Lab Departments");
    while (true)
    {
        db.labDepartments.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.labDepartments.count(); i++)
        {
            LabDepartment &d = db.labDepartments.at(i);
            rows.push_back({to_string(i + 1), d.id, d.name, d.date});
        }

        RowAction a = view.entityTable("LAB DEPARTMENTS", {"Sr", "ID", "Name", "Date"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addLabDepartment(); break;
        case RowAction::Edit: editLabDepartment(a.index); break;
        case RowAction::Delete: deleteLabDepartment(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addLabDepartment()
{
    view.clear();
    if (db.labDepartments.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }
    auto v = view.form("ADD NEW LAB DEPARTMENT", {"Lab Department Name"});
    LabDepartment d;
    d.id = db.labDepartments.nextId();
    d.name = v[0];
    d.date = Console::date();
    db.labDepartments.add(d);
    db.labDepartments.store();
    logAction("added lab department " + d.id);
    view.message("Lab department added (" + d.id + ").");
}

void App::editLabDepartment(int i)
{
    if (i < 0 || i >= db.labDepartments.count())
        return;
    view.clear();
    LabDepartment &d = db.labDepartments.at(i);
    auto v = view.form("EDIT LAB DEPARTMENT " + d.id + "  (current: " + d.name + ")",
                       {"New Name"});
    d.name = v[0];
    db.labDepartments.update(i);
    logAction("updated lab department " + d.id);
    view.message("Lab department updated.");
}

void App::deleteLabDepartment(int i)
{
    if (i < 0 || i >= db.labDepartments.count())
        return;
    view.clear();
    string id = db.labDepartments.at(i).id;
    if (!view.confirm("Delete lab department " + id + "?"))
        return;
    db.labDepartments.removeAt(i);
    db.labDepartments.store();
    logAction("deleted lab department " + id);
    view.message("Lab department deleted.");
}

// ---------------------------------------------------------------------------
// LAB TEST module. A lab test ties together a specimen, a lab department and a
// machine, so those must exist before a test can be created.
// ---------------------------------------------------------------------------
void App::labTestModule()
{
    logAction("opened Lab Tests");
    while (true)
    {
        db.labTests.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.labTests.count(); i++)
        {
            LabTest &t = db.labTests.at(i);
            rows.push_back({to_string(i + 1), t.id, t.name, t.rate, t.group, t.specimen});
        }

        RowAction a = view.entityTable("LAB TESTS",
                                       {"Sr", "ID", "Name", "Rate", "Dept", "Specimen"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addLabTest(); break;
        case RowAction::Edit: editLabTest(a.index); break;
        case RowAction::Delete: deleteLabTest(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addLabTest()
{
    view.clear();
    if (db.specimens.count() == 0)
    {
        view.message("Add a specimen first - every test needs one.");
        return;
    }
    if (db.labDepartments.count() == 0)
    {
        view.message("Add a lab department first.");
        return;
    }
    if (db.machines.count() == 0)
    {
        view.message("Add a machine first.");
        return;
    }
    if (db.labTests.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    auto v = view.form("ADD NEW LAB TEST",
                       {"Test Name", "Rate", "Result Unit (e.g. mg/dl)",
                        "Frequency (daily/weekly/...)", "Delivery Time (e.g. 2Hrs)", "Comments"});

    int si = view.select("Specimen required for this test", db.specimens.names());
    if (si < 0) return;
    int di = view.select("Lab Department / Group", db.labDepartments.names());
    if (di < 0) return;
    int mi = view.select("Machine used", db.machines.names());
    if (mi < 0) return;

    LabTest t;
    t.id = db.labTests.nextId();
    t.name = v[0];
    t.rate = v[1];
    t.unit = v[2];
    t.freq = v[3];
    t.time = v[4];
    t.comments = v[5];
    t.specimen = db.specimens.at(si).name;
    t.group = db.labDepartments.at(di).name;
    t.machine = db.machines.at(mi).name;
    db.labTests.add(t);
    db.labTests.store();
    logAction("added lab test " + t.id + " (" + t.name + ")");
    view.message("Lab test added (" + t.id + ").");
}

void App::editLabTest(int i)
{
    if (i < 0 || i >= db.labTests.count())
        return;
    view.clear();
    LabTest &t = db.labTests.at(i);

    auto v = view.form("EDIT LAB TEST " + t.id + "  (name " + t.name + ", rate " + t.rate + ")",
                       {"New Name", "New Rate", "New Unit", "New Frequency",
                        "New Delivery Time", "New Comments"});
    int si = view.select("Specimen (current: " + t.specimen + ")", db.specimens.names());
    int di = view.select("Department (current: " + t.group + ")", db.labDepartments.names());
    int mi = view.select("Machine (current: " + t.machine + ")", db.machines.names());

    t.name = v[0];
    t.rate = v[1];
    t.unit = v[2];
    t.freq = v[3];
    t.time = v[4];
    t.comments = v[5];
    if (si >= 0) t.specimen = db.specimens.at(si).name;
    if (di >= 0) t.group = db.labDepartments.at(di).name;
    if (mi >= 0) t.machine = db.machines.at(mi).name;
    db.labTests.update(i);
    logAction("updated lab test " + t.id);
    view.message("Lab test updated.");
}

void App::deleteLabTest(int i)
{
    if (i < 0 || i >= db.labTests.count())
        return;
    view.clear();
    string id = db.labTests.at(i).id;
    if (!view.confirm("Delete lab test " + id + "?"))
        return;
    db.labTests.removeAt(i);
    db.labTests.store();
    logAction("deleted lab test " + id);
    view.message("Lab test deleted.");
}

// ---------------------------------------------------------------------------
// MACHINE module.
// ---------------------------------------------------------------------------
void App::machineModule()
{
    logAction("opened Machines");
    while (true)
    {
        db.machines.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.machines.count(); i++)
        {
            Machine &m = db.machines.at(i);
            rows.push_back({to_string(i + 1), m.id, m.name, m.description, m.quantity});
        }

        RowAction a = view.entityTable("MACHINES",
                                       {"Sr", "ID", "Name", "Description", "Qty"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addMachine(); break;
        case RowAction::Edit: editMachine(a.index); break;
        case RowAction::Delete: deleteMachine(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addMachine()
{
    view.clear();
    if (db.machines.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }
    auto v = view.form("ADD NEW MACHINE",
                       {"Machine Name", "Description", "Quantity"});
    Machine m;
    m.id = db.machines.nextId();
    m.name = v[0];
    m.description = v[1];
    m.quantity = v[2];
    db.machines.add(m);
    db.machines.store();
    logAction("added machine " + m.id);
    view.message("Machine added (" + m.id + ").");
}

void App::editMachine(int i)
{
    if (i < 0 || i >= db.machines.count())
        return;
    view.clear();
    Machine &m = db.machines.at(i);
    auto v = view.form("EDIT MACHINE " + m.id,
                       {"New Name", "New Description", "New Quantity"});
    m.name = v[0];
    m.description = v[1];
    m.quantity = v[2];
    db.machines.update(i);
    logAction("updated machine " + m.id);
    view.message("Machine updated.");
}

void App::deleteMachine(int i)
{
    if (i < 0 || i >= db.machines.count())
        return;
    view.clear();
    string id = db.machines.at(i).id;
    if (!view.confirm("Delete machine " + id + "?"))
        return;
    db.machines.removeAt(i);
    db.machines.store();
    logAction("deleted machine " + id);
    view.message("Machine deleted.");
}

// ---------------------------------------------------------------------------
// PACKAGE module. A package bundles 2-5 lab tests at a discounted rate.
// ---------------------------------------------------------------------------
void App::packageModule()
{
    logAction("opened Packages");
    while (true)
    {
        db.packages.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.packages.count(); i++)
        {
            Package &p = db.packages.at(i);
            rows.push_back({to_string(i + 1), p.id, p.name, p.testCount,
                            p.rate, p.disc + "%"});
        }

        RowAction a = view.entityTable("PACKAGES",
                                       {"Sr", "ID", "Name", "Tests", "Rate", "Disc"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addPackage(); break;
        case RowAction::Edit: editPackage(a.index); break;
        case RowAction::Delete: deletePackage(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addPackage()
{
    view.clear();
    if (db.labTests.count() < 2)
    {
        view.message("Add at least two lab tests before creating a package.");
        return;
    }
    if (db.packages.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    auto v = view.form("ADD NEW PACKAGE", {"Package Name"});

    // How many tests (2..min(5, available)).
    int maxTests = db.labTests.count();
    if (maxTests > 5)
        maxTests = 5;
    vector<string> countOptions;
    for (int k = 2; k <= maxTests; k++)
        countOptions.push_back(to_string(k));
    int ci = view.select("How many tests in this package?", countOptions);
    int count = (ci >= 0) ? (ci + 2) : 2;

    // Pick each test (no duplicates).
    Package p;
    double price = 0;
    int added = 0;
    for (int a = 0; a < count; a++)
    {
        vector<string> opts;
        vector<int> mapIdx;
        for (int k = 0; k < db.labTests.count(); k++)
        {
            bool taken = false;
            for (int t = 0; t < added; t++)
                if (p.tests[t] == db.labTests.at(k).id)
                    taken = true;
            if (!taken)
            {
                opts.push_back(db.labTests.at(k).id + " - " + db.labTests.at(k).name);
                mapIdx.push_back(k);
            }
        }
        if (opts.empty())
            break;
        int sel = view.select("Select test " + to_string(a + 1), opts);
        if (sel < 0)
            break;
        LabTest &lt = db.labTests.at(mapIdx[sel]);
        p.tests[added] = lt.id;
        try { price += std::stod(lt.rate); } catch (...) {}
        added++;
    }
    if (added < 2)
    {
        view.message("A package needs at least two tests. Cancelled.");
        return;
    }

    // Discount (10..99). Re-ask until valid.
    string disc = view.ask("Enter discount % (10-99)");
    while (!Validator::packageDiscount(disc))
        disc = view.ask("Invalid. Enter discount % (10-99)");

    double discounted = price - (std::stod(disc) * price / 100.0);

    p.id = db.packages.nextId();
    p.name = v[0];
    p.testCount = to_string(added);
    p.disc = disc;
    p.rate = to_string(static_cast<int>(discounted + 0.5));
    db.packages.add(p);
    db.packages.store();
    logAction("added package " + p.id + " (" + p.name + ")");
    view.message("Package added (" + p.id + "), rate " + p.rate + ".");
}

void App::editPackage(int i)
{
    if (i < 0 || i >= db.packages.count())
        return;
    view.clear();
    Package &p = db.packages.at(i);

    auto v = view.form("EDIT PACKAGE " + p.id + "  (current: " + p.name +
                           ", disc " + p.disc + "%)",
                       {"New Name"});
    string disc = view.ask("New discount % (10-99)");
    while (!Validator::packageDiscount(disc))
        disc = view.ask("Invalid. New discount % (10-99)");

    // Recompute the rate from the package's tests at their current prices.
    double price = 0;
    for (const string &testId : p.tests)
    {
        if (testId.empty())
            continue;
        int k = db.labTests.indexOf(testId);
        if (k >= 0)
            try { price += std::stod(db.labTests.at(k).rate); } catch (...) {}
    }
    double discounted = price - (std::stod(disc) * price / 100.0);

    p.name = v[0];
    p.disc = disc;
    p.rate = to_string(static_cast<int>(discounted + 0.5));
    db.packages.update(i);
    logAction("updated package " + p.id);
    view.message("Package updated, new rate " + p.rate + ".");
}

void App::deletePackage(int i)
{
    if (i < 0 || i >= db.packages.count())
        return;
    view.clear();
    string id = db.packages.at(i).id;
    if (!view.confirm("Delete package " + id + "?"))
        return;
    db.packages.removeAt(i);
    db.packages.store();
    logAction("deleted package " + id);
    view.message("Package deleted.");
}

// ---------------------------------------------------------------------------
// SOP module. Each SOP is a step in the checklist for collecting a specimen,
// so adding one starts by picking the specimen it belongs to.
// ---------------------------------------------------------------------------
void App::sopModule()
{
    logAction("opened SOPs");
    while (true)
    {
        db.sops.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.sops.count(); i++)
        {
            Sop &s = db.sops.at(i);
            rows.push_back({to_string(i + 1), s.id, s.specimen, s.sop, s.date});
        }

        RowAction a = view.entityTable("SOPs (per specimen)",
                                       {"Sr", "ID", "Specimen", "SOP", "Date"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addSop(); break;
        case RowAction::Edit: editSop(a.index); break;
        case RowAction::Delete: deleteSop(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addSop()
{
    view.clear();
    if (db.specimens.count() == 0)
    {
        view.message("Add a specimen first - every SOP belongs to a specimen.");
        return;
    }
    if (db.sops.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    int si = view.select("Which specimen is this SOP for?", db.specimens.names());
    if (si < 0)
        return;

    auto v = view.form("ADD SOP for specimen '" + db.specimens.at(si).name + "'",
                       {"SOP / Procedure step"});

    Sop s;
    s.id = db.sops.nextId();
    s.specimen = db.specimens.at(si).name;
    s.sop = v[0];
    s.date = Console::date();
    db.sops.add(s);
    db.sops.store();
    logAction("added SOP " + s.id + " for specimen " + s.specimen);
    view.message("SOP added (" + s.id + ") for " + s.specimen + ".");
}

void App::editSop(int i)
{
    if (i < 0 || i >= db.sops.count())
        return;
    view.clear();
    Sop &s = db.sops.at(i);

    // Let the user re-point the SOP at a specimen, then edit the text.
    int si = (db.specimens.count() > 0)
                 ? view.select("Specimen (current: " + s.specimen + ")", db.specimens.names())
                 : -1;

    auto v = view.form("EDIT SOP " + s.id + "  (current: " + s.sop + ")",
                       {"New SOP / Procedure step"});

    if (si >= 0)
        s.specimen = db.specimens.at(si).name;
    s.sop = v[0];
    db.sops.update(i);
    logAction("updated SOP " + s.id);
    view.message("SOP updated.");
}

void App::deleteSop(int i)
{
    if (i < 0 || i >= db.sops.count())
        return;
    view.clear();
    string id = db.sops.at(i).id;
    if (!view.confirm("Delete SOP " + id + "?"))
        return;
    db.sops.removeAt(i);
    db.sops.store();
    logAction("deleted SOP " + id);
    view.message("SOP deleted.");
}

// ---------------------------------------------------------------------------
// CORPORATE module : referring companies / hospitals / doctors.
// ---------------------------------------------------------------------------
void App::companyModule()
{
    logAction("opened Corporate");
    while (true)
    {
        db.companies.load();

        vector<vector<string>> rows;
        for (int i = 0; i < db.companies.count(); i++)
        {
            Company &c = db.companies.at(i);
            rows.push_back({to_string(i + 1), c.id, c.name, c.type, c.contact,
                            c.discount + "%", c.commisionPerc + "%"});
        }

        RowAction a = view.entityTable("CORPORATE (Companies & Doctors)",
                                       {"Sr", "ID", "Name", "Type", "Contact", "Disc", "Comm"}, rows);
        switch (a.type)
        {
        case RowAction::Add: addCompany(); break;
        case RowAction::Edit: editCompany(a.index); break;
        case RowAction::Delete: deleteCompany(a.index); break;
        case RowAction::Back: return;
        }
    }
}

void App::addCompany()
{
    view.clear();
    if (db.companies.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }
    auto v = view.form("ADD NEW COMPANY / DOCTOR",
                       {"Name", "Contact", "Discount %", "Commission %"});
    vector<string> types = {"Collection Center", "Hospital", "Doctor"};
    int ti = view.select("Company Type", types);

    Company c;
    c.id = db.companies.nextId();
    c.name = v[0];
    c.contact = v[1];
    c.discount = v[2];
    c.commisionPerc = v[3];
    c.type = (ti >= 0) ? types[ti] : "Doctor";
    c.commision = "0";
    c.patients = "0";
    db.companies.add(c);
    db.companies.store();
    logAction("added company " + c.id + " (" + c.name + ")");
    view.message("Company added (" + c.id + ").");
}

void App::editCompany(int i)
{
    if (i < 0 || i >= db.companies.count())
        return;
    view.clear();
    Company &c = db.companies.at(i);
    auto v = view.form("EDIT COMPANY " + c.id + "  (current: " + c.name + ")",
                       {"New Name", "New Contact", "New Discount %", "New Commission %"});
    vector<string> types = {"Collection Center", "Hospital", "Doctor"};
    int ti = view.select("Type (current: " + c.type + ")", types);

    c.name = v[0];
    c.contact = v[1];
    c.discount = v[2];
    c.commisionPerc = v[3];
    if (ti >= 0)
        c.type = types[ti];
    db.companies.update(i);
    logAction("updated company " + c.id);
    view.message("Company updated.");
}

void App::deleteCompany(int i)
{
    if (i < 0 || i >= db.companies.count())
        return;
    view.clear();
    string id = db.companies.at(i).id;
    if (!view.confirm("Delete company " + id + "?"))
        return;
    db.companies.removeAt(i);
    db.companies.store();
    logAction("deleted company " + id);
    view.message("Company deleted.");
}

// ---------------------------------------------------------------------------
// TEST RATE LIST : read-only price list of all lab tests.
// ---------------------------------------------------------------------------
void App::rateListView()
{
    logAction("opened Test Rate List");
    db.labTests.load();

    vector<vector<string>> rows;
    for (int i = 0; i < db.labTests.count(); i++)
    {
        LabTest &t = db.labTests.at(i);
        rows.push_back({to_string(i + 1), t.id, t.name, t.rate});
    }

    view.clear();
    view.table("TEST RATE LIST", {"Sr", "ID", "Test Name", "Rate"}, rows);
    view.message(to_string(db.labTests.count()) + " test(s) listed.");
}
