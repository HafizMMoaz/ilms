#ifndef ILMS_EXPORT_H
#define ILMS_EXPORT_H

#include <string>
#include <vector>

// Writes data out to files: generic table exports (CSV / HTML) and printable
// HTML documents (invoice / payment receipt). HTML opens in a browser, which
// gives printing / "Save as PDF" for free - no third-party library required.
namespace Export
{
    // ---- generic table exports -------------------------------------------
    bool csv(const std::string &path,
             const std::vector<std::string> &headers,
             const std::vector<std::vector<std::string>> &rows);

    bool html(const std::string &path, const std::string &title,
              const std::vector<std::string> &headers,
              const std::vector<std::vector<std::string>> &rows);

    // ---- printable documents ---------------------------------------------
    struct InvoiceDoc
    {
        std::string id, date, patientId, patientName, patientContact;
        std::string sampleLocation, reference, discount, gross, net, paid, balance;
        std::vector<std::vector<std::string>> tests; // each: {name, specimen, rate, status, result}
    };
    bool invoice(const std::string &path, const InvoiceDoc &d);

    struct ReceiptDoc
    {
        std::string id, date, patientName, invoiceId, amount, paidTotal, balance;
    };
    bool receipt(const std::string &path, const ReceiptDoc &d);

    // A lab report: the patient's tests and their results for one invoice.
    struct ReportDoc
    {
        std::string invoiceId, date, patientId, patientName, patientContact,
            gender, age, bloodGroup, reference;
        std::vector<std::vector<std::string>> tests; // each: {test, result, unit, status}
    };
    bool report(const std::string &path, const ReportDoc &d);

    // ---- PDF variants (real .pdf, no library) ----------------------------
    bool tablePdf(const std::string &path, const std::string &title,
                  const std::vector<std::string> &headers,
                  const std::vector<std::vector<std::string>> &rows);
    bool invoicePdf(const std::string &path, const InvoiceDoc &d);
    bool receiptPdf(const std::string &path, const ReceiptDoc &d);
    bool reportPdf(const std::string &path, const ReportDoc &d);

    // Generic report PDF: title + key/value info lines + a table + footer lines.
    // Used for settlement reports and full patient reports.
    bool documentPdf(const std::string &path, const std::string &title,
                     const std::vector<std::vector<std::string>> &info, // each {key, value}
                     const std::vector<std::string> &headers,
                     const std::vector<std::vector<std::string>> &rows,
                     const std::vector<std::string> &footer);
}

#endif // ILMS_EXPORT_H
