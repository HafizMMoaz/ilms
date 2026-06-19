#include "Export.h"

#include <fstream>
#include <direct.h> // _mkdir (MinGW)

namespace
{
    // Create the directory part of `path` if it does not exist.
    void ensureParentDir(const std::string &path)
    {
        std::size_t s = path.find_last_of("/\\");
        if (s != std::string::npos)
            _mkdir(path.substr(0, s).c_str());
    }

    // Quote a CSV field if it contains a comma, quote or newline.
    std::string csvField(const std::string &s)
    {
        bool needsQuote = s.find(',') != std::string::npos ||
                          s.find('"') != std::string::npos ||
                          s.find('\n') != std::string::npos;
        if (!needsQuote)
            return s;
        std::string out = "\"";
        for (char c : s)
            out += (c == '"') ? "\"\"" : std::string(1, c);
        out += "\"";
        return out;
    }

    // Escape a value for HTML.
    std::string esc(const std::string &s)
    {
        std::string o;
        for (char c : s)
        {
            if (c == '&') o += "&amp;";
            else if (c == '<') o += "&lt;";
            else if (c == '>') o += "&gt;";
            else o += c;
        }
        return o;
    }

    const char *STYLE =
        "<style>"
        "body{font-family:Segoe UI,Arial,sans-serif;margin:24px;color:#222}"
        "h1,h2{color:#0a5}"
        "table{border-collapse:collapse;width:100%;margin-top:10px}"
        "th,td{border:1px solid #bbb;padding:6px 10px;text-align:left}"
        "th{background:#0a5;color:#fff}"
        "tr:nth-child(even){background:#f4f4f4}"
        ".totals td{border:none;padding:2px 10px}"
        ".muted{color:#777}"
        "</style>";
}

namespace Export
{
    bool csv(const std::string &path,
             const std::vector<std::string> &headers,
             const std::vector<std::vector<std::string>> &rows)
    {
        ensureParentDir(path);
        std::ofstream f(path);
        if (!f)
            return false;
        for (std::size_t c = 0; c < headers.size(); c++)
            f << (c ? "," : "") << csvField(headers[c]);
        f << "\n";
        for (const auto &r : rows)
        {
            for (std::size_t c = 0; c < r.size(); c++)
                f << (c ? "," : "") << csvField(r[c]);
            f << "\n";
        }
        return true;
    }

    bool html(const std::string &path, const std::string &title,
              const std::vector<std::string> &headers,
              const std::vector<std::vector<std::string>> &rows)
    {
        ensureParentDir(path);
        std::ofstream f(path);
        if (!f)
            return false;
        f << "<!doctype html><html><head><meta charset=\"utf-8\"><title>"
          << esc(title) << "</title>" << STYLE << "</head><body>";
        f << "<h1>ILMS</h1><h2>" << esc(title) << "</h2>";
        f << "<table><thead><tr>";
        for (const auto &h : headers)
            f << "<th>" << esc(h) << "</th>";
        f << "</tr></thead><tbody>";
        for (const auto &r : rows)
        {
            f << "<tr>";
            for (const auto &cell : r)
                f << "<td>" << esc(cell) << "</td>";
            f << "</tr>";
        }
        f << "</tbody></table>";
        f << "<p class=\"muted\">" << rows.size() << " record(s).</p>";
        f << "</body></html>";
        return true;
    }

    bool invoice(const std::string &path, const InvoiceDoc &d)
    {
        ensureParentDir(path);
        std::ofstream f(path);
        if (!f)
            return false;
        f << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Invoice "
          << esc(d.id) << "</title>" << STYLE << "</head><body>";
        f << "<h1>ILMS - Integrated Lab Management System</h1>";
        f << "<h2>Invoice " << esc(d.id) << "</h2>";
        f << "<p><b>Date:</b> " << esc(d.date) << "<br>";
        f << "<b>Patient:</b> " << esc(d.patientName) << " (" << esc(d.patientId) << ")<br>";
        f << "<b>Contact:</b> " << esc(d.patientContact) << "<br>";
        f << "<b>Sample location:</b> " << esc(d.sampleLocation) << "<br>";
        f << "<b>Reference:</b> " << esc(d.reference) << "</p>";

        f << "<table><thead><tr><th>#</th><th>Test</th><th>Specimen</th><th>Rate</th></tr></thead><tbody>";
        int n = 1;
        for (const auto &t : d.tests)
        {
            std::string name = t.size() > 0 ? t[0] : "";
            std::string spec = t.size() > 1 ? t[1] : "";
            std::string rate = t.size() > 2 ? t[2] : "";
            f << "<tr><td>" << n++ << "</td><td>" << esc(name) << "</td><td>"
              << esc(spec) << "</td><td>" << esc(rate) << "</td></tr>";
        }
        f << "</tbody></table>";

        f << "<table class=\"totals\" style=\"width:300px;margin-top:14px\">";
        f << "<tr><td>Gross total</td><td>" << esc(d.gross) << "</td></tr>";
        f << "<tr><td>Discount</td><td>" << esc(d.discount) << "%</td></tr>";
        f << "<tr><td><b>Net total</b></td><td><b>" << esc(d.net) << "</b></td></tr>";
        f << "<tr><td>Paid</td><td>" << esc(d.paid) << "</td></tr>";
        f << "<tr><td><b>Balance</b></td><td><b>" << esc(d.balance) << "</b></td></tr>";
        f << "</table>";
        f << "<p class=\"muted\">Thank you. Print with Ctrl+P.</p>";
        f << "</body></html>";
        return true;
    }

    bool receipt(const std::string &path, const ReceiptDoc &d)
    {
        ensureParentDir(path);
        std::ofstream f(path);
        if (!f)
            return false;
        f << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Receipt "
          << esc(d.id) << "</title>" << STYLE << "</head><body>";
        f << "<h1>ILMS</h1><h2>Payment Receipt " << esc(d.id) << "</h2>";
        f << "<table class=\"totals\" style=\"width:360px\">";
        f << "<tr><td>Date</td><td>" << esc(d.date) << "</td></tr>";
        f << "<tr><td>Patient</td><td>" << esc(d.patientName) << "</td></tr>";
        f << "<tr><td>Invoice</td><td>" << esc(d.invoiceId) << "</td></tr>";
        f << "<tr><td><b>Amount paid</b></td><td><b>" << esc(d.amount) << "</b></td></tr>";
        f << "<tr><td>Total paid so far</td><td>" << esc(d.paidTotal) << "</td></tr>";
        f << "<tr><td><b>Balance remaining</b></td><td><b>" << esc(d.balance) << "</b></td></tr>";
        f << "</table>";
        f << "<p class=\"muted\">Thank you. Print with Ctrl+P.</p>";
        f << "</body></html>";
        return true;
    }
}
