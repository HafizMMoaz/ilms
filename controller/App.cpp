#include "App.h"
#include "Console.h"
#include "Validator.h"
#include "Backup.h"
#include "Export.h"

#include <vector>
#include <string>
#include <algorithm>

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

        // The menu is built from the user's role (RBAC), then dispatched by label.
        std::vector<std::string> menu = menuForRole();
        int c = view.dashboardScreen(session.fullName() + "  [" + currentRoleName() + "]",
                                     stats, menu);
        std::string choice = (c >= 0 && c < (int)menu.size()) ? menu[c] : "Logout";

        if (choice == "Dashboard")
            continue; // redraw
        else if (choice == "Setup")
            setupModule();
        else if (choice == "Patient")
            patientModule();
        else if (choice == "Home Sampling")
            homeSamplingModule();
        else if (choice == "Doctor Portal")
            doctorPortal();
        else if (choice == "Courier")
            courierModule();
        else if (choice == "Collection Center")
            collectionCenterModule();
        else if (choice == "Receive Samples")
            receiveDispatches();
        else if (choice == "Reports")
            reportsModule();
        else if (choice == "Backup")
            backupModule();
        else if (choice == "Users")
            usersModule();
        else if (choice == "Settlements")
            settlementsModule();
        else if (choice == "Activity Logs")
            logsModule();
        else if (choice == "Roles")
            rolesModule();
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
// RBAC: each role sees only the modules it needs. Built-in roles have fixed
// permission sets; custom roles get the Dashboard only (a Super Admin can grant
// more later). Returns the top-level menu labels for the current role.
// ---------------------------------------------------------------------------
std::string App::currentRoleName()
{
    int k = db.roles.indexOf(session.userRole());
    return (k >= 0) ? db.roles.at(k).name : session.userRole();
}

std::vector<std::string> App::menuForRole()
{
    std::string r = currentRoleName();
    std::vector<std::string> m = {"Dashboard"};

    if (r == "Super Admin" || r == "Admin")
    {
        m.push_back("Setup");
        m.push_back("Patient");
        m.push_back("Home Sampling");
        m.push_back("Receive Samples");
        m.push_back("Reports");
        m.push_back("Backup");
        m.push_back("Users");
        m.push_back("Settlements");
        m.push_back("Activity Logs");
        if (r == "Super Admin")
            m.push_back("Roles");
    }
    else if (r == "Manager")
    {
        m.push_back("Setup");
        m.push_back("Patient");
        m.push_back("Home Sampling");
        m.push_back("Receive Samples");
        m.push_back("Reports");
        m.push_back("Settlements");
    }
    else if (r == "Receptionist")
    {
        m.push_back("Patient");
        m.push_back("Home Sampling");
    }
    else if (r == "Phlebotomist" || r == "Technician")
    {
        m.push_back("Patient");          // sample receiving / result entry
        m.push_back("Receive Samples");  // receive courier dispatches
    }
    else if (r == "Home Sampling")
    {
        m.push_back("Home Sampling");
    }
    else if (r == "Companies & Doctors")
    {
        m.push_back("Doctor Portal");
    }
    else if (r == "Courier")
    {
        m.push_back("Courier");
    }
    else if (r == "Collection Center")
    {
        m.push_back("Collection Center");
    }
    // (custom roles: Dashboard only)

    m.push_back("Logout");
    return m;
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
                           "Test Rate List", "Machines", "SOPs", "Corporate",
                           "Sampling Areas", "Back"});
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
        case 8: areasModule(); break;
        case 9: return;
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
// REPORTS : read-only summaries, each viewable then exportable (CSV / HTML).
// ---------------------------------------------------------------------------
void App::reportsModule()
{
    logAction("opened Reports");
    while (true)
    {
        view.clear();
        int c = view.menu("REPORTS",
                          {"Financial Summary", "Test & Specimen Status", "Referrals", "Back"});
        switch (c)
        {
        case 0: financialReport(); break;
        case 1: testStatusReport(); break;
        case 2: referralReport(); break;
        default: return;
        }
    }
}

void App::showReport(const std::string &title, const std::vector<std::string> &headers,
                     const std::vector<std::vector<std::string>> &rows)
{
    while (true)
    {
        view.clear();
        view.table(title, headers, rows);
        int c = view.menu("Report actions", {"Export", "Back"});
        if (c == 0)
            exportTable(title, headers, rows);
        else
            return;
    }
}

void App::financialReport()
{
    db.loadAll();
    long long net = 0, paid = 0;
    for (int i = 0; i < db.invoices.count(); i++)
        net += static_cast<long long>(invoiceNet(db.invoices.at(i).id) + 0.5);
    for (int i = 0; i < db.payments.count(); i++)
        try { paid += std::stoll(db.payments.at(i).amount); } catch (...) {}
    long long outstanding = net - paid;

    std::vector<std::vector<std::string>> rows = {
        {"Invoices (visits)", to_string(db.invoices.count())},
        {"Payments recorded", to_string(db.payments.count())},
        {"Total billed (net)", to_string(net)},
        {"Total received", to_string(paid)},
        {"Total outstanding", to_string(outstanding > 0 ? outstanding : 0)},
    };
    showReport("Financial Summary", {"Metric", "Value"}, rows);
}

void App::testStatusReport()
{
    db.loadAll();
    int total = db.patientTests.count();
    int collected = 0, pending = 0, done = 0;
    for (int i = 0; i < total; i++)
    {
        PatientTest &pt = db.patientTests.at(i);
        (pt.specimenTaken == "Y") ? collected++ : pending++;
        if (pt.status == "DONE") done++;
    }
    std::vector<std::vector<std::string>> rows = {
        {"Tests ordered", to_string(total)},
        {"Specimens collected", to_string(collected)},
        {"Specimens pending", to_string(pending)},
        {"Results entered", to_string(done)},
        {"Results pending", to_string(collected - done)},
    };
    showReport("Test & Specimen Status", {"Metric", "Value"}, rows);
}

void App::referralReport()
{
    db.loadAll();
    std::vector<std::string> refs;
    std::vector<int> count;
    std::vector<long long> billed;
    for (int i = 0; i < db.invoices.count(); i++)
    {
        Invoice &inv = db.invoices.at(i);
        std::string r = inv.reference.empty() ? "SELF" : inv.reference;
        int idx = -1;
        for (size_t k = 0; k < refs.size(); k++)
            if (refs[k] == r) { idx = (int)k; break; }
        if (idx < 0)
        {
            refs.push_back(r);
            count.push_back(0);
            billed.push_back(0);
            idx = (int)refs.size() - 1;
        }
        count[idx]++;
        billed[idx] += static_cast<long long>(invoiceNet(inv.id) + 0.5);
    }
    std::vector<std::vector<std::string>> rows;
    for (size_t k = 0; k < refs.size(); k++)
        rows.push_back({refs[k], to_string(count[k]), to_string(billed[k])});
    showReport("Referrals", {"Reference", "Invoices", "Net billed"}, rows);
}

// ---------------------------------------------------------------------------
// ROLES (Super Admin only): the built-in roles are fixed; custom ones can be
// added / edited / deleted.
// ---------------------------------------------------------------------------
void App::rolesModule()
{
    logAction("opened Roles");
    while (true)
    {
        db.roles.load();
        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.roles.count(); i++)
        {
            Role &r = db.roles.at(i);
            rows.push_back({to_string(i + 1), r.id, r.name,
                            (r.fixed == "Y") ? "built-in" : "custom"});
        }
        std::vector<std::string> headers = {"Sr", "ID", "Role", "Type"};
        RowAction a = view.entityTable("ROLES  (Super Admin)", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addRole(); break;
        case RowAction::Edit: editRole(a.index); break;
        case RowAction::Delete: deleteRole(a.index); break;
        case RowAction::Export: exportTable("Roles", headers, rows); break;
        case RowAction::Back: return;
        }
    }
}

void App::addRole()
{
    view.clear();
    if (db.roles.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }
    auto v = view.form("ADD NEW ROLE", {"Role Name"});
    Role r;
    r.id = db.roles.nextId();
    r.name = v[0];
    r.fixed = "N";
    db.roles.add(r);
    db.roles.store();
    logAction("added role " + r.id + " (" + r.name + ")");
    view.message("Role added (" + r.id + ").");
}

void App::editRole(int i)
{
    if (i < 0 || i >= db.roles.count())
        return;
    view.clear();
    Role &r = db.roles.at(i);
    if (r.fixed == "Y")
    {
        view.message("Built-in roles cannot be edited.");
        return;
    }
    auto v = view.form("EDIT ROLE " + r.id + "  (current: " + r.name + ")", {"New Name"});
    r.name = v[0];
    db.roles.update(i);
    logAction("updated role " + r.id);
    view.message("Role updated.");
}

void App::deleteRole(int i)
{
    if (i < 0 || i >= db.roles.count())
        return;
    view.clear();
    Role &r = db.roles.at(i);
    if (r.fixed == "Y")
    {
        view.message("Built-in roles cannot be deleted.");
        return;
    }
    std::string id = r.id;
    if (!view.confirm("Delete role " + id + "?"))
        return;
    db.roles.removeAt(i);
    db.roles.store();
    logAction("deleted role " + id);
    view.message("Role deleted.");
}

// ---------------------------------------------------------------------------
// USERS (Admin / Super Admin): create and manage staff accounts, assign a
// dynamic role; Home-Sampling users get an area (+ optional coords), and
// Companies & Doctors users link to a Company.
// ---------------------------------------------------------------------------
void App::usersModule()
{
    logAction("opened Users");
    while (true)
    {
        db.users.load();
        db.roles.load();

        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.users.count(); i++)
        {
            User &u = db.users.at(i);
            std::string roleName = u.role;
            int rk = db.roles.indexOf(u.role);
            if (rk >= 0)
                roleName = db.roles.at(rk).name;
            std::string extra = !u.area.empty() ? u.area
                                : (!u.companyId.empty() ? u.companyId : "-");
            rows.push_back({to_string(i + 1), u.id, u.fname, u.uname, roleName,
                            (u.active ? "yes" : "no"), extra});
        }
        std::vector<std::string> headers = {"Sr", "ID", "Name", "Username", "Role", "Active", "Area/Co"};
        RowAction a = view.entityTable("USERS", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addUser(); break;
        case RowAction::Edit: editUser(a.index); break;
        case RowAction::Delete: deleteUser(a.index); break;
        case RowAction::Export: exportTable("Users", headers, rows); break;
        case RowAction::Back: return;
        }
    }
}

// Fills a user's role-specific extras (area/coords for home sampling, linked
// company for corporate). Shared by add and edit.
static const char *ROLE_HOME = "Home Sampling";
static const char *ROLE_CORP = "Companies & Doctors";

void App::addUser()
{
    view.clear();
    if (db.roles.count() == 0)
    {
        view.message("Define a role first.");
        return;
    }
    if (db.users.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    auto v = view.form("ADD NEW USER",
                       {"Full Name", "Username", "Password", "Email", "Phone", "Address"});
    int ri = view.select("Role", db.roles.names());
    if (ri < 0)
        return;
    Role &role = db.roles.at(ri);

    User u;
    u.id = db.users.nextId();
    u.fname = v[0];
    u.uname = v[1];
    u.password = v[2];
    u.email = v[3];
    u.phone = v[4];
    u.address = v[5];
    u.role = role.id;
    u.active = true;

    if (role.name == ROLE_HOME)
    {
        if (db.areas.count() > 0)
        {
            int ai = view.select("Assign Area", db.areas.names());
            if (ai >= 0)
                u.area = db.areas.at(ai).name;
        }
        auto c = view.form("Optional coordinates (leave blank to skip)",
                           {"Latitude", "Longitude"});
        u.lat = c[0];
        u.lng = c[1];
    }
    else if (role.name == ROLE_CORP)
    {
        if (db.companies.count() > 0)
        {
            int ci = view.select("Link to Company / Doctor", db.companies.names());
            if (ci >= 0)
                u.companyId = db.companies.at(ci).id;
        }
    }

    db.users.add(u);
    db.users.store();
    logAction("added user " + u.id + " (" + u.uname + ") as " + role.id);
    view.message("User added (" + u.id + ").");
}

void App::editUser(int i)
{
    if (i < 0 || i >= db.users.count())
        return;
    view.clear();
    User &u = db.users.at(i);

    auto v = view.form("EDIT USER " + u.id + "  (" + u.uname + ")",
                       {"Full Name", "Username", "Password", "Email", "Phone", "Address"});
    u.fname = v[0];
    u.uname = v[1];
    u.password = v[2];
    u.email = v[3];
    u.phone = v[4];
    u.address = v[5];

    int ri = view.select("Role (current: " + u.role + ")", db.roles.names());
    if (ri >= 0)
    {
        Role &role = db.roles.at(ri);
        u.role = role.id;
        u.area.clear();
        u.lat.clear();
        u.lng.clear();
        u.companyId.clear();
        if (role.name == ROLE_HOME && db.areas.count() > 0)
        {
            int ai = view.select("Assign Area", db.areas.names());
            if (ai >= 0)
                u.area = db.areas.at(ai).name;
            auto c = view.form("Optional coordinates", {"Latitude", "Longitude"});
            u.lat = c[0];
            u.lng = c[1];
        }
        else if (role.name == ROLE_CORP && db.companies.count() > 0)
        {
            int ci = view.select("Link to Company / Doctor", db.companies.names());
            if (ci >= 0)
                u.companyId = db.companies.at(ci).id;
        }
    }

    u.active = view.confirm("Is this user active?");
    db.users.update(i);
    logAction("updated user " + u.id);
    view.message("User updated.");
}

void App::deleteUser(int i)
{
    if (i < 0 || i >= db.users.count())
        return;
    view.clear();
    User &u = db.users.at(i);
    if (u.id == session.id())
    {
        view.message("You cannot delete the account you are logged in with.");
        return;
    }
    std::string id = u.id;
    if (!view.confirm("Delete user " + id + " (" + u.uname + ")?"))
        return;
    db.users.removeAt(i);
    db.users.store();
    logAction("deleted user " + id);
    view.message("User deleted.");
}

// ---------------------------------------------------------------------------
// SAMPLING AREAS (Setup): named areas with optional coordinates, used to route
// home-sampling collections.
// ---------------------------------------------------------------------------
void App::areasModule()
{
    logAction("opened Sampling Areas");
    while (true)
    {
        db.areas.load();
        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.areas.count(); i++)
        {
            Area &a = db.areas.at(i);
            rows.push_back({to_string(i + 1), a.id, a.name, a.lat, a.lng});
        }
        std::vector<std::string> headers = {"Sr", "ID", "Area", "Lat", "Lng"};
        RowAction a = view.entityTable("SAMPLING AREAS", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addArea(); break;
        case RowAction::Edit: editArea(a.index); break;
        case RowAction::Delete: deleteArea(a.index); break;
        case RowAction::Export: exportTable("Sampling Areas", headers, rows); break;
        case RowAction::Back: return;
        }
    }
}

void App::addArea()
{
    view.clear();
    if (db.areas.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }
    auto v = view.form("ADD SAMPLING AREA",
                       {"Area Name", "Latitude (optional)", "Longitude (optional)"});
    Area a;
    a.id = db.areas.nextId();
    a.name = v[0];
    a.lat = v[1];
    a.lng = v[2];
    db.areas.add(a);
    db.areas.store();
    logAction("added area " + a.id + " (" + a.name + ")");
    view.message("Area added (" + a.id + ").");
}

void App::editArea(int i)
{
    if (i < 0 || i >= db.areas.count())
        return;
    view.clear();
    Area &a = db.areas.at(i);
    auto v = view.form("EDIT AREA " + a.id + "  (current: " + a.name + ")",
                       {"New Name", "Latitude", "Longitude"});
    a.name = v[0];
    a.lat = v[1];
    a.lng = v[2];
    db.areas.update(i);
    logAction("updated area " + a.id);
    view.message("Area updated.");
}

void App::deleteArea(int i)
{
    if (i < 0 || i >= db.areas.count())
        return;
    view.clear();
    std::string id = db.areas.at(i).id;
    if (!view.confirm("Delete area " + id + "?"))
        return;
    db.areas.removeAt(i);
    db.areas.store();
    logAction("deleted area " + id);
    view.message("Area deleted.");
}

// ---------------------------------------------------------------------------
// PATIENT area (relational): records + sample receiving + result entry +
// summary. A visit creates an Invoice with PatientTest rows; money is tracked
// in Payment rows, so a balance is netTotal - sum(payments).
// ---------------------------------------------------------------------------
void App::patientModule()
{
    while (true)
    {
        view.clear();
        int c = view.menu("PATIENT",
                          {"Patient Records", "Sample Receiving", "Result Entry",
                           "Patient Summary", "Back"});
        switch (c)
        {
        case 0: patientRecords(); break;
        case 1: sampleReceiving(); break;
        case 2: resultEntry(); break;
        case 3: patientSummary(); break;
        default: return;
        }
    }
}

// ---- billing helpers ------------------------------------------------------
double App::invoiceNet(const std::string &invoiceId)
{
    int k = db.invoices.indexOf(invoiceId);
    if (k < 0) return 0;
    try { return std::stod(db.invoices.at(k).netTotal); } catch (...) { return 0; }
}

double App::invoicePaid(const std::string &invoiceId)
{
    double sum = 0;
    for (int i = 0; i < db.payments.count(); i++)
        if (db.payments.at(i).invoiceId == invoiceId)
            try { sum += std::stod(db.payments.at(i).amount); } catch (...) {}
    return sum;
}

double App::patientBalance(const std::string &patientId)
{
    double bal = 0;
    for (int i = 0; i < db.invoices.count(); i++)
        if (db.invoices.at(i).patientId == patientId)
        {
            const std::string &iid = db.invoices.at(i).id;
            bal += invoiceNet(iid) - invoicePaid(iid);
        }
    return bal;
}

// ---- export + printable documents -----------------------------------------

// Helper: make a filesystem-safe slug from a title.
static std::string slug(const std::string &s)
{
    std::string out;
    for (char c : s)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            out += c;
        else if (out.empty() || out.back() != '_')
            out += '_';
    }
    while (!out.empty() && out.back() == '_')
        out.pop_back();
    return out.empty() ? "table" : out;
}

void App::exportTable(const std::string &title,
                      const std::vector<std::string> &headers,
                      const std::vector<std::vector<std::string>> &rows)
{
    view.clear();
    int fmt = view.menu("EXPORT '" + title + "'", {"CSV", "HTML", "PDF", "Cancel"});
    if (fmt == 3)
        return;

    std::string stamp = Console::date(); // YYYY-MM-DD
    std::string base = "Exports/" + slug(title) + "_" + stamp;
    bool ok = false;
    std::string path;
    if (fmt == 0)
    {
        path = base + ".csv";
        ok = Export::csv(path, headers, rows);
    }
    else if (fmt == 1)
    {
        path = base + ".html";
        ok = Export::html(path, title, headers, rows);
    }
    else
    {
        path = base + ".pdf";
        ok = Export::tablePdf(path, title, headers, rows);
    }

    if (!ok)
    {
        view.message("Could not write the export file.");
        return;
    }
    logAction("exported table '" + title + "' -> " + path);
    if (view.confirm("Saved to " + path + ".  Open it now?"))
        Console::openFile(path);
}

void App::printInvoice(const std::string &invoiceId)
{
    int k = db.invoices.indexOf(invoiceId);
    if (k < 0)
        return;
    Invoice &inv = db.invoices.at(k);

    Export::InvoiceDoc doc;
    doc.id = inv.id;
    doc.date = inv.date;
    doc.patientId = inv.patientId;
    int pk = db.patients.indexOf(inv.patientId);
    if (pk >= 0)
    {
        doc.patientName = db.patients.at(pk).name;
        doc.patientContact = db.patients.at(pk).contact;
    }
    doc.sampleLocation = inv.sampleLocation;
    doc.reference = inv.reference;
    doc.discount = inv.discount;
    doc.gross = inv.grossTotal;
    doc.net = inv.netTotal;
    long long paid = static_cast<long long>(invoicePaid(inv.id) + 0.5);
    long long bal = static_cast<long long>(invoiceNet(inv.id) + 0.5) - paid;
    doc.paid = to_string(paid);
    doc.balance = to_string(bal > 0 ? bal : 0);
    for (int i = 0; i < db.patientTests.count(); i++)
    {
        PatientTest &pt = db.patientTests.at(i);
        if (pt.invoiceId == inv.id)
            doc.tests.push_back({pt.testName, pt.specimen, pt.rate, pt.status, pt.result});
    }

    std::string path = "Exports/invoice_" + inv.id + ".pdf";
    if (!Export::invoicePdf(path, doc))
    {
        view.message("Could not write the invoice document.");
        return;
    }
    logAction("generated invoice document " + inv.id);
    if (view.confirm("Invoice saved to " + path + ".  Open / print now?"))
        Console::openFile(path);
}

// Generates a printable lab report (tests + results) for an invoice.
void App::printReport(const std::string &invoiceId)
{
    int k = db.invoices.indexOf(invoiceId);
    if (k < 0)
        return;
    Invoice &inv = db.invoices.at(k);

    Export::ReportDoc doc;
    doc.invoiceId = inv.id;
    doc.date = Console::date();
    doc.patientId = inv.patientId;
    doc.reference = inv.reference;
    int pk = db.patients.indexOf(inv.patientId);
    if (pk >= 0)
    {
        Patient &p = db.patients.at(pk);
        doc.patientName = p.name;
        doc.patientContact = p.contact;
        doc.gender = p.gender;
        doc.age = p.age;
        doc.bloodGroup = p.bloodGroup;
    }
    for (int i = 0; i < db.patientTests.count(); i++)
    {
        PatientTest &pt = db.patientTests.at(i);
        if (pt.invoiceId != inv.id)
            continue;
        std::string unit;
        int lk = db.labTests.indexOf(pt.testId);
        if (lk >= 0)
            unit = db.labTests.at(lk).unit;
        doc.tests.push_back({pt.testName, pt.result, unit, pt.status});
    }

    std::string path = "Exports/report_" + inv.id + ".pdf";
    if (!Export::reportPdf(path, doc))
    {
        view.message("Could not write the report document.");
        return;
    }
    logAction("generated lab report for invoice " + inv.id);
    if (view.confirm("Report saved to " + path + ".  Open / print now?"))
        Console::openFile(path);
}

void App::printReceipt(const std::string &invoiceId, const std::string &paymentId)
{
    int k = db.payments.indexOf(paymentId);
    if (k < 0)
        return;
    Payment &pay = db.payments.at(k);

    Export::ReceiptDoc doc;
    doc.id = pay.id;
    doc.date = pay.date;
    doc.invoiceId = invoiceId;
    doc.amount = pay.amount;
    long long paid = static_cast<long long>(invoicePaid(invoiceId) + 0.5);
    long long bal = static_cast<long long>(invoiceNet(invoiceId) + 0.5) - paid;
    doc.paidTotal = to_string(paid);
    doc.balance = to_string(bal > 0 ? bal : 0);
    int ik = db.invoices.indexOf(invoiceId);
    if (ik >= 0)
    {
        int pk = db.patients.indexOf(db.invoices.at(ik).patientId);
        if (pk >= 0)
            doc.patientName = db.patients.at(pk).name;
    }

    std::string path = "Exports/receipt_" + pay.id + ".pdf";
    if (!Export::receiptPdf(path, doc))
        return;
    logAction("generated receipt " + pay.id);
    if (view.confirm("Receipt saved to " + path + ".  Open / print now?"))
        Console::openFile(path);
}

void App::patientRecords()
{
    logAction("opened Patient Records");
    while (true)
    {
        db.loadAll(); // patients + invoices + tests + payments (for balances)

        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.patients.count(); i++)
        {
            Patient &p = db.patients.at(i);
            long long bal = static_cast<long long>(patientBalance(p.id) + 0.5);
            rows.push_back({to_string(i + 1), p.id, p.name, p.contact, p.gender,
                            p.age, to_string(bal)});
        }

        // [Add New] = register a new patient; row [Edit] = manage (new visit /
        // invoices); [Delete] = remove the patient and their billing records.
        std::vector<std::string> headers = {"Sr", "ID", "Name", "Contact", "Gender", "Age", "Balance"};
        RowAction a = view.entityTable("PATIENTS  (Edit = new visit / invoices)", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: registerPatient(); break;
        case RowAction::Edit: managePatient(a.index); break;
        case RowAction::Delete: deletePatient(a.index); break;
        case RowAction::Export: exportTable("Patients", headers, rows); break;
        case RowAction::Back: return;
        }
    }
}

// Read-only aggregate view over patients / tests / billing.
void App::patientSummary()
{
    logAction("opened Patient Summary");
    db.loadAll();

    int totalTests = db.patientTests.count();
    int collected = 0, resulted = 0, pending = 0;
    for (int i = 0; i < totalTests; i++)
    {
        PatientTest &pt = db.patientTests.at(i);
        (pt.specimenTaken == "Y") ? collected++ : pending++;
        if (pt.status == "DONE") resulted++;
    }

    long long net = 0, paid = 0;
    for (int i = 0; i < db.invoices.count(); i++)
        net += static_cast<long long>(invoiceNet(db.invoices.at(i).id) + 0.5);
    for (int i = 0; i < db.payments.count(); i++)
        try { paid += std::stoll(db.payments.at(i).amount); } catch (...) {}
    long long balance = net - paid;

    vector<vector<string>> rows = {
        {"Total patients", to_string(db.patients.count())},
        {"Total invoices (visits)", to_string(db.invoices.count())},
        {"Total tests ordered", to_string(totalTests)},
        {"Specimens collected", to_string(collected)},
        {"Specimens pending", to_string(pending)},
        {"Results entered", to_string(resulted)},
        {"Total billed (net)", to_string(net)},
        {"Total received", to_string(paid)},
        {"Total outstanding", to_string(balance > 0 ? balance : 0)},
    };

    view.clear();
    view.table("PATIENT SUMMARY", {"Metric", "Value"}, rows);
    view.message("Summary across " + to_string(db.patients.count()) + " patient(s).");
}

void App::registerPatient()
{
    view.clear();
    if (db.patients.count() >= Database::MAX)
    {
        view.message("You have reached the maximum storage limit.");
        return;
    }

    auto v = view.form("REGISTER NEW PATIENT",
                       {"Patient Name", "CNIC", "Phone No.", "Gender (M/F/O)",
                        "Age", "Blood Group", "Address"});
    Patient p;
    p.id = db.patients.nextId();
    p.name = v[0]; p.cnic = v[1]; p.contact = v[2]; p.gender = v[3];
    p.age = v[4]; p.bloodGroup = v[5]; p.address = v[6];
    p.regDate = Console::date();
    db.patients.add(p);
    db.patients.store();
    logAction("registered patient " + p.id + " (" + p.name + ")");

    // First visit / invoice.
    std::string inv = orderTests(p.id);
    if (inv.empty())
        view.message("Patient " + p.id + " saved (no tests ordered yet).");
}

// Creates one invoice (its PatientTest rows + an opening payment) for an
// existing patient. Returns the new invoice id, or "" if nothing was ordered.
std::string App::orderTests(const std::string &patientId)
{
    if (db.labTests.count() == 0 && db.packages.count() == 0)
    {
        view.message("No lab tests or packages exist yet. Add some first.");
        return "";
    }

    Invoice inv;
    inv.id = db.invoices.nextId();
    inv.patientId = patientId;
    inv.date = Console::date();

    std::vector<std::string> locations = {"LAB", "Home", "Collection Center", "Hospital", "Doctor"};
    int locIdx = view.select("Sample Location", locations);
    inv.sampleLocation = (locIdx >= 0) ? locations[locIdx] : "LAB";

    // Home sample -> pick an area and assign a home-sampling person.
    if (inv.sampleLocation == "Home")
    {
        if (db.areas.count() > 0)
        {
            int ai = view.select("Home sample area", db.areas.names());
            if (ai >= 0)
                inv.area = db.areas.at(ai).name;
        }
        else
            inv.area = view.ask("Home sample area");

        std::string homeRole = homeSamplingRoleId();
        std::vector<std::string> opts, ids;
        for (int u = 0; u < db.users.count(); u++)
        {
            User &usr = db.users.at(u);
            if (usr.role != homeRole || !usr.active)
                continue;
            opts.push_back(usr.fname + "  [" + (usr.area.empty() ? "any area" : usr.area) + "]");
            ids.push_back(usr.id);
        }
        if (!ids.empty())
        {
            int hi = view.select("Assign home-sampling person", opts);
            if (hi >= 0)
                inv.homeSampler = ids[hi];
        }
        else
            view.message("No active home-sampling staff yet - leaving unassigned.");
    }

    inv.reference = "SELF";
    std::string discount = "0";
    int refCompany = -1; // index into db.companies, -1 = self
    int rc = view.menu("Referral", {"None (self)", "Reference company / doctor", "Coupon code"});
    if (rc == 1 && db.companies.count() > 0)
    {
        int ci = view.select("Select Reference", db.companies.names());
        if (ci >= 0)
        {
            refCompany = ci;
            inv.reference = db.companies.at(ci).name;
            discount = db.companies.at(ci).discount;
        }
    }
    else if (rc == 2)
    {
        std::string code = view.ask("Coupon code");
        int ci = companyByCoupon(code);
        if (ci >= 0)
        {
            refCompany = ci;
            Company &co = db.companies.at(ci);
            inv.reference = co.name;
            double base = 0, extra = 0;
            try { base = std::stod(co.discount); } catch (...) {}
            try { extra = std::stod(co.couponPct); } catch (...) {}
            discount = to_string(base + extra);
            view.message("Coupon applied: " + co.name + " (" + co.discount + "% + " +
                         co.couponPct + "% extra = " + discount + "%)");
        }
        else
            view.message("Invalid coupon - ordering as SELF.");
    }
    else if (rc == 1)
        view.message("No company records - ordering as SELF.");

    double gross = 0;
    if (!chooseTests(inv.id, patientId, gross, discount))
    {
        view.message("No tests ordered.");
        return "";
    }

    double discPct = 0;
    try { discPct = std::stod(discount); } catch (...) {}
    double net = gross - (discPct * gross / 100.0);

    inv.discount = discount;
    inv.grossTotal = to_string(static_cast<long long>(gross + 0.5));
    inv.netTotal = to_string(static_cast<long long>(net + 0.5));
    db.invoices.add(inv);
    db.invoices.store();
    logAction("created invoice " + inv.id + " for patient " + patientId);

    // Accrue the doctor/company share into the settlement ledger.
    if (refCompany >= 0)
    {
        int nTests = 0;
        for (int t = 0; t < db.patientTests.count(); t++)
            if (db.patientTests.at(t).invoiceId == inv.id)
                nTests++;
        accrueShare(refCompany, inv.id, net, nTests);
    }

    // Opening payment.
    view.clear();
    view.message("Invoice " + inv.id + ":  net " + inv.netTotal + " (gross " +
                 inv.grossTotal + ", discount " + discount + "%)");
    std::string recv = view.ask("Amount received now");
    long long amount = 0;
    try { amount = std::stoll(recv); } catch (...) { amount = 0; }
    std::string payId;
    if (amount > 0)
    {
        Payment pay;
        pay.id = db.payments.nextId();
        pay.invoiceId = inv.id;
        pay.amount = to_string(amount);
        pay.date = Console::date();
        db.payments.add(pay);
        db.payments.store();
        payId = pay.id;
        logAction("payment " + pay.id + " of " + pay.amount + " on invoice " + inv.id);
    }

    long long balance = static_cast<long long>(net + 0.5) - amount;
    view.message("Invoice " + inv.id + " created. Paid " + to_string(amount) +
                 ", balance " + to_string(balance > 0 ? balance : 0) + ".");

    // Generate the printable invoice (and a receipt for the opening payment).
    printInvoice(inv.id);
    if (!payId.empty())
        printReceipt(inv.id, payId);
    return inv.id;
}

// Adds PatientTest rows for a package or individually-picked tests, summing the
// gross. A package also sets `discount`. Returns false if nothing was added.
bool App::chooseTests(const std::string &invoiceId, const std::string &patientId,
                      double &gross, std::string &discount)
{
    gross = 0;
    int added = 0;

    auto addTest = [&](int k)
    {
        LabTest &lt = db.labTests.at(k);
        PatientTest pt;
        pt.id = db.patientTests.nextId();
        pt.invoiceId = invoiceId;
        pt.patientId = patientId;
        pt.testId = lt.id;
        pt.testName = lt.name;
        pt.specimen = lt.specimen;
        pt.rate = lt.rate;
        pt.specimenTaken = "N";
        pt.status = "PEND";
        db.patientTests.add(pt);
        try { gross += std::stod(lt.rate); } catch (...) {}
        added++;
    };

    if (view.confirm("Attach a package?") && db.packages.count() > 0)
    {
        int pi = view.select("Select Package", db.packages.names());
        if (pi >= 0)
        {
            Package &pack = db.packages.at(pi);
            discount = pack.disc; // package discount overrides the reference one
            for (const string &tid : pack.tests)
            {
                if (tid.empty()) continue;
                int k = db.labTests.indexOf(tid);
                if (k >= 0) addTest(k);
            }
        }
    }

    if (added == 0) // individual tests
    {
        if (db.labTests.count() == 0)
            return false;
        std::string numStr = view.ask("How many tests to add");
        int num = 0;
        try { num = std::stoi(numStr); } catch (...) { num = 0; }
        if (num < 1) num = 1;
        if (num > 20) num = 20;

        std::vector<std::string> options;
        for (int k = 0; k < db.labTests.count(); k++)
            options.push_back(db.labTests.at(k).id + " - " + db.labTests.at(k).name);

        for (int a = 0; a < num; a++)
        {
            int ti = view.select("Select test " + to_string(a + 1), options);
            if (ti < 0) break;
            addTest(ti);
        }
    }

    if (added == 0)
        return false;
    db.patientTests.store();
    return true;
}

// ---------------------------------------------------------------------------
// Manage a patient: order another visit, or browse invoices & payments.
// ---------------------------------------------------------------------------
void App::managePatient(int i)
{
    if (i < 0 || i >= db.patients.count())
        return;
    std::string pid = db.patients.at(i).id;
    std::string pname = db.patients.at(i).name;
    while (true)
    {
        view.clear();
        int c = view.menu("PATIENT " + pid + " - " + pname,
                          {"New Visit (order tests)", "Invoices & Payments",
                           "Patient Report (PDF)", "Back"});
        if (c == 0)
            orderTests(pid);
        else if (c == 1)
            patientInvoices(pid);
        else if (c == 2)
            printPatientReport(pid);
        else
            return;
    }
}

void App::patientInvoices(const std::string &patientId)
{
    while (true)
    {
        db.invoices.load();
        db.payments.load();
        db.patientTests.load();

        std::vector<std::vector<std::string>> rows;
        std::vector<std::string> ids;
        for (int i = 0; i < db.invoices.count(); i++)
        {
            Invoice &inv = db.invoices.at(i);
            if (inv.patientId != patientId) continue;
            long long paid = static_cast<long long>(invoicePaid(inv.id) + 0.5);
            long long bal = static_cast<long long>(invoiceNet(inv.id) + 0.5) - paid;
            ids.push_back(inv.id);
            rows.push_back({to_string((int)ids.size()), inv.id, inv.date, inv.netTotal,
                            to_string(paid), to_string(bal > 0 ? bal : 0)});
        }

        view.clear();
        view.table("INVOICES for " + patientId,
                   {"Sr", "Invoice", "Date", "Net", "Paid", "Balance"}, rows);
        if (ids.empty())
        {
            view.message("No invoices yet for this patient.");
            return;
        }
        std::vector<std::string> opts = ids;
        opts.push_back("<< Back");
        int sel = view.select("Open an invoice", opts);
        if (sel < 0 || sel >= (int)ids.size())
            return;
        invoiceDetail(ids[sel]);
    }
}

void App::invoiceDetail(const std::string &invoiceId)
{
    while (true)
    {
        db.patientTests.load();
        db.payments.load();
        db.invoices.load();

        std::vector<std::vector<std::string>> rows;
        for (int i = 0; i < db.patientTests.count(); i++)
        {
            PatientTest &pt = db.patientTests.at(i);
            if (pt.invoiceId != invoiceId) continue;
            std::string taken = (pt.specimenTaken == "Y") ? "YES" : "no";
            rows.push_back({pt.testId, pt.testName, pt.specimen, pt.rate, taken, pt.status, pt.result});
        }

        long long net = static_cast<long long>(invoiceNet(invoiceId) + 0.5);
        long long paid = static_cast<long long>(invoicePaid(invoiceId) + 0.5);
        long long bal = net - paid;

        view.clear();
        view.table("INVOICE " + invoiceId + "   Net " + to_string(net) + "  Paid " +
                       to_string(paid) + "  Balance " + to_string(bal > 0 ? bal : 0),
                   {"Test", "Name", "Specimen", "Rate", "Taken", "Status", "Result"}, rows);

        int c = view.menu("Invoice actions",
                          {"Add Payment", "Print Invoice", "Print Lab Report", "Back"});
        if (c == 0)
            addPayment(invoiceId);
        else if (c == 1)
            printInvoice(invoiceId);
        else if (c == 2)
            printReport(invoiceId);
        else
            return;
    }
}

void App::addPayment(const std::string &invoiceId)
{
    view.clear();
    long long bal = static_cast<long long>((invoiceNet(invoiceId) - invoicePaid(invoiceId)) + 0.5);
    if (bal <= 0)
    {
        view.message("This invoice is already fully paid.");
        return;
    }
    view.message("Outstanding balance: " + to_string(bal));
    std::string recv = view.ask("Payment amount");
    long long amount = 0;
    try { amount = std::stoll(recv); } catch (...) { amount = 0; }
    if (amount <= 0)
    {
        view.message("No payment recorded.");
        return;
    }
    Payment pay;
    pay.id = db.payments.nextId();
    pay.invoiceId = invoiceId;
    pay.amount = to_string(amount);
    pay.date = Console::date();
    db.payments.add(pay);
    db.payments.store();
    logAction("payment " + pay.id + " of " + pay.amount + " on invoice " + invoiceId);
    long long newBal = bal - amount;
    view.message("Payment recorded. New balance: " + to_string(newBal > 0 ? newBal : 0));
    printReceipt(invoiceId, pay.id);
}

// ---------------------------------------------------------------------------
// Sample Receiving: collect specimens for any pending PatientTest (across all
// patients). Collecting shows that specimen's SOP checklist.
// ---------------------------------------------------------------------------
void App::sampleReceiving()
{
    logAction("opened Sample Receiving");
    while (true)
    {
        db.patientTests.load();
        db.patients.load();
        db.sops.load();

        std::vector<int> idx;
        std::vector<std::string> opts;
        for (int i = 0; i < db.patientTests.count(); i++)
        {
            PatientTest &pt = db.patientTests.at(i);
            if (pt.specimenTaken == "Y") continue; // only pending
            std::string pname = "?";
            int pk = db.patients.indexOf(pt.patientId);
            if (pk >= 0) pname = db.patients.at(pk).name;
            opts.push_back(pt.patientId + " " + pname + "  |  " + pt.testName +
                           "  |  specimen: " + pt.specimen);
            idx.push_back(i);
        }

        view.clear();
        if (opts.empty())
        {
            view.message("No specimens pending collection.");
            return;
        }
        opts.push_back("<< Back");
        int sel = view.select("SAMPLE RECEIVING - pick a test to collect its specimen", opts);
        if (sel < 0 || sel >= (int)idx.size())
            return;

        int i = idx[sel];
        PatientTest &pt = db.patientTests.at(i);

        std::vector<std::vector<std::string>> sopRows;
        for (int q = 0; q < db.sops.count(); q++)
            if (db.sops.at(q).specimen == pt.specimen)
                sopRows.push_back({db.sops.at(q).id, db.sops.at(q).sop});

        view.clear();
        if (sopRows.empty())
        {
            if (!view.confirm("No SOPs for specimen '" + pt.specimen + "'. Mark it collected?"))
                continue;
        }
        else
        {
            view.table("SOPs for specimen '" + pt.specimen + "'", {"ID", "Procedure"}, sopRows);
            if (!view.confirm("Did you follow ALL SOPs and collect the specimen?"))
                continue;
        }

        pt.specimenTaken = "Y";
        db.patientTests.update(i);
        logAction("collected specimen for test " + pt.id + " (" + pt.testName +
                  ") patient " + pt.patientId);
        view.message("Specimen collected for " + pt.testName + ".");
    }
}

// ---------------------------------------------------------------------------
// Result Entry: for tests whose specimen is collected but result is pending,
// record the result and mark the test DONE.
// ---------------------------------------------------------------------------
void App::resultEntry()
{
    logAction("opened Result Entry");
    while (true)
    {
        db.patientTests.load();
        db.patients.load();

        std::vector<int> idx;
        std::vector<std::string> opts;
        for (int i = 0; i < db.patientTests.count(); i++)
        {
            PatientTest &pt = db.patientTests.at(i);
            if (pt.specimenTaken != "Y") continue; // specimen must be collected
            if (pt.status == "DONE") continue;      // result not yet entered
            std::string pname = "?";
            int pk = db.patients.indexOf(pt.patientId);
            if (pk >= 0) pname = db.patients.at(pk).name;
            opts.push_back(pt.patientId + " " + pname + "  |  " + pt.testName +
                           "  |  " + pt.specimen);
            idx.push_back(i);
        }

        view.clear();
        if (opts.empty())
        {
            view.message("No tests awaiting results (collect specimens first).");
            return;
        }
        opts.push_back("<< Back");
        int sel = view.select("RESULT ENTRY - pick a test", opts);
        if (sel < 0 || sel >= (int)idx.size())
            return;

        int i = idx[sel];
        PatientTest &pt = db.patientTests.at(i);
        auto v = view.form("Enter result for " + pt.testName + " (" + pt.testId + ")", {"Result"});
        std::string res = v[0];
        for (char &ch : res)
            if (ch == ',') ch = ';'; // keep the CSV row intact
        pt.result = res;
        pt.status = "DONE";
        db.patientTests.update(i);
        logAction("entered result for test " + pt.id + " (" + pt.testName +
                  ") patient " + pt.patientId);
        view.message("Result saved for " + pt.testName + ".");
    }
}

// Deletes a patient and cascades to their invoices, tests and payments.
void App::deletePatient(int i)
{
    if (i < 0 || i >= db.patients.count())
        return;
    view.clear();
    std::string id = db.patients.at(i).id;
    if (!view.confirm("Delete patient " + id + " and ALL their invoices/tests/payments?"))
        return;

    std::vector<std::string> invIds;
    for (int k = 0; k < db.invoices.count(); k++)
        if (db.invoices.at(k).patientId == id)
            invIds.push_back(db.invoices.at(k).id);
    auto belongs = [&](const std::string &iv)
    {
        for (const std::string &x : invIds)
            if (x == iv) return true;
        return false;
    };

    for (int k = db.patientTests.count() - 1; k >= 0; k--)
        if (db.patientTests.at(k).patientId == id)
            db.patientTests.removeAt(k);
    for (int k = db.payments.count() - 1; k >= 0; k--)
        if (belongs(db.payments.at(k).invoiceId))
            db.payments.removeAt(k);
    for (int k = db.invoices.count() - 1; k >= 0; k--)
        if (db.invoices.at(k).patientId == id)
            db.invoices.removeAt(k);
    db.patients.removeAt(i);

    db.patients.store();
    db.invoices.store();
    db.patientTests.store();
    db.payments.store();
    logAction("deleted patient " + id + " (cascade)");
    view.message("Patient and related records deleted.");
}

// ---------------------------------------------------------------------------
// HOME SAMPLING: visits whose sample location is "Home". A home-sampling user
// sees only their own assignments; others see all. Payment must be cleared
// before the sample can be collected.
// ---------------------------------------------------------------------------
std::string App::homeSamplingRoleId()
{
    for (int i = 0; i < db.roles.count(); i++)
        if (db.roles.at(i).name == "Home Sampling")
            return db.roles.at(i).id;
    return "R005";
}

void App::homeSamplingModule()
{
    logAction("opened Home Sampling");
    std::string homeRole = homeSamplingRoleId();
    bool mine = (session.userRole() == homeRole);

    while (true)
    {
        db.loadAll();
        std::vector<std::string> opts, ids;
        for (int i = 0; i < db.invoices.count(); i++)
        {
            Invoice &inv = db.invoices.at(i);
            if (inv.sampleLocation != "Home")
                continue;
            if (mine && inv.homeSampler != session.id())
                continue;

            std::string pname = "?";
            int pk = db.patients.indexOf(inv.patientId);
            if (pk >= 0)
                pname = db.patients.at(pk).name;
            std::string sampler = "-";
            int sk = db.users.indexOf(inv.homeSampler);
            if (sk >= 0)
                sampler = db.users.at(sk).fname;
            long long bal = static_cast<long long>(invoiceNet(inv.id) - invoicePaid(inv.id) + 0.5);
            bool pending = false;
            for (int t = 0; t < db.patientTests.count(); t++)
                if (db.patientTests.at(t).invoiceId == inv.id &&
                    db.patientTests.at(t).specimenTaken != "Y")
                    pending = true;
            std::string status = (bal > 0) ? "PAYMENT DUE" : (pending ? "to collect" : "collected");

            opts.push_back(inv.id + "  " + pname + "  | " + inv.area + " | " + sampler +
                           " | bal " + to_string(bal > 0 ? bal : 0) + " | " + status);
            ids.push_back(inv.id);
        }

        view.clear();
        if (opts.empty())
        {
            view.message(mine ? "No home samples assigned to you." : "No home-sample visits yet.");
            return;
        }
        opts.push_back("<< Back");
        int sel = view.select("HOME SAMPLING - pick a visit", opts);
        if (sel < 0 || sel >= (int)ids.size())
            return;
        homeSampleDetail(ids[sel]);
    }
}

void App::homeSampleDetail(const std::string &invoiceId)
{
    while (true)
    {
        db.loadAll();
        int ik = db.invoices.indexOf(invoiceId);
        if (ik < 0)
            return;
        Invoice &inv = db.invoices.at(ik);

        std::vector<std::vector<std::string>> rows;
        bool anyPending = false;
        for (int i = 0; i < db.patientTests.count(); i++)
        {
            PatientTest &pt = db.patientTests.at(i);
            if (pt.invoiceId != invoiceId)
                continue;
            std::string taken = (pt.specimenTaken == "Y") ? "YES" : "no";
            if (pt.specimenTaken != "Y")
                anyPending = true;
            rows.push_back({pt.testId, pt.testName, pt.specimen, taken, pt.status});
        }
        long long net = static_cast<long long>(invoiceNet(invoiceId) + 0.5);
        long long paid = static_cast<long long>(invoicePaid(invoiceId) + 0.5);
        long long bal = net - paid;

        std::string pname = "?";
        int pk = db.patients.indexOf(inv.patientId);
        if (pk >= 0)
            pname = db.patients.at(pk).name;
        std::string sampler = "(unassigned)";
        int sk = db.users.indexOf(inv.homeSampler);
        if (sk >= 0)
            sampler = db.users.at(sk).fname;

        view.clear();
        view.table("HOME SAMPLE " + invoiceId + "   [" + pname + " / " + inv.area + " / " +
                       sampler + "]   Balance " + to_string(bal > 0 ? bal : 0),
                   {"Test", "Name", "Specimen", "Taken", "Status"}, rows);

        std::vector<std::string> actions;
        if (bal > 0)
            actions.push_back("Receive Payment");
        if (bal <= 0 && anyPending)
            actions.push_back("Collect Sample (mark done)");
        actions.push_back("Back");

        int c = view.menu("Home sampling actions", actions);
        std::string act = actions[c];
        if (act == "Receive Payment")
            addPayment(invoiceId);
        else if (act.rfind("Collect Sample", 0) == 0)
            collectHomeSample(invoiceId);
        else
            return;
    }
}

void App::collectHomeSample(const std::string &invoiceId)
{
    view.clear();
    if (!view.confirm("Mark the home sample as COLLECTED for invoice " + invoiceId + "?"))
        return;
    int changed = 0;
    for (int i = 0; i < db.patientTests.count(); i++)
    {
        PatientTest &pt = db.patientTests.at(i);
        if (pt.invoiceId == invoiceId && pt.specimenTaken != "Y")
        {
            pt.specimenTaken = "Y";
            changed++;
        }
    }
    if (changed)
        db.patientTests.store();
    logAction("home sample collected for invoice " + invoiceId + " (" + to_string(changed) + " tests)");
    view.message("Home sample collected (" + to_string(changed) + " test(s)). Results can now be entered.");
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

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Description"};
        RowAction a = view.entityTable("SPECIMENS", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addSpecimen(); break;
        case RowAction::Edit: editSpecimen(a.index); break;
        case RowAction::Delete: deleteSpecimen(a.index); break;
        case RowAction::Export: exportTable("Specimens", headers, rows); break;
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

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Date"};
        RowAction a = view.entityTable("LAB DEPARTMENTS", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addLabDepartment(); break;
        case RowAction::Edit: editLabDepartment(a.index); break;
        case RowAction::Delete: deleteLabDepartment(a.index); break;
        case RowAction::Export: exportTable("Lab Departments", headers, rows); break;
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

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Rate", "Dept", "Specimen"};
        RowAction a = view.entityTable("LAB TESTS", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addLabTest(); break;
        case RowAction::Edit: editLabTest(a.index); break;
        case RowAction::Delete: deleteLabTest(a.index); break;
        case RowAction::Export: exportTable("Lab Tests", headers, rows); break;
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

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Description", "Qty"};
        RowAction a = view.entityTable("MACHINES", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addMachine(); break;
        case RowAction::Edit: editMachine(a.index); break;
        case RowAction::Delete: deleteMachine(a.index); break;
        case RowAction::Export: exportTable("Machines", headers, rows); break;
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

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Tests", "Rate", "Disc"};
        RowAction a = view.entityTable("PACKAGES", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addPackage(); break;
        case RowAction::Edit: editPackage(a.index); break;
        case RowAction::Delete: deletePackage(a.index); break;
        case RowAction::Export: exportTable("Packages", headers, rows); break;
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

        std::vector<std::string> headers = {"Sr", "ID", "Specimen", "SOP", "Date"};
        RowAction a = view.entityTable("SOPs (per specimen)", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addSop(); break;
        case RowAction::Edit: editSop(a.index); break;
        case RowAction::Delete: deleteSop(a.index); break;
        case RowAction::Export: exportTable("SOPs", headers, rows); break;
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
            std::string share = (c.shareType == "FIXED") ? (c.shareValue + "/test")
                                                         : (c.shareValue + "%");
            rows.push_back({to_string(i + 1), c.id, c.name, c.type,
                            c.discount + "%", share, c.couponCode, c.settlementPeriod});
        }

        std::vector<std::string> headers = {"Sr", "ID", "Name", "Type", "Disc", "Share", "Coupon", "Settle"};
        RowAction a = view.entityTable("CORPORATE (Companies & Doctors)", headers, rows);
        switch (a.type)
        {
        case RowAction::Add: addCompany(); break;
        case RowAction::Edit: editCompany(a.index); break;
        case RowAction::Delete: deleteCompany(a.index); break;
        case RowAction::Export: exportTable("Corporate", headers, rows); break;
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
    captureCompanyTerms(c);
    db.companies.add(c);
    db.companies.store();
    logAction("added company " + c.id + " (" + c.name + ")");
    view.message("Company added (" + c.id + ").");
}

// Prompts for the share / coupon / settlement terms (shared by add + edit).
void App::captureCompanyTerms(Company &c)
{
    int st = view.menu("Share type", {"Percentage of patient-paid", "Fixed amount per test"});
    c.shareType = (st == 1) ? "FIXED" : "PCT";
    c.shareValue = view.ask(c.shareType == "FIXED" ? "Share amount per test"
                                                   : "Share percentage %");
    auto cp = view.form("Coupon (leave blank for none)", {"Coupon code", "Coupon extra %"});
    c.couponCode = cp[0];
    c.couponPct = cp[1];
    std::vector<std::string> periods = {"daily", "weekly", "monthly", "yearly"};
    int sp = view.menu("Settlement period", periods);
    c.settlementPeriod = periods[(sp >= 0) ? sp : 2];
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
    captureCompanyTerms(c);
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

int App::companyByCoupon(const std::string &code)
{
    if (code.empty())
        return -1;
    for (int i = 0; i < db.companies.count(); i++)
        if (!db.companies.at(i).couponCode.empty() && db.companies.at(i).couponCode == code)
            return i;
    return -1;
}

std::string App::corpRoleId()
{
    for (int i = 0; i < db.roles.count(); i++)
        if (db.roles.at(i).name == "Companies & Doctors")
            return db.roles.at(i).id;
    return "R003";
}

std::string App::currentUserCompanyId()
{
    int k = db.users.indexOf(session.id());
    return (k >= 0) ? db.users.at(k).companyId : "";
}

// Records the company's share of a referred invoice in the settlement ledger.
void App::accrueShare(int companyIdx, const std::string &invoiceId, double net, int numTests)
{
    if (companyIdx < 0 || companyIdx >= db.companies.count())
        return;
    Company &co = db.companies.at(companyIdx);

    double v = 0;
    try { v = std::stod(co.shareValue); } catch (...) {}
    double share = (co.shareType == "FIXED") ? (v * numTests) : (v * net / 100.0);

    // If a corporate user for THIS company collected the money, they owe the lab
    // the remainder; otherwise the lab owes them their share.
    bool docCollected = (session.userRole() == corpRoleId() && currentUserCompanyId() == co.id);
    double labBalance = docCollected ? (net - share) : (-share);

    Settlement s;
    s.id = db.settlements.nextId();
    s.companyId = co.id;
    s.invoiceId = invoiceId;
    s.amount = to_string(static_cast<long long>(share + 0.5));
    s.labBalance = to_string(static_cast<long long>(labBalance + (labBalance >= 0 ? 0.5 : -0.5)));
    s.direction = docCollected ? "DOC_OWES" : "LAB_OWES";
    s.date = Console::date();
    s.settled = "N";
    db.settlements.add(s);
    db.settlements.store();
    logAction("accrued share " + s.amount + " for company " + co.id +
              " on invoice " + invoiceId + " (" + s.direction + ")");
}

// ---------------------------------------------------------------------------
// SETTLEMENTS : net each company's unsettled ledger entries and clear them.
//   balance < 0 -> lab pays the company; balance > 0 -> the company pays the lab.
// ---------------------------------------------------------------------------
void App::settlementsModule()
{
    logAction("opened Settlements");
    while (true)
    {
        db.loadAll();

        std::vector<std::string> ids;
        std::vector<long long> netBal;
        std::vector<int> cnt;
        for (int i = 0; i < db.settlements.count(); i++)
        {
            Settlement &s = db.settlements.at(i);
            if (s.settled == "Y")
                continue;
            long long lb = 0;
            try { lb = std::stoll(s.labBalance); } catch (...) {}
            int idx = -1;
            for (size_t k = 0; k < ids.size(); k++)
                if (ids[k] == s.companyId) { idx = (int)k; break; }
            if (idx < 0)
            {
                ids.push_back(s.companyId);
                netBal.push_back(0);
                cnt.push_back(0);
                idx = (int)ids.size() - 1;
            }
            netBal[idx] += lb;
            cnt[idx]++;
        }

        std::vector<std::vector<std::string>> rows;
        for (size_t k = 0; k < ids.size(); k++)
        {
            std::string name = ids[k], period = "-";
            int ck = db.companies.indexOf(ids[k]);
            if (ck >= 0)
            {
                name = db.companies.at(ck).name;
                period = db.companies.at(ck).settlementPeriod;
            }
            long long nb = netBal[k];
            std::string bal = (nb < 0) ? ("lab pays " + to_string(-nb))
                              : (nb > 0 ? ("owes lab " + to_string(nb)) : "even");
            rows.push_back({to_string(k + 1), ids[k], name, period, to_string(cnt[k]), bal});
        }

        view.clear();
        view.table("SETTLEMENTS (unsettled)",
                   {"Sr", "Company", "Name", "Period", "Entries", "Balance"}, rows);
        if (ids.empty())
        {
            view.message("Nothing to settle.");
            return;
        }
        std::vector<std::string> opts;
        for (size_t k = 0; k < ids.size(); k++)
            opts.push_back(ids[k] + "  " + rows[k][2] + "  (" + rows[k][5] + ")");
        opts.push_back("<< Back");
        int sel = view.select("Pick a company", opts);
        if (sel < 0 || sel >= (int)ids.size())
            return;
        int act = view.menu("Company " + ids[sel],
                            {"Settle now", "Print settlement report", "Back"});
        if (act == 0)
            settleCompany(ids[sel]);
        else if (act == 1)
            printSettlementReport(ids[sel]);
    }
}

void App::settleCompany(const std::string &companyId)
{
    view.clear();
    long long nb = 0;
    int n = 0;
    for (int i = 0; i < db.settlements.count(); i++)
    {
        Settlement &s = db.settlements.at(i);
        if (s.companyId == companyId && s.settled != "Y")
        {
            long long lb = 0;
            try { lb = std::stoll(s.labBalance); } catch (...) {}
            nb += lb;
            n++;
        }
    }
    std::string who = (nb < 0) ? ("Lab pays company " + to_string(-nb))
                      : (nb > 0 ? ("Company pays lab " + to_string(nb)) : "Nothing due");
    if (!view.confirm("Settle " + to_string(n) + " entr(ies) for " + companyId + "?  (" + who + ")"))
        return;
    for (int i = 0; i < db.settlements.count(); i++)
    {
        Settlement &s = db.settlements.at(i);
        if (s.companyId == companyId && s.settled != "Y")
            s.settled = "Y";
    }
    db.settlements.store();
    logAction("settled company " + companyId + " (" + to_string(n) + " entries, balance " + to_string(nb) + ")");
    view.message("Settled " + to_string(n) + " entr(ies). " + who + ".");
}

// A printable PDF of one company's settlement ledger.
void App::printSettlementReport(const std::string &companyId)
{
    int ck = db.companies.indexOf(companyId);
    std::string name = (ck >= 0) ? db.companies.at(ck).name : companyId;
    std::string period = (ck >= 0) ? db.companies.at(ck).settlementPeriod : "-";

    std::vector<std::vector<std::string>> rows;
    long long open = 0;
    int openN = 0, settledN = 0;
    for (int i = 0; i < db.settlements.count(); i++)
    {
        Settlement &s = db.settlements.at(i);
        if (s.companyId != companyId)
            continue;
        long long lb = 0;
        try { lb = std::stoll(s.labBalance); } catch (...) {}
        if (s.settled == "Y")
            settledN++;
        else { open += lb; openN++; }
        rows.push_back({s.date, s.invoiceId, s.amount, s.direction, s.labBalance,
                        (s.settled == "Y" ? "settled" : "open")});
    }
    std::string netLine = (open < 0) ? ("Outstanding: lab pays company " + to_string(-open))
                          : (open > 0 ? ("Outstanding: company owes lab " + to_string(open))
                                      : "Outstanding: nil");

    std::vector<std::vector<std::string>> info = {
        {"Company", companyId}, {"Name", name}, {"Period", period}, {"Date", Console::date()}};
    std::vector<std::string> footer = {
        netLine, to_string(openN) + " open / " + to_string(settledN) + " settled entr(ies)"};

    std::string path = "Exports/settlement_" + companyId + ".pdf";
    if (!Export::documentPdf(path, "Settlement Report - " + name, info,
                             {"Date", "Invoice", "Share", "Direction", "LabBal", "Status"},
                             rows, footer))
    {
        view.message("Could not write the settlement report.");
        return;
    }
    logAction("generated settlement report for " + companyId);
    if (view.confirm("Settlement report saved to " + path + ".  Open / print now?"))
        Console::openFile(path);
}

// A full PDF of a patient: every visit's tests, results and billing.
void App::printPatientReport(const std::string &patientId)
{
    int pk = db.patients.indexOf(patientId);
    if (pk < 0)
        return;
    Patient &p = db.patients.at(pk);

    std::vector<std::vector<std::string>> rows;
    long long billed = 0, paid = 0;
    for (int i = 0; i < db.invoices.count(); i++)
    {
        Invoice &inv = db.invoices.at(i);
        if (inv.patientId != patientId)
            continue;
        billed += static_cast<long long>(invoiceNet(inv.id) + 0.5);
        paid += static_cast<long long>(invoicePaid(inv.id) + 0.5);
        for (int t = 0; t < db.patientTests.count(); t++)
        {
            PatientTest &pt = db.patientTests.at(t);
            if (pt.invoiceId == inv.id)
                rows.push_back({inv.id, inv.date, pt.testName, pt.result, pt.status, pt.rate});
        }
    }
    long long bal = billed - paid;

    std::vector<std::vector<std::string>> info = {
        {"Patient", p.id}, {"Name", p.name}, {"CNIC", p.cnic}, {"Contact", p.contact},
        {"Gender / Age", p.gender + " / " + p.age}, {"Blood", p.bloodGroup}, {"Date", Console::date()}};
    std::vector<std::string> footer = {
        "Total billed : " + to_string(billed),
        "Received     : " + to_string(paid),
        "Balance      : " + to_string(bal > 0 ? bal : 0)};

    std::string path = "Exports/patient_report_" + patientId + ".pdf";
    if (!Export::documentPdf(path, "Patient Report - " + p.name, info,
                             {"Invoice", "Date", "Test", "Result", "Status", "Rate"},
                             rows, footer))
    {
        view.message("Could not write the patient report.");
        return;
    }
    logAction("generated patient report for " + patientId);
    if (view.confirm("Patient report saved to " + path + ".  Open / print now?"))
        Console::openFile(path);
}

// ---------------------------------------------------------------------------
// DOCTOR / CORPORATE PORTAL: a Companies & Doctors user sees their referred
// patients and their own settlement balance (read-only).
// ---------------------------------------------------------------------------
void App::doctorPortal()
{
    logAction("opened Doctor Portal");
    std::string companyId = currentUserCompanyId();
    if (companyId.empty())
    {
        view.clear();
        view.message("Your account is not linked to a company/doctor. Ask an admin.");
        return;
    }
    int ck = db.companies.indexOf(companyId);
    std::string cname = (ck >= 0) ? db.companies.at(ck).name : companyId;

    while (true)
    {
        view.clear();
        int c = view.menu("DOCTOR PORTAL - " + cname,
                          {"My Referred Patients", "My Settlement", "Back"});
        if (c == 2 || c < 0)
            return;

        db.loadAll();
        if (c == 0)
        {
            std::vector<std::vector<std::string>> rows;
            for (int i = 0; i < db.invoices.count(); i++)
            {
                Invoice &inv = db.invoices.at(i);
                if (inv.reference != cname)
                    continue;
                std::string pname = "?";
                int pidx = db.patients.indexOf(inv.patientId);
                if (pidx >= 0)
                    pname = db.patients.at(pidx).name;
                long long bal = static_cast<long long>(invoiceNet(inv.id) - invoicePaid(inv.id) + 0.5);
                rows.push_back({inv.id, inv.date, pname, inv.netTotal, to_string(bal > 0 ? bal : 0)});
            }
            view.clear();
            view.table("MY REFERRED PATIENTS (" + cname + ")",
                       {"Invoice", "Date", "Patient", "Net", "Balance"}, rows);
            view.message(to_string(rows.size()) + " referred visit(s).");
        }
        else // My Settlement
        {
            std::vector<std::vector<std::string>> rows;
            long long open = 0;
            for (int i = 0; i < db.settlements.count(); i++)
            {
                Settlement &s = db.settlements.at(i);
                if (s.companyId != companyId)
                    continue;
                long long lb = 0;
                try { lb = std::stoll(s.labBalance); } catch (...) {}
                if (s.settled != "Y")
                    open += lb;
                rows.push_back({s.date, s.invoiceId, s.amount, s.direction,
                                (s.settled == "Y" ? "settled" : "open")});
            }
            view.clear();
            view.table("MY SETTLEMENT (" + cname + ")",
                       {"Date", "Invoice", "Share", "Direction", "Status"}, rows);
            std::string msg = (open < 0) ? ("Lab owes you " + to_string(-open))
                              : (open > 0 ? ("You owe the lab " + to_string(open))
                                          : "Nothing outstanding");
            view.message(msg + ".");
        }
    }
}

// ---------------------------------------------------------------------------
// SAMPLE DISPATCH: a batch of invoices handed off for transport.
//   Collection Center creates (CREATED) -> Courier picks up (IN_TRANSIT) ->
//   Lab receives (RECEIVED), which marks those invoices' specimens collected.
// ---------------------------------------------------------------------------
bool App::invoiceInOpenDispatch(const std::string &invoiceId)
{
    for (int i = 0; i < db.dispatches.count(); i++)
    {
        Dispatch &d = db.dispatches.at(i);
        if (d.status == "RECEIVED")
            continue;
        for (const std::string &iv : Utils::split(d.invoiceIds, ';'))
            if (iv == invoiceId)
                return true;
    }
    return false;
}

// Collection Center: batch un-dispatched invoices that still need their
// specimens collected, and create a dispatch.
void App::collectionCenterModule()
{
    logAction("opened Collection Center");
    while (true)
    {
        view.clear();
        int c = view.menu("COLLECTION CENTER", {"New Dispatch", "My Dispatches", "Back"});
        if (c == 2 || c < 0)
            return;
        db.loadAll();

        if (c == 1) // My Dispatches
        {
            std::vector<std::vector<std::string>> rows;
            for (int i = 0; i < db.dispatches.count(); i++)
            {
                Dispatch &d = db.dispatches.at(i);
                int n = (int)Utils::split(d.invoiceIds, ';').size();
                rows.push_back({d.id, d.date, d.origin, to_string(n), d.status});
            }
            view.clear();
            view.table("DISPATCHES", {"ID", "Date", "Origin", "Invoices", "Status"}, rows);
            view.message(to_string(rows.size()) + " dispatch(es).");
            continue;
        }

        // New Dispatch: pick candidate invoices (pending specimens, not already
        // in an open dispatch).
        std::vector<std::string> chosen;
        while (true)
        {
            std::vector<std::string> opts, ids;
            for (int i = 0; i < db.invoices.count(); i++)
            {
                Invoice &inv = db.invoices.at(i);
                bool pending = false;
                for (int t = 0; t < db.patientTests.count(); t++)
                    if (db.patientTests.at(t).invoiceId == inv.id &&
                        db.patientTests.at(t).specimenTaken != "Y")
                        pending = true;
                if (!pending || invoiceInOpenDispatch(inv.id))
                    continue;
                if (std::find(chosen.begin(), chosen.end(), inv.id) != chosen.end())
                    continue;
                std::string pname = "?";
                int pk = db.patients.indexOf(inv.patientId);
                if (pk >= 0)
                    pname = db.patients.at(pk).name;
                opts.push_back(inv.id + "  " + pname + "  [" + inv.sampleLocation + "]");
                ids.push_back(inv.id);
            }
            view.clear();
            if (opts.empty())
            {
                if (chosen.empty())
                {
                    view.message("No samples awaiting dispatch.");
                    break;
                }
            }
            else
                opts.push_back("");
            opts.push_back(chosen.empty() ? "Cancel" : "Done (" + to_string(chosen.size()) + " added)");
            int sel = view.select("Add invoice to dispatch", opts);
            if (sel < 0 || sel >= (int)ids.size()) // Done/Cancel (or the blank)
                break;
            chosen.push_back(ids[sel]);
        }
        if (chosen.empty())
            continue;

        Dispatch d;
        d.id = db.dispatches.nextId();
        std::string me = session.fullName();
        int uk = db.users.indexOf(session.id());
        if (uk >= 0 && !db.users.at(uk).area.empty())
            me += " (" + db.users.at(uk).area + ")";
        d.origin = me;
        d.status = "CREATED";
        d.invoiceIds = Utils::join(chosen, ';');
        d.date = Console::date();
        db.dispatches.add(d);
        db.dispatches.store();
        logAction("created dispatch " + d.id + " with " + to_string((int)chosen.size()) + " invoice(s)");
        view.message("Dispatch " + d.id + " created with " + to_string((int)chosen.size()) + " invoice(s).");
    }
}

// Courier: pick up CREATED dispatches and carry them.
void App::courierModule()
{
    logAction("opened Courier");
    while (true)
    {
        view.clear();
        int c = view.menu("COURIER", {"Available Pickups", "My In-Transit", "Back"});
        if (c == 2 || c < 0)
            return;
        db.loadAll();

        std::string want = (c == 0) ? "CREATED" : "IN_TRANSIT";
        std::vector<std::string> opts, ids;
        for (int i = 0; i < db.dispatches.count(); i++)
        {
            Dispatch &d = db.dispatches.at(i);
            if (d.status != want)
                continue;
            if (c == 1 && d.courierId != session.id())
                continue;
            int n = (int)Utils::split(d.invoiceIds, ';').size();
            opts.push_back(d.id + "  " + d.origin + "  (" + to_string(n) + " invoices)  " + d.date);
            ids.push_back(d.id);
        }
        view.clear();
        if (opts.empty())
        {
            view.message(c == 0 ? "No dispatches awaiting pickup." : "Nothing in transit for you.");
            continue;
        }
        opts.push_back("<< Back");
        int sel = view.select(c == 0 ? "Pick up a dispatch" : "Your in-transit dispatches", opts);
        if (sel < 0 || sel >= (int)ids.size())
            continue;
        int di = db.dispatches.indexOf(ids[sel]);
        if (di < 0)
            continue;
        if (c == 0)
        {
            db.dispatches.at(di).courierId = session.id();
            db.dispatches.at(di).status = "IN_TRANSIT";
            db.dispatches.update(di);
            logAction("picked up dispatch " + ids[sel]);
            view.message("Dispatch " + ids[sel] + " picked up. Deliver it to the lab.");
        }
        else
            view.message("Dispatch " + ids[sel] + " is in transit. The lab will receive it.");
    }
}

// Lab: receive in-transit dispatches -> mark their specimens collected.
void App::receiveDispatches()
{
    logAction("opened Receive Samples");
    while (true)
    {
        db.loadAll();
        std::vector<std::string> opts, ids;
        for (int i = 0; i < db.dispatches.count(); i++)
        {
            Dispatch &d = db.dispatches.at(i);
            if (d.status != "IN_TRANSIT")
                continue;
            int n = (int)Utils::split(d.invoiceIds, ';').size();
            opts.push_back(d.id + "  from " + d.origin + "  (" + to_string(n) + " invoices)");
            ids.push_back(d.id);
        }
        view.clear();
        if (opts.empty())
        {
            view.message("No dispatches awaiting receipt.");
            return;
        }
        opts.push_back("<< Back");
        int sel = view.select("RECEIVE SAMPLES - pick a dispatch", opts);
        if (sel < 0 || sel >= (int)ids.size())
            return;
        int di = db.dispatches.indexOf(ids[sel]);
        if (di < 0)
            continue;
        Dispatch &d = db.dispatches.at(di);
        if (!view.confirm("Receive dispatch " + d.id + " and mark its specimens collected?"))
            continue;

        int marked = 0;
        for (const std::string &iv : Utils::split(d.invoiceIds, ';'))
            for (int t = 0; t < db.patientTests.count(); t++)
            {
                PatientTest &pt = db.patientTests.at(t);
                if (pt.invoiceId == iv && pt.specimenTaken != "Y")
                {
                    pt.specimenTaken = "Y";
                    marked++;
                }
            }
        if (marked)
            db.patientTests.store();
        d.status = "RECEIVED";
        db.dispatches.update(di);
        logAction("received dispatch " + d.id + " (" + to_string(marked) + " specimens collected)");
        view.message("Dispatch " + d.id + " received. " + to_string(marked) +
                     " specimen(s) marked collected - results can now be entered.");
    }
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
