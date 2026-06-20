#include "Pdf.h"
#include "hpdf.h"

#include <fstream>
#include <algorithm>
#include <direct.h> // _mkdir (MinGW)

namespace
{
    void ensureParentDir(const std::string &path)
    {
        std::size_t s = path.find_last_of("/\\");
        if (s != std::string::npos)
            _mkdir(path.substr(0, s).c_str());
    }

    // libHaru is a Latin-only library here; keep printable ASCII.
    std::string ascii(const std::string &s)
    {
        std::string o;
        for (unsigned char c : s)
            o += (c >= 32 && c <= 126) ? (char)c : ' ';
        return o;
    }

    // On error libHaru calls this; we just flag the document as failed and
    // return (further API calls then become safe no-ops).
    void errorHandler(HPDF_STATUS, HPDF_STATUS, void *user)
    {
        if (user)
            static_cast<Pdf *>(user)->markError();
    }

    const double MARGIN = 40.0;
}

Pdf::Pdf()
{
    HPDF_Doc doc = HPDF_New(errorHandler, this);
    if (!doc)
        return;
    doc_ = doc;
    HPDF_SetCompressionMode(doc, HPDF_COMP_NONE);
    font_ = HPDF_GetFont(doc, "Helvetica", NULL);
    bold_ = HPDF_GetFont(doc, "Helvetica-Bold", NULL);
    mono_ = HPDF_GetFont(doc, "Courier", NULL);
    ok_ = true;
    newPage();
}

Pdf::~Pdf()
{
    if (doc_)
        HPDF_Free((HPDF_Doc)doc_);
}

void Pdf::newPage()
{
    HPDF_Page page = HPDF_AddPage((HPDF_Doc)doc_);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    page_ = page;
    y_ = HPDF_Page_GetHeight(page) - MARGIN;
}

void Pdf::ensure(double needed)
{
    if (y_ - needed < MARGIN)
        newPage();
}

void Pdf::text(double x, double y, void *font, double size, const std::string &s)
{
    HPDF_Page p = (HPDF_Page)page_;
    HPDF_Page_SetFontAndSize(p, (HPDF_Font)font, size);
    HPDF_Page_BeginText(p);
    HPDF_Page_TextOut(p, x, y, ascii(s).c_str());
    HPDF_Page_EndText(p);
}

void Pdf::heading(const std::string &title)
{
    double left = MARGIN;

    // Optional logo (JPEG) from assets/logo.jpg.
    std::ifstream lf("assets/logo.jpg", std::ios::binary);
    if (lf.good())
    {
        lf.close();
        HPDF_Image img = HPDF_LoadJpegImageFromFile((HPDF_Doc)doc_, "assets/logo.jpg");
        if (img && ok_)
        {
            const double w = 90, h = 45;
            HPDF_Page_DrawImage((HPDF_Page)page_, img, left, y_ - h + 12, w, h);
            left += w + 12;
        }
    }

    text(left, y_, bold_, 18, title);
    y_ -= 26;

    HPDF_Page p = (HPDF_Page)page_;
    HPDF_Page_SetLineWidth(p, 1.0);
    HPDF_Page_MoveTo(p, MARGIN, y_ + 6);
    HPDF_Page_LineTo(p, HPDF_Page_GetWidth(p) - MARGIN, y_ + 6);
    HPDF_Page_Stroke(p);
    y_ -= 8;
}

void Pdf::line(const std::string &t)
{
    ensure(16);
    text(MARGIN, y_, font_, 11, t);
    y_ -= 16;
}

void Pdf::keyVal(const std::string &k, const std::string &v)
{
    ensure(15);
    text(MARGIN, y_, bold_, 10, k);
    text(MARGIN + 110, y_, font_, 10, v);
    y_ -= 15;
}

void Pdf::blank()
{
    y_ -= 8;
}

void Pdf::table(const std::vector<std::string> &headers,
                const std::vector<std::vector<std::string>> &rows)
{
    int cols = (int)headers.size();
    if (cols == 0)
        return;

    std::vector<std::size_t> w(cols, 0);
    for (int c = 0; c < cols; c++)
        w[c] = headers[c].size();
    for (const auto &r : rows)
        for (int c = 0; c < cols && c < (int)r.size(); c++)
            w[c] = std::max(w[c], r[c].size());

    const double charW = 5.2, pad = 6, rowH = 15, fs = 9;
    std::vector<double> cw(cols);
    double total = 0;
    for (int c = 0; c < cols; c++)
    {
        cw[c] = w[c] * charW + 2 * pad;
        total += cw[c];
    }
    double maxW = HPDF_Page_GetWidth((HPDF_Page)page_) - 2 * MARGIN;
    if (total > maxW)
    {
        double k = maxW / total;
        for (int c = 0; c < cols; c++)
            cw[c] *= k;
        total = maxW;
    }

    auto drawRow = [&](const std::vector<std::string> &cells, bool header)
    {
        ensure(rowH + 2);
        HPDF_Page p = (HPDF_Page)page_;
        double top = y_, bottom = y_ - rowH;

        if (header)
        {
            HPDF_Page_SetRGBFill(p, 0.88, 0.95, 0.91);
            HPDF_Page_Rectangle(p, MARGIN, bottom, total, rowH);
            HPDF_Page_Fill(p);
        }

        double x = MARGIN;
        for (int c = 0; c < cols; c++)
        {
            std::string cell = (c < (int)cells.size()) ? cells[c] : "";
            std::size_t maxch = (std::size_t)((cw[c] - 2 * pad) / charW);
            if (maxch > 1 && cell.size() > maxch)
                cell = cell.substr(0, maxch);
            text(x + pad, bottom + 4, mono_, fs, cell);
            x += cw[c];
        }

        HPDF_Page_SetLineWidth(p, 0.5);
        HPDF_Page_SetRGBStroke(p, 0.5, 0.5, 0.5);
        HPDF_Page_Rectangle(p, MARGIN, bottom, total, rowH);
        HPDF_Page_Stroke(p);
        x = MARGIN;
        for (int c = 0; c < cols - 1; c++)
        {
            x += cw[c];
            HPDF_Page_MoveTo(p, x, bottom);
            HPDF_Page_LineTo(p, x, top);
            HPDF_Page_Stroke(p);
        }
        HPDF_Page_SetRGBStroke(p, 0, 0, 0);
        y_ -= rowH;
    };

    drawRow(headers, true);
    for (const auto &r : rows)
        drawRow(r, false);
    y_ -= 4;
}

bool Pdf::save(const std::string &path)
{
    if (!ok_ || !doc_)
        return false;
    ensureParentDir(path);
    HPDF_STATUS st = HPDF_SaveToFile((HPDF_Doc)doc_, path.c_str());
    return st == HPDF_OK && ok_;
}
