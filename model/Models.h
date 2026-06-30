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

// A referring company or doctor. Patients referred by it get its `discount`
// (plus a coupon's extra %). The company earns a `share` of each referred bill
// - either a percentage of the patient-paid amount or a fixed amount per test -
// settled every `settlementPeriod`. (Share/coupon/settlement fields are appended
// so older company rows still load.)
struct Company
{
    std::string id, name, type, discount, commision, commisionPerc, patients, contact;
    std::string createdAt, updatedAt;
    std::string shareType, shareValue;       // shareType = "PCT" | "FIXED"
    std::string couponCode, couponPct;       // coupon -> extra % discount
    std::string settlementPeriod;            // daily | weekly | monthly | yearly

    std::string toCSV() const
    {
        return id + "," + name + "," + type + "," + discount + "," + commision + "," +
               commisionPerc + "," + patients + "," + contact + "," +
               createdAt + "," + updatedAt + "," + shareType + "," + shareValue + "," +
               couponCode + "," + couponPct + "," + settlementPeriod;
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
        shareType = Utils::field(r, 10);
        shareValue = Utils::field(r, 11);
        couponCode = Utils::field(r, 12);
        couponPct = Utils::field(r, 13);
        settlementPeriod = Utils::field(r, 14);
    }
};

// One ledger entry per corporate-referred invoice, from the lab's perspective:
// labBalance < 0  -> the lab owes the company (lab collected, owes the share);
// labBalance > 0  -> the company owes the lab (the doctor collected the money).
struct Settlement
{
    std::string id, companyId, invoiceId, amount, labBalance, direction;
    std::string date, settled, createdAt, updatedAt; // settled = "Y" | "N"

    std::string toCSV() const
    {
        return id + "," + companyId + "," + invoiceId + "," + amount + "," + labBalance + "," +
               direction + "," + date + "," + settled + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        companyId = Utils::field(r, 1);
        invoiceId = Utils::field(r, 2);
        amount = Utils::field(r, 3);
        labBalance = Utils::field(r, 4);
        direction = Utils::field(r, 5);
        date = Utils::field(r, 6);
        settled = Utils::field(r, 7);
        createdAt = Utils::field(r, 8);
        updatedAt = Utils::field(r, 9);
    }
};

// A patient is now just demographics. What they ordered, what it costs and what
// they paid lives in the related Invoice / PatientTest / Payment records, keyed
// by id (a small relational model instead of everything inline).
struct Patient
{
    std::string id, name, cnic, contact, gender, age, bloodGroup, regDate, address;
    std::string createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + cnic + "," + contact + "," + gender + "," +
               age + "," + bloodGroup + "," + regDate + "," + address + "," +
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
        createdAt = Utils::field(r, 9);
        updatedAt = Utils::field(r, 10);
    }
};

// One bill per visit. Generated whenever a patient orders a set of tests.
// For a Home sample, `area` + `homeSampler` (a Home-Sampling user id) record the
// collection assignment. (area/homeSampler are appended so older rows still load.)
struct Invoice
{
    std::string id, patientId, date, sampleLocation, reference, discount;
    std::string grossTotal, netTotal, createdAt, updatedAt;
    std::string area, homeSampler;

    std::string toCSV() const
    {
        return id + "," + patientId + "," + date + "," + sampleLocation + "," +
               reference + "," + discount + "," + grossTotal + "," + netTotal + "," +
               createdAt + "," + updatedAt + "," + area + "," + homeSampler;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        patientId = Utils::field(r, 1);
        date = Utils::field(r, 2);
        sampleLocation = Utils::field(r, 3);
        reference = Utils::field(r, 4);
        discount = Utils::field(r, 5);
        grossTotal = Utils::field(r, 6);
        netTotal = Utils::field(r, 7);
        createdAt = Utils::field(r, 8);
        updatedAt = Utils::field(r, 9);
        area = Utils::field(r, 10);
        homeSampler = Utils::field(r, 11);
    }
};

// One row per ordered test, belonging to an invoice (and patient). Tracks
// whether its specimen was collected and the result once entered.
struct PatientTest
{
    std::string id, invoiceId, patientId, testId, testName, specimen, rate;
    std::string specimenTaken, status, result, createdAt, updatedAt; // taken Y/N, status PEND/DONE

    std::string toCSV() const
    {
        return id + "," + invoiceId + "," + patientId + "," + testId + "," + testName + "," +
               specimen + "," + rate + "," + specimenTaken + "," + status + "," + result + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        invoiceId = Utils::field(r, 1);
        patientId = Utils::field(r, 2);
        testId = Utils::field(r, 3);
        testName = Utils::field(r, 4);
        specimen = Utils::field(r, 5);
        rate = Utils::field(r, 6);
        specimenTaken = Utils::field(r, 7);
        status = Utils::field(r, 8);
        result = Utils::field(r, 9);
        createdAt = Utils::field(r, 10);
        updatedAt = Utils::field(r, 11);
    }
};

// A payment made against an invoice. Many payments per invoice => balance =
// invoice.netTotal - sum(payments).
struct Payment
{
    std::string id, invoiceId, amount, date, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + invoiceId + "," + amount + "," + date + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        invoiceId = Utils::field(r, 1);
        amount = Utils::field(r, 2);
        date = Utils::field(r, 3);
        createdAt = Utils::field(r, 4);
        updatedAt = Utils::field(r, 5);
    }
};

struct User
{
    std::string id, fname, uname, email, password, phone, address, role;
    std::string area, lat, lng;  // for Home Sampling users (area name + optional coords)
    std::string companyId;       // for Companies & Doctors users (links a Company)
    std::string createdAt, updatedAt;
    bool active = true;

    User() {}
    User(std::string id_, std::string fname_, std::string uname_,
         std::string password_, std::string role_, bool active_)
        : id(id_), fname(fname_), uname(uname_), password(password_),
          role(role_), active(active_) {}

    std::string toCSV() const
    {
        return id + "," + fname + "," + uname + "," + email + "," + password + "," +
               phone + "," + address + "," + role + "," + area + "," + lat + "," +
               lng + "," + companyId + "," + (active ? "1" : "0") + "," +
               createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        fname = Utils::field(r, 1);
        uname = Utils::field(r, 2);
        email = Utils::field(r, 3);
        password = Utils::field(r, 4);
        phone = Utils::field(r, 5);
        address = Utils::field(r, 6);
        role = Utils::field(r, 7);
        area = Utils::field(r, 8);
        lat = Utils::field(r, 9);
        lng = Utils::field(r, 10);
        companyId = Utils::field(r, 11);
        active = (Utils::field(r, 12) != "0");
        createdAt = Utils::field(r, 13);
        updatedAt = Utils::field(r, 14);
    }
};

// A home-sampling service area: a named area with optional coordinates.
struct Area
{
    std::string id, name, lat, lng, createdAt, updatedAt;

    std::string toCSV() const
    {
        return id + "," + name + "," + lat + "," + lng + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        lat = Utils::field(r, 2);
        lng = Utils::field(r, 3);
        createdAt = Utils::field(r, 4);
        updatedAt = Utils::field(r, 5);
    }
};

// A user role. The built-in ones (fixed == "Y") cannot be edited or deleted;
// a Super Admin can add more custom roles in the Roles setup screen.
struct Role
{
    std::string id, name, fixed, createdAt, updatedAt; // fixed = "Y" / "N"

    std::string toCSV() const
    {
        return id + "," + name + "," + fixed + "," + createdAt + "," + updatedAt;
    }
    void fromCSV(const std::string &r)
    {
        id = Utils::field(r, 0);
        name = Utils::field(r, 1);
        fixed = Utils::field(r, 2);
        createdAt = Utils::field(r, 3);
        updatedAt = Utils::field(r, 4);
    }
};

#endif // ILMS_MODELS_H
