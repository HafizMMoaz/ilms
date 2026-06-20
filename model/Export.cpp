#include "Export.h"
#include "Pdf.h"

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

        f << "<table><thead><tr><th>#</th><th>Test</th><th>Specimen</th><th>Rate</th>"
             "<th>Status</th><th>Result</th></tr></thead><tbody>";
        int n = 1;
        for (const auto &t : d.tests)
        {
            std::string name = t.size() > 0 ? t[0] : "";
            std::string spec = t.size() > 1 ? t[1] : "";
            std::string rate = t.size() > 2 ? t[2] : "";
            std::string status = t.size() > 3 ? t[3] : "";
            std::string result = t.size() > 4 ? t[4] : "";
            f << "<tr><td>" << n++ << "</td><td>" << esc(name) << "</td><td>"
              << esc(spec) << "</td><td>" << esc(rate) << "</td><td>" << esc(status)
              << "</td><td>" << esc(result) << "</td></tr>";
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

    bool report(const std::string &path, const ReportDoc &d)
    {
        ensureParentDir(path);
        std::ofstream f(path);
        if (!f)
            return false;
        f << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Lab Report "
          << esc(d.invoiceId) << "</title>" << STYLE << "</head><body>";
        f << "<h1>ILMS - Laboratory Report</h1>";
        f << "<p><b>Patient:</b> " << esc(d.patientName) << " (" << esc(d.patientId) << ")"
          << " &nbsp; <b>Gender:</b> " << esc(d.gender)
          << " &nbsp; <b>Age:</b> " << esc(d.age)
          << " &nbsp; <b>Blood group:</b> " << esc(d.bloodGroup) << "<br>";
        f << "<b>Contact:</b> " << esc(d.patientContact)
          << " &nbsp; <b>Reference:</b> " << esc(d.reference) << "<br>";
        f << "<b>Invoice:</b> " << esc(d.invoiceId)
          << " &nbsp; <b>Date:</b> " << esc(d.date) << "</p>";

        f << "<table><thead><tr><th>#</th><th>Test</th><th>Result</th><th>Unit</th>"
             "<th>Status</th></tr></thead><tbody>";
        int n = 1;
        for (const auto &t : d.tests)
        {
            std::string name = t.size() > 0 ? t[0] : "";
            std::string result = t.size() > 1 ? t[1] : "";
            std::string unit = t.size() > 2 ? t[2] : "";
            std::string status = t.size() > 3 ? t[3] : "";
            f << "<tr><td>" << n++ << "</td><td>" << esc(name) << "</td><td>"
              << esc(result) << "</td><td>" << esc(unit) << "</td><td>"
              << esc(status) << "</td></tr>";
        }
        f << "</tbody></table>";
        f << "<p class=\"muted\">Results pending tests are blank. Print with Ctrl+P.</p>";
        f << "</body></html>";
        return true;
    }

    // ---- PDF variants (libHaru-backed: bordered tables + optional logo) ---
    namespace
    {
        // Prepend a 1-based "#" column to the document's test rows.
        std::vector<std::vector<std::string>> numbered(const std::vector<std::vector<std::string>> &src)
        {
            std::vector<std::vector<std::string>> out;
            int n = 1;
            for (const auto &t : src)
            {
                std::vector<std::string> row;
                row.push_back(std::to_string(n++));
                for (const auto &x : t)
                    row.push_back(x);
                out.push_back(row);
            }
            return out;
        }
    }

    bool tablePdf(const std::string &path, const std::string &title,
                  const std::vector<std::string> &headers,
                  const std::vector<std::vector<std::string>> &rows)
    {
        Pdf pdf;
        if (!pdf.valid())
            return false;
        pdf.heading(title);
        pdf.table(headers, rows);
        pdf.blank();
        pdf.line(std::to_string(rows.size()) + " record(s).");
        return pdf.save(path);
    }

    bool invoicePdf(const std::string &path, const InvoiceDoc &d)
    {
        Pdf pdf;
        if (!pdf.valid())
            return false;
        pdf.heading("Invoice " + d.id);
        pdf.keyVal("Date", d.date);
        pdf.keyVal("Patient", d.patientName + " (" + d.patientId + ")");
        pdf.keyVal("Contact", d.patientContact);
        pdf.keyVal("Location", d.sampleLocation);
        pdf.keyVal("Reference", d.reference);
        pdf.blank();
        pdf.table({"#", "Test", "Specimen", "Rate", "Status", "Result"}, numbered(d.tests));
        pdf.blank();
        pdf.keyVal("Gross total", d.gross);
        pdf.keyVal("Discount", d.discount + "%");
        pdf.keyVal("Net total", d.net);
        pdf.keyVal("Paid", d.paid);
        pdf.keyVal("Balance", d.balance);
        return pdf.save(path);
    }

    bool receiptPdf(const std::string &path, const ReceiptDoc &d)
    {
        Pdf pdf;
        if (!pdf.valid())
            return false;
        pdf.heading("Payment Receipt " + d.id);
        pdf.keyVal("Date", d.date);
        pdf.keyVal("Patient", d.patientName);
        pdf.keyVal("Invoice", d.invoiceId);
        pdf.keyVal("Amount paid", d.amount);
        pdf.keyVal("Total paid", d.paidTotal);
        pdf.keyVal("Balance", d.balance);
        return pdf.save(path);
    }

    bool reportPdf(const std::string &path, const ReportDoc &d)
    {
        Pdf pdf;
        if (!pdf.valid())
            return false;
        pdf.heading("Laboratory Report");
        pdf.keyVal("Patient", d.patientName + " (" + d.patientId + ")");
        pdf.keyVal("Gender / Age", d.gender + " / " + d.age + "   Blood: " + d.bloodGroup);
        pdf.keyVal("Contact", d.patientContact);
        pdf.keyVal("Reference", d.reference);
        pdf.keyVal("Invoice / Date", d.invoiceId + "   " + d.date);
        pdf.blank();
        pdf.table({"#", "Test", "Result", "Unit", "Status"}, numbered(d.tests));
        return pdf.save(path);
    }
}
