#ifndef COMMON_H
#define COMMON_H

#include <gtk/gtk.h>

// Struktur, um die Widgets an die Callback-Funktionen zu übergeben
typedef struct {
    GtkWidget *euro_entry;
    GtkWidget *dm_entry;
    GtkWidget *mwst_entry;
    GtkWidget *euro_netto_label;
    GtkWidget *dm_netto_label;
    GtkWidget *euro_mwst_label;
    GtkWidget *dm_mwst_label;
} AppWidgets;

#endif /* COMMON_H */