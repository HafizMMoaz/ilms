#include "ConsoleView.h"
#include "Console.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <conio.h>

using namespace std;

namespace
{
    const int COL_NORMAL = 0x07;    // light grey on black
    const int COL_HILITE = 0x70;    // black on white (selected row)
    const int COL_TITLE = 0x0B;     // cyan
    const int LEFT = 2;             // left margin

    // Case-insensitive "does hay contain needle?" (empty needle matches all).
    bool containsCI(const string &hay, const string &needle)
    {
        if (needle.empty())
            return true;
        string h = hay, n = needle;
        transform(h.begin(), h.end(), h.begin(), [](unsigned char c) { return (char)tolower(c); });
        transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return (char)tolower(c); });
        return h.find(n) != string::npos;
    }

    // Draw one menu option, highlighting it if selected.
    void drawOption(int y, const string &text, bool selected)
    {
        Console::gotoxy(LEFT + 2, y);
        Console::setColor(selected ? COL_HILITE : COL_NORMAL);
        cout << (selected ? "> " : "  ") << text << "    ";
        Console::setColor(COL_NORMAL);
    }
}

void ConsoleView::init()
{
    Console::screenSetup(120, 300, 120, 40);
}

void ConsoleView::clear()
{
    system("cls");
    row = 1;
}

void ConsoleView::ensureSpace(int needed)
{
    // Keep output within the visible window; start a new screen if it won't fit.
    if (row + needed > 37)
        clear();
}

void ConsoleView::drawBanner()
{
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, row++);
    cout << "ILMS : Integrated Lab Management System";
    Console::setColor(COL_NORMAL);
    row++;
}

void ConsoleView::drawTitleArt(int x, int y)
{
    Console::setColor(COL_TITLE);
    Console::gotoxy(x, y);     cout << "-------------------------------------";
    Console::gotoxy(x, y + 1); cout << "****** **       **     **   *******  ";
    Console::gotoxy(x, y + 2); cout << "  **   **       ***   ***  ********* ";
    Console::gotoxy(x, y + 3); cout << "  **   **       ** * * ** *          ";
    Console::gotoxy(x, y + 4); cout << "  **   **       **  *  **  ********  ";
    Console::gotoxy(x, y + 5); cout << "  **   **       **     **          * ";
    Console::gotoxy(x, y + 6); cout << "  **   ******** **     ** *********  ";
    Console::gotoxy(x, y + 7); cout << "****** ******** **     **  *******   ";
    Console::gotoxy(x, y + 8); cout << "-------------------------------------";
    Console::gotoxy(x, y + 9); cout << "   INTEGRATED LAB MANAGEMENT SYSTEM  ";
    Console::gotoxy(x, y + 10); cout << "-------------------------------------";
    Console::setColor(COL_NORMAL);
}

// Shared arrow-key menu loop used by menu(), select() and confirm().
int ConsoleView::runMenu(const string &title, const vector<string> &options, bool allowEmpty)
{
    if (options.empty())
        return allowEmpty ? -1 : 0;

    ensureSpace(static_cast<int>(options.size()) + 3);
    int titleRow = row;
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, titleRow);
    cout << title;
    Console::setColor(COL_NORMAL);

    int optStart = titleRow + 2;
    int n = static_cast<int>(options.size());
    int sel = 0;
    for (int i = 0; i < n; i++)
        drawOption(optStart + i, options[i], i == sel);

    while (true)
    {
        int ch = _getch();
        if (ch == 0 || ch == 224)
        {
            ch = _getch();
            if (ch == 72 && sel > 0) // up
            {
                drawOption(optStart + sel, options[sel], false);
                sel--;
                drawOption(optStart + sel, options[sel], true);
            }
            else if (ch == 80 && sel < n - 1) // down
            {
                drawOption(optStart + sel, options[sel], false);
                sel++;
                drawOption(optStart + sel, options[sel], true);
            }
        }
        else if (ch == '\r' || ch == '\n')
            break;
    }

    row = optStart + n + 1;
    Console::gotoxy(LEFT, row);
    return sel;
}

int ConsoleView::menu(const string &title, const vector<string> &options)
{
    return runMenu(title, options, false);
}

int ConsoleView::select(const string &title, const vector<string> &options)
{
    return runMenu(title, options, true);
}

vector<string> ConsoleView::form(const string &title, const vector<string> &labels)
{
    ensureSpace(static_cast<int>(labels.size()) + 3);
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, row++);
    cout << title;
    Console::setColor(COL_NORMAL);
    row++;

    Console::cursor(true);
    vector<string> values;
    for (const auto &label : labels)
    {
        Console::gotoxy(LEFT, row);
        cout << label << ": ";
        string value;
        getline(cin, value);
        values.push_back(value);
        row++;
    }
    Console::cursor(false);
    row++;
    return values;
}

string ConsoleView::ask(const string &label)
{
    ensureSpace(3);
    Console::cursor(true);
    Console::gotoxy(LEFT, row);
    cout << label << ": ";
    string value;
    getline(cin, value);
    Console::cursor(false);
    row += 2;
    return value;
}

void ConsoleView::table(const string &title,
                        const vector<string> &headers,
                        const vector<vector<string>> &rows)
{
    ensureSpace(static_cast<int>(rows.size()) + 5);
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, row++);
    cout << title;
    Console::setColor(COL_NORMAL);

    int cols = static_cast<int>(headers.size());
    vector<int> width(cols, 0);
    for (int c = 0; c < cols; c++)
        width[c] = static_cast<int>(headers[c].length());
    for (const auto &r : rows)
        for (int c = 0; c < cols && c < (int)r.size(); c++)
            width[c] = max(width[c], static_cast<int>(r[c].length()));

    auto separator = [&]()
    {
        Console::gotoxy(LEFT, row++);
        cout << "+";
        for (int c = 0; c < cols; c++)
        {
            cout << string(width[c] + 2, '-') << "+";
        }
    };

    auto printRow = [&](const vector<string> &r)
    {
        Console::gotoxy(LEFT, row++);
        cout << "|";
        for (int c = 0; c < cols; c++)
        {
            string cell = (c < (int)r.size()) ? r[c] : "";
            cout << " " << left << setw(width[c]) << cell << " |";
        }
    };

    separator();
    printRow(headers);
    separator();
    if (rows.empty())
    {
        Console::gotoxy(LEFT, row++);
        cout << "|  (no records yet)";
    }
    else
    {
        for (const auto &r : rows)
            printRow(r);
    }
    separator();
    row++;
}

void ConsoleView::message(const string &text)
{
    ensureSpace(3);
    Console::gotoxy(LEFT, row++);
    cout << ">> " << text;
    Console::gotoxy(LEFT, row++);
    Console::setColor(0x08);
    cout << "   (press any key to continue)";
    Console::setColor(COL_NORMAL);
    _getch();
    row++;
}

bool ConsoleView::confirm(const string &question)
{
    return runMenu(question, {"Yes", "No"}, false) == 0;
}

bool ConsoleView::logViewer(const string &title, const vector<string> &allLines)
{
    const int PAGE_SIZE = 12;
    const int titleY = 2, statusY = 3, toolbarY = 4, firstRowY = 6;
    const int helpY = firstRowY + PAGE_SIZE + 1;
    const int blankW = 116;

    string query;
    vector<int> filtered;
    int page = 0, focusCol = 0; // toolbar only: 0 = Delete All, 1 = Back

    auto recompute = [&]()
    {
        filtered.clear();
        for (int i = 0; i < (int)allLines.size(); i++)
            if (query.empty() || containsCI(allLines[i], query))
                filtered.push_back(i);
        int totalPages = max(1, (int)(filtered.size() + PAGE_SIZE - 1) / PAGE_SIZE);
        if (page >= totalPages) page = totalPages - 1;
        if (page < 0) page = 0;
    };
    recompute();

    auto blank = [&](int y) { Console::gotoxy(LEFT, y); cout << string(blankW, ' '); };
    auto button = [&](int x, int y, const string &label, bool hot)
    {
        Console::gotoxy(x, y);
        Console::setColor(hot ? COL_HILITE : COL_NORMAL);
        cout << "[ " << label << " ]";
        Console::setColor(COL_NORMAL);
    };

    clear();
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, titleY);
    cout << title;
    Console::setColor(COL_NORMAL);
    Console::setColor(0x08);
    Console::gotoxy(LEFT, helpY);
    cout << "Left/Right: button   Enter: select   n/p: page   s: search   c: clear";
    Console::setColor(COL_NORMAL);

    auto render = [&]()
    {
        int total = (int)filtered.size();
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
        int start = page * PAGE_SIZE;
        int rows = min(PAGE_SIZE, total - start);
        if (rows < 0) rows = 0;

        blank(statusY);
        Console::gotoxy(LEFT, statusY);
        cout << "Search: " << (query.empty() ? "(none)" : query)
             << "    Page " << (page + 1) << "/" << totalPages
             << "    " << total << " line(s)";

        blank(toolbarY);
        button(LEFT, toolbarY, "Delete All", focusCol == 0);
        button(LEFT + 18, toolbarY, "Back", focusCol == 1);

        for (int s = 0; s < PAGE_SIZE; s++)
        {
            int y = firstRowY + s;
            blank(y);
            if (s < rows)
            {
                string ln = allLines[filtered[start + s]];
                if ((int)ln.size() > blankW - 1)
                    ln = ln.substr(0, blankW - 1);
                Console::gotoxy(LEFT, y);
                cout << ln;
            }
        }
        if (rows == 0)
        {
            Console::gotoxy(LEFT, firstRowY);
            cout << (query.empty() ? "(no activity logged yet)" : "(no matching lines)");
        }
    };
    render();

    while (true)
    {
        int total = (int)filtered.size();
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
        int ch = _getch();
        if (ch == 0 || ch == 224)
        {
            ch = _getch();
            if (ch == 75) focusCol = 0;                          // left
            else if (ch == 77) focusCol = 1;                     // right
            else if (ch == 73 && page > 0) page--;               // PageUp
            else if (ch == 81 && page < totalPages - 1) page++;  // PageDown
            render();
        }
        else if (ch == '\r' || ch == '\n')
            return focusCol == 0; // true == Delete All
        else if (ch == 'n' || ch == 'N')
        {
            if (page < totalPages - 1) page++;
            render();
        }
        else if (ch == 'p' || ch == 'P')
        {
            if (page > 0) page--;
            render();
        }
        else if (ch == 's' || ch == 'S')
        {
            blank(statusY);
            Console::gotoxy(LEFT, statusY);
            cout << "Search: ";
            Console::cursor(true);
            string q;
            getline(cin, q);
            Console::cursor(false);
            query = q;
            recompute();
            page = 0;
            render();
        }
        else if (ch == 'c' || ch == 'C')
        {
            query.clear();
            recompute();
            page = 0;
            render();
        }
    }
}

int ConsoleView::splashScreen()
{
    clear();
    drawTitleArt(40, 4);
    row = 17;
    return runMenu("Welcome - choose an option", {"LOGIN", "EXIT"}, false);
}

Credentials ConsoleView::loginScreen()
{
    clear();
    drawTitleArt(40, 4);
    row = 17;
    Credentials cr;
    auto values = form("LOGIN", {"Username", "Password"});
    cr.username = values.size() > 0 ? values[0] : "";
    cr.password = values.size() > 1 ? values[1] : "";
    return cr;
}

// A simple vertical navigable list drawn at an absolute position.
int ConsoleView::navList(int x, int y, const vector<string> &options)
{
    int n = static_cast<int>(options.size());
    if (n == 0)
        return -1;
    int sel = 0;
    auto draw = [&]()
    {
        for (int i = 0; i < n; i++)
        {
            Console::gotoxy(x, y + i);
            Console::setColor(i == sel ? COL_HILITE : COL_NORMAL);
            cout << (i == sel ? "> " : "  ") << options[i] << "    ";
            Console::setColor(COL_NORMAL);
        }
    };
    draw();
    while (true)
    {
        int ch = _getch();
        if (ch == 0 || ch == 224)
        {
            ch = _getch();
            if (ch == 72 && sel > 0)
                sel--;
            else if (ch == 80 && sel < n - 1)
                sel++;
            draw();
        }
        else if (ch == '\r' || ch == '\n')
            break;
    }
    return sel;
}

void ConsoleView::drawDashboardPanels(const DashboardStats &s)
{
    const int w = 32;
    auto box = [&](int bx, int by, const string &title, const vector<string> &lines)
    {
        Console::gotoxy(bx, by);     cout << "+" << string(w - 2, '-') << "+";
        Console::gotoxy(bx, by + 1); cout << "|" << string(w - 2, ' ') << "|";
        Console::setColor(COL_TITLE);
        Console::gotoxy(bx + (w - 2 - static_cast<int>(title.size())) / 2, by + 1);
        cout << title;
        Console::setColor(COL_NORMAL);
        Console::gotoxy(bx, by + 2); cout << "+" << string(w - 2, '-') << "+";
        int ly = by + 3;
        for (const auto &ln : lines)
        {
            Console::gotoxy(bx, ly); cout << "| " << ln;
            Console::gotoxy(bx + w - 1, ly); cout << "|";
            ly++;
        }
        Console::gotoxy(bx, ly); cout << "+" << string(w - 2, '-') << "+";
    };

    int x = 28, y = 5;
    box(x, y, "CASH SUMMARY",
        {"Patients : " + to_string(s.patients), "Paid     : --",
         "Balance  : --", "Expenses : --"});
    box(x + 36, y, "PATIENT SUMMARY",
        {"Patients  : " + to_string(s.patients), "Lab Tests : " + to_string(s.labTests),
         "Specimens : " + to_string(s.specimens), "Machines  : " + to_string(s.machines)});
    box(x, y + 8, "REVENUE", {"Today : --", "Total : --"});
    box(x + 36, y + 8, "INVENTORY",
        {"Lab Depts : " + to_string(s.labDepartments),
         "Packages  : " + to_string(s.packages),
         "Companies : " + to_string(s.companies)});
}

int ConsoleView::dashboardScreen(const string &fullName, const DashboardStats &stats,
                                 const vector<string> &menuOptions)
{
    clear();
    drawBanner();
    Console::gotoxy(LEFT, 3);
    cout << "Welcome, " << fullName << "!";
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, 5);
    cout << "MENU";
    Console::setColor(COL_NORMAL);
    drawDashboardPanels(stats);
    return navList(LEFT, 7, menuOptions);
}

RowAction ConsoleView::entityTable(const string &title, const vector<string> &headers,
                                   const vector<vector<string>> &allRows)
{
    const int PAGE_SIZE = 8;
    int cols = static_cast<int>(headers.size());

    // Column widths are computed from ALL rows so the layout stays stable as
    // the user pages/filters.
    vector<int> width(cols, 0);
    for (int c = 0; c < cols; c++)
        width[c] = static_cast<int>(headers[c].size());
    for (const auto &r : allRows)
        for (int c = 0; c < cols && c < (int)r.size(); c++)
            width[c] = max(width[c], static_cast<int>(r[c].size()));

    int tableWidth = 1;
    for (int c = 0; c < cols; c++)
        tableWidth += width[c] + 3;
    int buttonX = LEFT + tableWidth + 3;
    int blankW = min(buttonX + 22, 118) - LEFT; // safe line-clear width

    const int titleY = 2, statusY = 3, toolbarY = 4;
    const int topSepY = 6, headerY = 7, midSepY = 8, firstRowY = 9;
    const int botSepY = firstRowY + PAGE_SIZE;
    const int helpY = botSepY + 2;

    string query;
    vector<int> filtered; // original indices of rows passing the search filter
    int page = 0, focusRow = -1, focusCol = 0;

    auto recompute = [&]()
    {
        filtered.clear();
        for (int i = 0; i < (int)allRows.size(); i++)
        {
            bool match = query.empty();
            if (!match)
                for (const auto &cell : allRows[i])
                    if (containsCI(cell, query)) { match = true; break; }
            if (match)
                filtered.push_back(i);
        }
        int totalPages = max(1, (int)(filtered.size() + PAGE_SIZE - 1) / PAGE_SIZE);
        if (page >= totalPages) page = totalPages - 1;
        if (page < 0) page = 0;
    };
    recompute();

    auto sepLine = [&](int y)
    {
        Console::gotoxy(LEFT, y);
        cout << "+";
        for (int c = 0; c < cols; c++)
            cout << string(width[c] + 2, '-') << "+";
    };
    auto rowLine = [&](int y, const vector<string> &r)
    {
        Console::gotoxy(LEFT, y);
        cout << "|";
        for (int c = 0; c < cols; c++)
        {
            string cell = (c < (int)r.size()) ? r[c] : "";
            cout << " " << left << setw(width[c]) << cell << " |";
        }
    };
    auto blank = [&](int y) { Console::gotoxy(LEFT, y); cout << string(blankW, ' '); };
    auto button = [&](int x, int y, const string &label, bool hot)
    {
        Console::gotoxy(x, y);
        Console::setColor(hot ? COL_HILITE : COL_NORMAL);
        cout << "[ " << label << " ]";
        Console::setColor(COL_NORMAL);
    };

    // Static frame (drawn once).
    clear();
    Console::setColor(COL_TITLE);
    Console::gotoxy(LEFT, titleY);
    cout << title;
    Console::setColor(COL_NORMAL);
    sepLine(topSepY);
    rowLine(headerY, headers);
    sepLine(midSepY);
    sepLine(botSepY);
    Console::setColor(0x08);
    Console::gotoxy(LEFT, helpY);
    cout << "Up/Down,Left/Right: move   Enter: select   n/p: page   s: search   c: clear";
    Console::setColor(COL_NORMAL);

    auto render = [&]()
    {
        int total = (int)filtered.size();
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
        int pageStart = page * PAGE_SIZE;
        int pageRows = min(PAGE_SIZE, total - pageStart);
        if (pageRows < 0) pageRows = 0;

        blank(statusY);
        Console::gotoxy(LEFT, statusY);
        cout << "Search: " << (query.empty() ? "(none)" : query)
             << "    Page " << (page + 1) << "/" << totalPages
             << "    " << total << " record(s)";

        blank(toolbarY);
        button(LEFT, toolbarY, "Add New", focusRow == -1 && focusCol == 0);
        button(LEFT + 14, toolbarY, "Export", focusRow == -1 && focusCol == 1);
        button(LEFT + 26, toolbarY, "Back", focusRow == -1 && focusCol == 2);

        for (int s = 0; s < PAGE_SIZE; s++)
        {
            int y = firstRowY + s;
            blank(y);
            if (s < pageRows)
            {
                int idx = filtered[pageStart + s];
                rowLine(y, allRows[idx]);
                button(buttonX, y, "Edit", focusRow == s && focusCol == 0);
                button(buttonX + 10, y, "Delete", focusRow == s && focusCol == 1);
            }
        }
        if (pageRows == 0)
        {
            Console::gotoxy(LEFT, firstRowY);
            cout << (query.empty() ? "(no records yet)" : "(no matching records)");
        }
    };
    render();

    while (true)
    {
        int total = (int)filtered.size();
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
        int pageStart = page * PAGE_SIZE;
        int pageRows = min(PAGE_SIZE, total - pageStart);
        if (pageRows < 0) pageRows = 0;

        int ch = _getch();
        if (ch == 0 || ch == 224)
        {
            ch = _getch();
            if (ch == 72 && focusRow > -1)         focusRow--;                 // up
            else if (ch == 80 && focusRow < pageRows - 1) focusRow++;          // down
            else if (ch == 75)                     { if (focusCol > 0) focusCol--; }            // left
            else if (ch == 77)                     { int mx = (focusRow == -1) ? 2 : 1; if (focusCol < mx) focusCol++; } // right
            else if (ch == 73 && page > 0)         { page--; focusRow = -1; }  // PageUp
            else if (ch == 81 && page < totalPages - 1) { page++; focusRow = -1; } // PageDown
            if (focusRow >= 0 && focusCol > 1) focusCol = 1; // rows have only 2 buttons
            render();
        }
        else if (ch == '\r' || ch == '\n')
        {
            RowAction a;
            if (focusRow == -1)
            {
                a.type = (focusCol == 0) ? RowAction::Add
                         : (focusCol == 1) ? RowAction::Export
                                           : RowAction::Back;
                a.index = -1;
            }
            else
            {
                a.type = (focusCol == 0) ? RowAction::Edit : RowAction::Delete;
                a.index = filtered[pageStart + focusRow];
            }
            return a;
        }
        else if (ch == 'n' || ch == 'N')
        {
            if (page < totalPages - 1) { page++; focusRow = -1; }
            render();
        }
        else if (ch == 'p' || ch == 'P')
        {
            if (page > 0) { page--; focusRow = -1; }
            render();
        }
        else if (ch == 's' || ch == 'S')
        {
            blank(statusY);
            Console::gotoxy(LEFT, statusY);
            cout << "Search: ";
            Console::cursor(true);
            string q;
            getline(cin, q);
            Console::cursor(false);
            query = q;
            recompute();
            page = 0;
            focusRow = -1;
            render();
        }
        else if (ch == 'c' || ch == 'C')
        {
            query.clear();
            recompute();
            page = 0;
            focusRow = -1;
            render();
        }
    }
}
