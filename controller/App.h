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

    // Super Admin (R009) only - manages roles.
    bool isSuperAdmin() const { return session.userRole() == "R009"; }

    // flow
    bool login();
    void sessionLoop();
    void setupModule();
    void patientModule();
    void patientRecords();
    void sampleReceiving(); // collect specimens across patients
    void resultEntry();     // enter results for collected specimens

    // Home sampling: assign + collect home-sample visits (payment-gated).
    void homeSamplingModule();
    void homeSampleDetail(const std::string &invoiceId);
    void collectHomeSample(const std::string &invoiceId);
    std::string homeSamplingRoleId(); // id of the "Home Sampling" role
    void patientSummary();
    void rateListView();
    void backupModule();
    void logsModule(); // admin-only activity log viewer

    // Dynamic roles (Super Admin only).
    void rolesModule();
    void addRole();
    void editRole(int i);
    void deleteRole(int i);

    // Users management (Admin / Super Admin).
    void usersModule();
    void addUser();
    void editUser(int i);
    void deleteUser(int i);

    // Home-sampling service areas (Setup).
    void areasModule();
    void addArea();
    void editArea(int i);
    void deleteArea(int i);

    // Reports (read-only summaries, each exportable).
    void reportsModule();
    void financialReport();
    void testStatusReport();
    void referralReport();
    void showReport(const std::string &title, const std::vector<std::string> &headers,
                    const std::vector<std::vector<std::string>> &rows);

    void companyModule();
    void addCompany();
    void editCompany(int i);
    void deleteCompany(int i);
    void captureCompanyTerms(Company &c);     // share/coupon/settlement prompts
    int companyByCoupon(const std::string &code); // index of company with that coupon, -1 if none

    // Corporate share accounting + settlement.
    void accrueShare(int companyIdx, const std::string &invoiceId, double net, int numTests);
    void settlementsModule();
    void settleCompany(const std::string &companyId);
    std::string corpRoleId();             // id of the "Companies & Doctors" role
    std::string currentUserCompanyId();   // company linked to the logged-in user (if any)

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

    // patient registration + management (relational: patient/invoice/test/payment)
    void registerPatient();        // new patient + first visit
    void managePatient(int i);     // per-patient menu (new visit / invoices)
    void deletePatient(int i);     // cascades to the patient's invoices/tests/payments
    void patientInvoices(const std::string &patientId); // list a patient's invoices
    void invoiceDetail(const std::string &invoiceId);   // tests + payments + balance
    void addPayment(const std::string &invoiceId);

    // Creates an invoice (+ its tests + an opening payment) for an existing
    // patient. Returns the new invoice id, or "" if nothing could be ordered.
    std::string orderTests(const std::string &patientId);
    // Adds PatientTest rows for the chosen package/individual tests; accumulates
    // the gross total and (for a package) sets the discount. Returns false if
    // there is nothing to order.
    bool chooseTests(const std::string &invoiceId, const std::string &patientId,
                     double &gross, std::string &discount);

    // Billing helpers.
    double invoiceNet(const std::string &invoiceId);
    double invoicePaid(const std::string &invoiceId);
    double patientBalance(const std::string &patientId);

    // Export the currently shown table (CSV / HTML) + printable documents.
    void exportTable(const std::string &title,
                     const std::vector<std::string> &headers,
                     const std::vector<std::vector<std::string>> &rows);
    void printInvoice(const std::string &invoiceId);
    void printReceipt(const std::string &invoiceId, const std::string &paymentId);
    void printReport(const std::string &invoiceId);

public:
    explicit App(View &v) : view(v) {}
    void run();
};

#endif // ILMS_APP_H
