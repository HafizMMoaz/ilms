#ifndef ILMS_MODELS_H
#define ILMS_MODELS_H

#include <string>
#include <vector>
#include "Utils.h"

// ---------------------------------------------------------------------------
// Data models.  Every record type that used to live in a bunch of parallel
// global arrays is now a small class.  Each one knows how to (de)serialise
// itself to a single CSV line, which is all the file persistence layer needs.
// Public fields are used deliberately: these are plain data records and the
// UI/Repository layers read & write them directly.
//
// Every persisted record carries `createdAt` / `updatedAt` audit stamps; these
// are appended as the last two CSV columns. The Repository fills them in
// automatically (see Repository::add / Repository::update), so older data files
// that lack the columns simply load with blank stamps.
// ---------------------------------------------------------------------------

struct Specimen
{
    std::string id, name, description, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + description + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        description = Utils::field(r, 2);
        createdAt = Utils::field(r, 3);
        updatedAt = Utils::field(r, 4);
    }
};

struct LabDepartment
{
    std::string id, name, date, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + date + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        date = Utils::field(r, 2);
        createdAt = Utils::field(r, 3);
        updatedAt = Utils::field(r, 4);
    }
};

struct LabTest
{
    std::string id, name, rate, group, machine, unit, freq, time, comments, specimen;
    std::string createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + rate + "," + group + "," + machine + "," +
               unit + "," + freq + "," + time + "," + comments + "," + specimen + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        rate = Utils::field(r, 2);
        group = Utils::field(r, 3);
        machine = Utils::field(r, 4);
        unit = Utils::field(r, 5);
        freq = Utils::field(r, 6);
        time = Utils::field(r, 7);
        comments = Utils::field(r, 8);
        specimen = Utils::field(r, 9);
        createdAt = Utils::field(r, 10);
        updatedAt = Utils::field(r, 11);
    }
};

struct Machine
{
    std::string id, name, description, quantity, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + description + "," + quantity + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        description = Utils::field(r, 2);
        quantity = Utils::field(r, 3);
        createdAt = Utils::field(r, 4);
        updatedAt = Utils::field(r, 5);
    }
};

// A standard operating procedure followed when collecting a specimen.
// A specimen can have several SOPs (a checklist); `specimen` holds the
// specimen NAME it applies to (matching how LabTest.specimen is stored).
struct Sop
{
    std::string id, specimen, sop, date, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + specimen + "," + sop + "," + date + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        specimen = Utils::field(r, 1);
        sop = Utils::field(r, 2);
        date = Utils::field(r, 3);
        createdAt = Utils::field(r, 4);
        updatedAt = Utils::field(r, 5);
    }
};

struct Package
{
    std::string id, name, testCount, rate, disc, createdAt, updatedAt;
    std::vector<std::string> tests; // up to 5 lab-test IDs

    Package() : tests(5) {}

    std::string toCSV() const
    {
        std::string row = id + "," + name + "," + testCount + "," + rate + "," + disc;
        for (const auto &t : tests)
            row += "," + t;
        row += "," + createdAt + "," + updatedAt;
        return row;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        testCount = Utils::field(r, 2);
        rate = Utils::field(r, 3);
        disc = Utils::field(r, 4);
        tests.assign(5, "");
        for (int i = 0; i < 5; i++)
            tests[i] = Utils::field(r, 5 + i);
        createdAt = Utils::field(r, 10);
        updatedAt = Utils::field(r, 11);
    }
};

struct Query
{
    std::string id, name, contact, details, isSolved, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + contact + "," + details + "," + isSolved + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        contact = Utils::field(r, 2);
        details = Utils::field(r, 3);
        isSolved = Utils::field(r, 4);
        createdAt = Utils::field(r, 5);
        updatedAt = Utils::field(r, 6);
    }
};

struct Company
{
    std::string id, name, type, discount, commision, commisionPerc, patients, contact;
    std::string createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + type + "," + discount + "," + commision + "," +
               commisionPerc + "," + patients + "," + contact + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        type = Utils::field(r, 2);
        discount = Utils::field(r, 3);
        commision = Utils::field(r, 4);
        commisionPerc = Utils::field(r, 5);
        patients = Utils::field(r, 6);
        contact = Utils::field(r, 7);
        createdAt = Utils::field(r, 8);
        updatedAt = Utils::field(r, 9);
    }
};

struct Patient
{
    std::string id, name, cnic, contact, gender, age, bloodGroup, regDate, address;
    std::string sampleLocation, reference, testCount, price, discount, receivedAmount, balance, status;
    std::string createdAt, updatedAt;
    std::vector<std::string> tests;        // selected lab-test IDs (max 20)
    std::vector<std::string> testStatus;   // per-test result status (max 20)
    std::vector<std::string> specimenTaken; // per-test "Y"/"N" specimen collected (max 20)
    std::vector<std::string> sampleCode;   // per-test sample code (max 20)

    Patient() : tests(20), testStatus(20), specimenTaken(20, "N"), sampleCode(20) {}

    // Number of tests actually selected for this patient.
    int testTotal() const
    {
        int n = 0;
        try { n = std::stoi(testCount); } catch (...) { n = 0; }
        if (n < 0) n = 0;
        if (n > 20) n = 20;
        return n;
    }

    // The test list / status are stored as ';'-joined sub-fields so they fit in
    // one CSV column each (no extra commas).
    std::string toCSV() const
    {
        int n = testTotal();
        std::vector<std::string> t(tests.begin(), tests.begin() + n);
        std::vector<std::string> s(testStatus.begin(), testStatus.begin() + n);
        std::vector<std::string> sp(specimenTaken.begin(), specimenTaken.begin() + n);
        return id + "," + name + "," + cnic + "," + contact + "," + gender + "," +
               age + "," + bloodGroup + "," + regDate + "," + address + "," +
               sampleLocation + "," + reference + "," + testCount + "," + price + "," +
               discount + "," + receivedAmount + "," + balance + "," + status + "," +
               Utils::join(t, ';') + "," + Utils::join(s, ';') + "," + Utils::join(sp, ';') + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        cnic = Utils::field(r, 2);
        contact = Utils::field(r, 3);
        gender = Utils::field(r, 4);
        age = Utils::field(r, 5);
        bloodGroup = Utils::field(r, 6);
        regDate = Utils::field(r, 7);
        address = Utils::field(r, 8);
        sampleLocation = Utils::field(r, 9);
        reference = Utils::field(r, 10);
        testCount = Utils::field(r, 11);
        price = Utils::field(r, 12);
        discount = Utils::field(r, 13);
        receivedAmount = Utils::field(r, 14);
        balance = Utils::field(r, 15);
        status = Utils::field(r, 16);

        tests.assign(20, "");
        testStatus.assign(20, "");
        specimenTaken.assign(20, "N");
        sampleCode.assign(20, "");
        auto t = Utils::split(Utils::field(r, 17), ';');
        auto s = Utils::split(Utils::field(r, 18), ';');
        auto sp = Utils::split(Utils::field(r, 19), ';');
        for (size_t i = 0; i < t.size() && i < 20; i++)
            tests[i] = t[i];
        for (size_t i = 0; i < s.size() && i < 20; i++)
            testStatus[i] = s[i];
        for (size_t i = 0; i < sp.size() && i < 20; i++)
            specimenTaken[i] = sp[i];
        createdAt = Utils::field(r, 20);
        updatedAt = Utils::field(r, 21);
    }
};

struct User
{
    std::string id, fname, uname, email, password, phone, address, role, location;
    std::string createdAt, updatedAt;
    bool active = true;

    User() {}
    User(std::string id_, std::string fname_, std::string uname_,
         std::string password_, std::string role_, bool active_)
        : id(id_), fname(fname_), uname(uname_), password(password_),
          role(role_), active(active_) {}
};

#endif // ILMS_MODELS_H
