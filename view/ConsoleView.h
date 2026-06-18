#ifndef ILMS_CONSOLE_VIEW_H
#define ILMS_CONSOLE_VIEW_H

#include "View.h"

// Console implementation of the View interface.
//
// It renders top-to-bottom (like a normal terminal program) using an internal
// row cursor, instead of the original program's hard-coded screen coordinates.
// A future GuiView would implement the same interface with real widgets.
class ConsoleView : public View
{
    int row = 1; // next free screen row; reset by clear()

    // internal helpers
    void drawBanner();
    void drawTitleArt(int x, int y);
    void drawDashboardPanels(const DashboardStats &stats);
    // Vertical navigable list drawn at an absolute (x,y); returns chosen index.
    int navList(int x, int y, const std::vector<std::string> &options);
    // Start a fresh screen if `needed` rows would overflow the window.
    void ensureSpace(int needed);
    int runMenu(const std::string &title, const std::vector<std::string> &options,
                bool allowEmpty);

public:
    void init() override;
    void clear() override;

    int menu(const std::string &title, const std::vector<std::string> &options) override;
    int select(const std::string &title, const std::vector<std::string> &options) override;
    std::vector<std::string> form(const std::string &title,
                                  const std::vector<std::string> &labels) override;
    std::string ask(const std::string &label) override;
    void table(const std::string &title,
               const std::vector<std::string> &headers,
               const std::vector<std::vector<std::string>> &rows) override;
    RowAction entityTable(const std::string &title,
                          const std::vector<std::string> &headers,
                          const std::vector<std::vector<std::string>> &rows) override;
    void message(const std::string &text) override;
    bool confirm(const std::string &question) override;
    bool logViewer(const std::string &title,
                   const std::vector<std::string> &lines) override;

    int splashScreen() override;
    Credentials loginScreen() override;
    int dashboardScreen(const std::string &fullName,
                        const DashboardStats &stats,
                        const std::vector<std::string> &menuOptions) override;
};

#endif // ILMS_CONSOLE_VIEW_H
