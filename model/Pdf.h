#ifndef ILMS_PDF_H
#define ILMS_PDF_H

#include <string>
#include <vector>

// PDF generator backed by libHaru (vendored under third_party/libharu).
// Renders a branded heading (with an optional logo from assets/logo.jpg),
// text / key-value lines, and bordered tables, paginating automatically.
//
// libHaru handles are kept as void* so this header stays free of <hpdf.h>.
class Pdf
{
    void *doc_ = nullptr;  // HPDF_Doc
    void *page_ = nullptr; // HPDF_Page
    void *font_ = nullptr; // Helvetica
    void *bold_ = nullptr; // Helvetica-Bold
    void *mono_ = nullptr; // Courier
    double y_ = 0;         // current baseline cursor (top-down)
    bool ok_ = false;

    void newPage();
    void ensure(double needed);
    void text(double x, double y, void *font, double size, const std::string &s);

public:
    Pdf();
    ~Pdf();
    Pdf(const Pdf &) = delete;
    Pdf &operator=(const Pdf &) = delete;

    bool valid() const { return ok_; }

    void heading(const std::string &title); // optional logo + big title + rule
    void line(const std::string &text);
    void keyVal(const std::string &key, const std::string &value);
    void blank();
    void table(const std::vector<std::string> &headers,
               const std::vector<std::vector<std::string>> &rows);
    bool save(const std::string &path);

    // Called by the libHaru error handler; not for general use.
    void markError() { ok_ = false; }
};

#endif // ILMS_PDF_H
