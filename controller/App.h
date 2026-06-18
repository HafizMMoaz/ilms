#ifndef ILMS_APP_H
#define ILMS_APP_H

#include "Database.h"
#include "Session.h"
#include "Logger.h"
#include "View.h"

// ---------------------------------------------------------------------------
// Controller (the "C" in MVC).
//
// Owns the model (Database + Session) and drives the flow by calling the
// abstract View. It contains NO console code, so the same controller works
// with any View implementation (ConsoleView now, GuiView later).
// ---------------------------------------------------------------------------
class App
{
    Database db;
    Session session;
    Logger logger;
    View &view; // injected; the controller never knows the concrete type
    std::string loginTime;

    // Records an action in logs.txt against the current user.
    void logAction(const std::string &action)
    {
        logger.log(session.isActive() ? session.userName() : "", action);
    }

    // Admin = Admin (R008) or Super Admin (R009); only they may view the logs.
    bool isAdmin() const
    {
        std::string r = session.userRole();
        return r == "R008" || r == "R009";
    }

    // flow
    bool login();
    void sessionLoop();
    void setupModule();
    void patientModule();
    void patientRecords();
    void patientSummary();
    void rateListView();
    void backupModule();
    void logsModule(); // admin-only activity log viewer

    void companyModule();
    void addCompany();
    void editCompany(int i);
    void deleteCompany(int i);

    // Entity modules each follow the same shape: show an interactive table,
    // then dispatch the chosen RowAction to add/edit/delete. Edit/Delete take
    // the selected row index (no ID typing - the user picks the row).
    void specimenModule();
    void addSpecimen();
    void editSpecimen(int i);
    void deleteSpecimen(int i);

    void labDepartmentModule();
    void addLabDepartment();
    void editLabDepartment(int i);
    void deleteLabDepartment(int i);

    void labTestModule();
    void addLabTest();
    void editLabTest(int i);
    void deleteLabTest(int i);

    void machineModule();
    void addMachine();
    void editMachine(int i);
    void deleteMachine(int i);

    void packageModule();
    void addPackage();
    void editPackage(int i);
    void deletePackage(int i);

    // SOPs are tied to a specimen (the checklist for collecting it).
    void sopModule();
    void addSop();
    void editSop(int i);
    void deleteSop(int i);

    // patient registration + management of an existing patient
    void registerPatient();
    void managePatient(int i);     // per-patient menu (add test / collect specimen)
    void addTestToPatient(int i);  // append a new lab test to an existing patient
    void collectSpecimen(int i);   // mark a test's specimen collected (now or later)
    void deletePatient(int i);
    // Lets the user attach a package or pick individual tests. Fills in the
    // patient's tests/testCount/price/discount. Returns false if there is
    // nothing in the system to register (no packages and no lab tests).
    bool choosePatientTests(Patient &p, double &price);

public:
    explicit App(View &v) : view(v) {}
    void run();
};

#endif // ILMS_APP_H
