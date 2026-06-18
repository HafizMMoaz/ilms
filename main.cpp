// ILMS - Integrated Lab Management System
// Entry point / composition root.
//
// This is the ONLY place that picks a concrete View. To move to a GUI later,
// swap `ConsoleView` for a `GuiView` here - nothing else changes.

#include "ConsoleView.h"
#include "App.h"

int main()
{
    ConsoleView view; // the presentation layer (V)
    App app(view);    // the controller (C); it owns the model (M) internally
    app.run();
    return 0;
}
