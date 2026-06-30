# ILMS — Architecture (MVC)

The program is organised using the **Model–View–Controller** pattern so the UI
can be replaced (console today, GUI later) without rewriting the logic.

```
main.cpp                 Composition root: picks a View, starts the App.
│
├── model/   (M)  Data + rules. Knows nothing about screens.
│   ├── Utils.h/.cpp        CSV field parsing + ID generation
│   ├── Models.h            Specimen, LabTest, Patient, Company, ... (plain records)
│   ├── Repository.h        Repository<T>: vector + load/store/CRUD/nextId (one template for all)
│   ├── Session.h/.cpp      Current logged-in user
│   ├── Validator.h/.cpp    Pure input checks
│   ├── Backup.h/.cpp       Copy data files to/from a Backup folder
│   ├── Logger.h/.cpp       Append timestamped activity to logs.txt
│   ├── Export.h/.cpp       CSV/HTML table export + invoice/receipt/report docs
│   ├── Pdf.h/.cpp          PDF writer backed by libHaru (logos, bordered tables)
│   └── Database.h/.cpp     Owns one Repository per record type + login lookup
│
vendor/libharu/            Vendored libHaru 2.3.0 (no zlib/libpng); built to libhpdf.a
assets/logo.jpg            Optional logo embedded into PDF documents
│
├── view/    (V)  Everything you can see / press.
│   ├── View.h             Abstract interface the controller talks to (the contract)
│   ├── ConsoleView.h/.cpp Console implementation of View (menus, tables, forms)
│   └── Console.h/.cpp      Low-level Windows console helpers (gotoxy, colour, ...)
│
└── controller/  (C)  Application flow.
    └── App.h/.cpp         Owns the model + a View&, drives splash→login→menus→CRUD
```

## Why this shape

* **The controller depends on `View` (the interface), not `ConsoleView`.**
  To switch to a GUI, write `GuiView : public View`, then change one line in
  `main.cpp`. The controller and model don't change at all.
* **Everything crossing the View boundary is plain `string`/`int`.**
  The view has no `#include` of any model class, so the two layers are fully
  decoupled.
* **One `Repository<T>` replaces ~12 parallel arrays + load/store functions**
  from the original single-file version.

## Build & run

```bat
build.bat            REM produces ilms.exe   (or use the g++ line below)
ilms.exe
```

```
g++ -std=c++17 -Imodel -Iview -Icontroller main.cpp ^
    model\Utils.cpp model\Session.cpp model\Validator.cpp model\Database.cpp ^
    view\Console.cpp view\ConsoleView.cpp controller\App.cpp -o ilms.exe
```

Login with one of the seeded users (see `model/Database.cpp`), e.g.
`hafiz` / `1234`.

## Adding the next entity (e.g. Machines) — copy the Specimen pattern

1. **Model** — already exists in `Models.h` (`Machine`). It just needs
   `id`, public fields, and `toCSV()/fromCSV()`.
2. **Database** — already has `Repository<Machine> machines{...}` and loads it
   in `loadAll()`.
3. **Controller** — copy `specimenModule()/addSpecimen()/editSpecimen()/
   deleteSpecimen()` in `App.cpp`, swap `specimens` → `machines` and the field
   list. Wire it into `setupModule()` (replace the "coming soon" branch).

No new View code is required — `table()`, `form()`, `ask()`, `menu()` are
generic and already handle any entity.

## Interactive tables

Entity lists are rendered by `View::entityTable(...)`. Each data row carries its
own **[ Edit ] [ Delete ]** buttons and the top has a **[ Add New ] [ Back ]**
toolbar:

* **Up / Down** — move between the toolbar and the rows
* **Left / Right** — move between the buttons on the focused row
* **Enter** — activate
* **n / p** (or PageDown / PageUp) — next / previous page (8 rows per page)
* **s** — search/filter (matches any cell, case-insensitive); **c** — clear

The controller gets back a `RowAction {type, index}` and dispatches it, so
Edit/Delete act on the *selected row* (no ID typing). Search + pagination live
entirely inside the view: it maps the on-screen row back to the original record
index, so the controller is unchanged. A GUI view would render the same thing as
a real table with a search box and pager.

## What is implemented now

* Splash → login → **dashboard** (summary panels + navigation menu, live counts).
* **SETUP**, using the interactive table UI:
  * **Specimen** — Add / Edit / Delete
  * **Lab Departments** — Add / Edit / Delete
  * **Lab Tests** — name, rate, unit, frequency, delivery time, comments, plus
    a chosen specimen + lab department + machine; Add / Edit / Delete. (The
    specimen choice is what links a test to its SOP checklist.)
  * **Machines** — Add / Edit / Delete
  * **Packages** — bundle 2–5 lab tests at a discount; Add / Edit / Delete
  * **SOPs** — each SOP belongs to a **specimen** (the checklist for collecting
    it; a specimen can have many). Add picks the specimen first; persists to
    `DataBase/sops.txt`.
* **PATIENT** — a small **relational** model instead of everything inline:
  * `Patient` (demographics) — `patient.txt`
  * `Invoice` (one per visit: sample location, reference, discount, gross/net) — `invoice.txt`
  * `PatientTest` (one row per ordered test: specimen, rate, collected Y/N,
    status PEND/DONE, result) — `patienttest.txt`
  * `Payment` (money against an invoice) — `payment.txt`; **balance = invoice net
    − Σ payments**
  * **Patient Records** (interactive table, shows outstanding balance):
    **[Add New]** registers a patient + first visit; **[Edit]** = manage
    (New Visit → another invoice + tests + opening payment; or Invoices &
    Payments → per-invoice tests/results + **Add Payment**); **[Delete]**
    cascades to the patient's invoices/tests/payments.
  * **Sample Receiving** — collect specimens for any pending test across all
    patients; shows that specimen's **SOP checklist** (`sop.specimen ==
    PatientTest.specimen`) and requires confirmation.
  * **Result Entry** — for tests whose specimen is collected, record the result
    and mark the test `DONE`.
  * **Patient Summary** — aggregates (patients, visits, tests, specimens
    collected/pending, results entered, billed/received/outstanding).
* **SETUP → Corporate** — referring companies/doctors: discount %, a **share**
  (% of patient-paid or fixed per test), a **coupon code** (extra % for referred
  patients) and a **settlement period**. Registration offers None / Reference /
  Coupon; a referred invoice accrues the company's share into a ledger.
* **Settlements** (main menu, Admin) — nets each company's unsettled ledger
  entries (`balance < 0` = lab pays company; `> 0` = company pays lab; the
  direction depends on who collected the money) and clears them per period.
* **SETUP → Test Rate List** — read-only price list of all lab tests.
* **BACKUP / RESTORE** (main menu): *Create Backup* copies every data file into
  a new timestamped folder `Backup/backup_YYYY-MM-DD_HH-MM-SS/`; *Restore Backup*
  lists the saved backups (newest first), lets you pick one, then copies it back
  and reloads. Logic lives in `model/Backup.h/.cpp` (`create`/`list`/`restore`);
  `Database::dataFiles()` lists the files to copy.
* **Reports** (main menu): Financial Summary, Test & Specimen Status, and
  Referrals — each a read-only table that can be exported.
* **Export** — every interactive table (and each report) can be exported to
  **CSV / HTML / PDF** (`Exports/<name>_<date>.<ext>`). CSV is properly quoted,
  HTML is a styled table, PDF is a real `.pdf`. Logic in `model/Export.h/.cpp`.
* **Invoice, receipt & lab-report documents** — ordering tests generates a
  printable **invoice** (`Exports/invoice_<id>.pdf`) and a **payment receipt**;
  more receipts on every later payment; and a **lab report** (tests + results)
  from *Invoices & Payments → Print Lab Report*. All are real PDFs rendered by
  `model/Pdf`, which is backed by **libHaru** (vendored in `vendor/libharu/`,
  built without zlib/libpng) — so documents have **bordered tables** and an
  optional **logo** (`assets/logo.jpg`). They open via `Console::openFile`.

## Roles, Users & Areas

* **Roles** (main menu, **Super Admin** only) — a persisted, dynamic role list
  seeded with the ten built-in roles; built-ins are locked, custom roles can be
  added/edited/deleted (`DataBase/role.txt`).
* **Users** (main menu, **Admin/Super Admin**) — create/manage staff accounts,
  assign any dynamic role, set password + active. **Home Sampling** users get an
  assigned **area** (+ optional lat/lng); **Companies & Doctors** users link to a
  Company. Persisted to `DataBase/user.txt` (git-ignored — it holds passwords;
  the three default logins are re-seeded if the file is absent).
* **Sampling Areas** (Setup) — named areas (Nabipura, Mughalpura, …) with
  optional coordinates, used to route home-sampling collections.

### Building with the vendored libHaru

`build.bat` compiles `vendor/libharu/src/*.c` once into `libhpdf.a` (cached;
delete it to rebuild) and links the app against it. CI does the same. Nothing
needs to be installed — the library source ships in the repo.

## Audit trail (timestamps + activity log)

* **Every record carries `createdAt` / `updatedAt`** (last two CSV columns).
  `Repository::add` stamps both on insert; `Repository::update(i)` bumps
  `updatedAt` and saves. Older data files without the columns load with blank
  stamps and gain them on the next edit.
* **`Database/logs.txt`** records the logged-in user's activity — logins (and
  failed attempts), screen opens, and every add / update / delete — one
  timestamped line each, via `Logger` + the controller's `logAction()` helper.
* **Admins only** (role `R008`/`R009`) see an **Activity Logs** entry on the main
  menu: a read-only, searchable, paged viewer (`View::logViewer`). It cannot
  edit lines; it can **Delete All**, and the very next log line records who
  cleared it (`<user>: cleared all activity logs`).

## Notes / preserved behaviour

* The original single-file program is kept verbatim at
  `legacy/ilms_original.cpp` for reference.
* Data files in `DataBase/` are unchanged and load correctly.
* `tests/test_model.cpp` exercises the model layer (run it to verify CSV
  parsing, `nextId`, and CRUD).
