# Assets

Drop a **`logo.jpg`** in this folder to brand the generated PDF documents
(invoices, receipts, lab reports). It is loaded automatically when present and
drawn in the top-left of each document's heading.

- Format: **JPEG** (`.jpg`). PNG is not supported (libHaru is built here without
  libpng); convert a PNG logo to JPEG first.
- A landscape logo around **180 x 90 px** looks best (it is scaled to ~90x45 pt).

If `assets/logo.jpg` is absent, documents simply render the text heading.
