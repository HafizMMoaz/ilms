#ifndef ILMS_PDF_H
#define ILMS_PDF_H

#include <string>
#include <vector>

// A minimal, dependency-free PDF generator. It lays out monospaced text lines
// (the standard Courier font, which needs no embedding) across one or more
// US-Letter pages and writes a valid PDF with a correct cross-reference table.
class Pdf
{
    std::vector<std::string> lines_;

public:
    void line(const std::string &text) { lines_.push_back(text); }
    void blank() { lines_.push_back(""); }

    // Appends a monospaced table: header row, a dashed separator, then the data
    // rows, with columns auto-sized to their widest cell.
    void table(const std::vector<std::string> &headers,
               const std::vector<std::vector<std::string>> &rows);

    // Writes the PDF to `path`. Returns false on failure.
    bool save(const std::string &path) const;
};

#endif // ILMS_PDF_H
