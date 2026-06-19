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
        std::vector<std::vector<std::string>> tests; // each: {name, specimen, rate}
    };
    bool invoice(const std::string &path, const InvoiceDoc &d);

    struct ReceiptDoc
    {
        std::string id, date, patientName, invoiceId, amount, paidTotal, balance;
    };
    bool receipt(const std::string &path, const ReceiptDoc &d);
}

#endif // ILMS_EXPORT_H
