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
│   └── Database.h/.cpp     Owns one Repository per record type + login lookup
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
* **SETUP → Corporate** — referring companies/hospitals/doctors (name, type,
  contact, discount %, commission %); Add / Edit / Delete.
* **SETUP → Test Rate List** — read-only price list of all lab tests.
* **BACKUP / RESTORE** (main menu): *Create Backup* copies every data file into
  a new timestamped folder `Backup/backup_YYYY-MM-DD_HH-MM-SS/`; *Restore Backup*
  lists the saved backups (newest first), lets you pick one, then copies it back
  and reloads. Logic lives in `model/Backup.h/.cpp` (`create`/`list`/`restore`);
  `Database::dataFiles()` lists the files to copy.
* **Export** — every interactive table has an **[Export]** toolbar button that
  writes the table to `Exports/<name>_<date>.csv` or `.html` (CSV is properly
  quoted; HTML is a styled table) and offers to open it. Logic in
  `model/Export.h/.cpp`.
* **Invoice & receipt documents** — ordering tests generates a printable
  **invoice** (`Exports/invoice_<id>.html`) and a **payment receipt**
  (`Exports/receipt_<id>.html`); the same are produced on every later payment,
  and an invoice can be re-printed from *Invoices & Payments*. They open in the
  browser (`Console::openFile`), where they print / "Save as PDF" — no extra
  library needed.

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
