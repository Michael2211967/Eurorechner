#include "print.h"
#include "common.h" // NEU: Header-Datei für die Strukturdeklaration

// Globale Zeiger auf die Widgets
static AppWidgets *widgets_to_print;
static PangoLayout *layout;

// Callback-Funktion, die aufgerufen wird, bevor das Drucken beginnt
static void begin_print(GtkPrintOperation *operation, GtkPrintContext *context) {
    gtk_print_operation_set_n_pages(operation, 1);
    layout = gtk_print_context_create_pango_layout(context);
}

// Callback-Funktion, die aufgerufen wird, um eine Seite zu zeichnen
static void draw_page(GtkPrintOperation *operation, GtkPrintContext *context, gint page_nr) {
    cairo_t *cr = gtk_print_context_get_cairo_context(context);
    
    // Die Werte aus den Labels holen
    const gchar *euro_netto_text = gtk_label_get_text(GTK_LABEL(widgets_to_print->euro_netto_label));
    const gchar *dm_netto_text = gtk_label_get_text(GTK_LABEL(widgets_to_print->dm_netto_label));
    const gchar *euro_mwst_text = gtk_label_get_text(GTK_LABEL(widgets_to_print->euro_mwst_label));
    const gchar *dm_mwst_text = gtk_label_get_text(GTK_LABEL(widgets_to_print->dm_mwst_label));
    const gchar *mwst_rate_text = gtk_entry_get_text(GTK_ENTRY(widgets_to_print->mwst_entry));
    const gchar *euro_brutto_text = gtk_entry_get_text(GTK_ENTRY(widgets_to_print->euro_entry));
    const gchar *dm_brutto_text = gtk_entry_get_text(GTK_ENTRY(widgets_to_print->dm_entry));

    // Den zu druckenden Text formatieren
    gchar *print_text = g_strdup_printf(
        "Euro-Umrechnungskurs = %.5f DM\n\n"
        "\t\tDM\t\tEuro\n\n"
        "Netto\t\t%.2f\t\t%.2f\n"
        "+ %.1f %% MwSt.\t\t%.2f\t\t%.2f\n"
        "Brutto\t\t%s\t\t%s\n\n",
        1.95583,
        strtod(dm_netto_text, NULL), strtod(euro_netto_text, NULL),
        strtod(mwst_rate_text, NULL),
        strtod(dm_mwst_text, NULL), strtod(euro_mwst_text, NULL),
        dm_brutto_text, euro_brutto_text
    );
    
    // Den Text auf der Seite zeichnen
    pango_layout_set_text(layout, print_text, -1);
    
    cairo_move_to(cr, 50, 50); // Position des Textes
    pango_cairo_show_layout(cr, layout);

    g_free(print_text);
}

// Callback-Funktion, die nach dem Drucken aufgerufen wird
static void end_print(GtkPrintOperation *operation, GtkPrintContext *context) {
    g_object_unref(layout);
}

// Die eigentliche Callback-Funktion für den Drucken-Knopf
void on_print_activate(GtkWidget *widget, gpointer data) {
    widgets_to_print = (AppWidgets *)data;
    
    GtkPrintOperation *print_op = gtk_print_operation_new();
    
    g_signal_connect(print_op, "begin-print", G_CALLBACK(begin_print), NULL);
    g_signal_connect(print_op, "draw-page", G_CALLBACK(draw_page), NULL);
    g_signal_connect(print_op, "end-print", G_CALLBACK(end_print), NULL);

    GError *error = NULL;
    GtkPrintOperationResult result = gtk_print_operation_run(
        print_op,
        GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        &error
    );

    if (result == GTK_PRINT_OPERATION_RESULT_ERROR) {
        g_warning("Drucken fehlgeschlagen: %s", error->message);
        g_error_free(error);
    }

    g_object_unref(print_op);
}