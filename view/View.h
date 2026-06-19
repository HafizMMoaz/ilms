#ifndef ILMS_VIEW_H
#define ILMS_VIEW_H

#include <string>
#include <vector>

// Username + password collected from the login screen.
struct Credentials
{
    std::string username;
    std::string password;
};

// Summary numbers shown on the dashboard. Plain ints, so the view stays
// decoupled from the model.
struct DashboardStats
{
    int patients = 0;
    int specimens = 0;
    int labDepartments = 0;
    int labTests = 0;
    int machines = 0;
    int packages = 0;
    int companies = 0;
};

// Result of an interactive entity table: which action the user picked and, for
// Edit/Delete, the index of the row it applies to.
struct RowAction
{
    enum Type
    {
        Add,
        Edit,
        Delete,
        Export, // export the whole table (toolbar)
        Back
    } type;
    int index; // selected record index (valid for Edit/Delete)
};

// ---------------------------------------------------------------------------
// Abstract presentation layer (the "V" in MVC).
//
// The controller talks ONLY to this interface, never to the console directly,
// so the entire UI can be swapped (ConsoleView today, a GuiView tomorrow)
// without touching the controller or the model.
//
// Everything crossing this boundary is plain strings/ints, so the view has no
// dependency on the model classes.
// ---------------------------------------------------------------------------
class View
{
public:
    virtual ~View() = default;

    // ---- lifecycle --------------------------------------------------------
    virtual void init() = 0;  // one-time setup (window size, etc.)
    virtual void clear() = 0; // begin a fresh screen

    // ---- generic widgets --------------------------------------------------
    // Vertical menu; returns the chosen index (0-based). Blocks for input.
    virtual int menu(const std::string &title,
                     const std::vector<std::string> &options) = 0;

    // Pick one entry from a list; returns its index, or -1 if the list is empty.
    virtual int select(const std::string &title,
                       const std::vector<std::string> &options) = 0;

    // Prompt for each label in order; returns the entered values (same order).
    virtual std::vector<std::string> form(const std::string &title,
                                          const std::vector<std::string> &labels) = 0;

    // Prompt for a single value.
    virtual std::string ask(const std::string &label) = 0;

    // Render a bordered table (read-only).
    virtual void table(const std::string &title,
                       const std::vector<std::string> &headers,
                       const std::vector<std::vector<std::string>> &rows) = 0;

    // Interactive entity table. The user moves Up/Down between rows and the
    // top toolbar, and Left/Right between buttons ([Add New]/[Back] on the
    // toolbar, [Edit]/[Delete] on each row). Returns the chosen action.
    virtual RowAction entityTable(const std::string &title,
                                  const std::vector<std::string> &headers,
                                  const std::vector<std::vector<std::string>> &rows) = 0;

    // Transient status / notification (waits for a keypress).
    virtual void message(const std::string &text) = 0;

    // Yes/No confirmation.
    virtual bool confirm(const std::string &question) = 0;

    // Read-only, searchable, paged viewer for plain text lines (the activity
    // log). Returns true if the user picked "Delete All", false to go back.
    virtual bool logViewer(const std::string &title,
                           const std::vector<std::string> &lines) = 0;

    // ---- branded screens --------------------------------------------------
    // Splash screen; returns 0 = LOGIN, 1 = EXIT.
    virtual int splashScreen() = 0;

    // Login screen; returns the entered credentials.
    virtual Credentials loginScreen() = 0;

    // Dashboard: draws the summary panels + a navigation menu, and returns the
    // chosen menu index.
    virtual int dashboardScreen(const std::string &fullName,
                                const DashboardStats &stats,
                                const std::vector<std::string> &menuOptions) = 0;
};

#endif // ILMS_VIEW_H
