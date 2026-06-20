#include "Pdf.h"

#include <fstream>
#include <algorithm>
#include <cstdio>
#include <direct.h> // _mkdir (MinGW)

namespace
{
    void ensureParentDir(const std::string &path)
    {
        std::size_t s = path.find_last_of("/\\");
        if (s != std::string::npos)
            _mkdir(path.substr(0, s).c_str());
    }

    // Escape characters special inside a PDF string literal; drop anything that
    // isn't printable ASCII (Courier is a Latin font).
    std::string escapePdf(const std::string &s)
    {
        std::string o;
        for (unsigned char c : s)
        {
            if (c == '\\' || c == '(' || c == ')')
            {
                o += '\\';
                o += (char)c;
            }
            else if (c < 32 || c > 126)
                o += ' ';
            else
                o += (char)c;
        }
        return o;
    }
}

void Pdf::table(const std::vector<std::string> &headers,
                const std::vector<std::vector<std::string>> &rows)
{
    int cols = static_cast<int>(headers.size());
    std::vector<std::size_t> w(cols, 0);
    for (int c = 0; c < cols; c++)
        w[c] = headers[c].size();
    for (const auto &r : rows)
        for (int c = 0; c < cols && c < (int)r.size(); c++)
            w[c] = std::max(w[c], r[c].size());

    auto fmt = [&](const std::vector<std::string> &r)
    {
        std::string out;
        for (int c = 0; c < cols; c++)
        {
            std::string cell = (c < (int)r.size()) ? r[c] : "";
            cell.resize(w[c], ' ');
            out += (c ? " | " : "") + cell;
        }
        return out;
    };

    line(fmt(headers));
    std::string sep;
    for (int c = 0; c < cols; c++)
    {
        if (c) sep += "-+-";
        sep += std::string(w[c], '-');
    }
    line(sep);
    for (const auto &r : rows)
        line(fmt(r));
}

bool Pdf::save(const std::string &path) const
{
    const int LINES_PER_PAGE = 58;
    const int MAX_CHARS = 95; // keep lines inside the page width

    std::vector<std::string> ls = lines_;
    if (ls.empty())
        ls.push_back("");
    int pageCount = (int)((ls.size() + LINES_PER_PAGE - 1) / LINES_PER_PAGE);
    if (pageCount < 1)
        pageCount = 1;
    int totalObjs = 3 + 2 * pageCount; // catalog, pages, font, then page+content per page

    std::string out = "%PDF-1.4\n";
    std::vector<std::size_t> off(totalObjs + 1, 0);
    auto emit = [&](int num, const std::string &body)
    {
        off[num] = out.size();
        out += std::to_string(num) + " 0 obj\n" + body + "\nendobj\n";
    };

    emit(1, "<< /Type /Catalog /Pages 2 0 R >>");
    std::string kids;
    for (int p = 0; p < pageCount; p++)
        kids += std::to_string(4 + 2 * p) + " 0 R ";
    emit(2, "<< /Type /Pages /Kids [" + kids + "] /Count " + std::to_string(pageCount) + " >>");
    emit(3, "<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>");

    for (int p = 0; p < pageCount; p++)
    {
        int pageObj = 4 + 2 * p, contentObj = 5 + 2 * p;
        std::string stream = "BT\n/F1 10 Tf\n12 TL\n50 760 Td\n";
        int start = p * LINES_PER_PAGE;
        int end = std::min((int)ls.size(), start + LINES_PER_PAGE);
        for (int i = start; i < end; i++)
        {
            std::string s = ls[i];
            if ((int)s.size() > MAX_CHARS)
                s = s.substr(0, MAX_CHARS);
            stream += "(" + escapePdf(s) + ") Tj\nT*\n";
        }
        stream += "ET";

        emit(pageObj, "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] "
                      "/Resources << /Font << /F1 3 0 R >> >> /Contents " +
                          std::to_string(contentObj) + " 0 R >>");
        emit(contentObj, "<< /Length " + std::to_string(stream.size()) +
                             " >>\nstream\n" + stream + "\nendstream");
    }

    std::size_t xref = out.size();
    out += "xref\n0 " + std::to_string(totalObjs + 1) + "\n";
    out += "0000000000 65535 f\r\n";
    for (int n = 1; n <= totalObjs; n++)
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%010lu 00000 n\r\n", (unsigned long)off[n]);
        out += buf;
    }
    out += "trailer\n<< /Size " + std::to_string(totalObjs + 1) +
           " /Root 1 0 R >>\nstartxref\n" + std::to_string(xref) + "\n%%EOF";

    ensureParentDir(path);
    std::ofstream f(path, std::ios::binary);
    if (!f)
        return false;
    f.write(out.data(), (std::streamsize)out.size());
    return true;
}
