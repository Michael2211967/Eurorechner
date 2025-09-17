#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <stdio.h>
#include <stdlib.h>
#include "mehrwert.h"
#include "print.h" // NEU: Header-Datei für die Druckfunktion
#include "common.h" // NEU: Header-Datei für die Strukturdeklaration

#define EURO_DM_RATE 1.95583

// Gemeinsame Funktion zum Aktualisieren aller Werte
static void update_values(AppWidgets *widgets, gdouble changed_value, const char* changed_field) {
    const gchar *mwst_rate_text = gtk_entry_get_text(GTK_ENTRY(widgets->mwst_entry));
    if (g_strcmp0(mwst_rate_text, "") == 0) {
        mwst_rate_text = "0.0";
    }
    gdouble mwst_rate = strtod(mwst_rate_text, NULL);

    gdouble euro_value = 0.0, dm_value = 0.0;

    if (g_strcmp0(changed_field, "euro") == 0) {
        euro_value = changed_value;
        dm_value = euro_value * EURO_DM_RATE;
        gchar *result_text = g_strdup_printf("%.2f", dm_value);
        gtk_entry_set_text(GTK_ENTRY(widgets->dm_entry), result_text);
        g_free(result_text);
    } else if (g_strcmp0(changed_field, "dm") == 0) {
        dm_value = changed_value;
        euro_value = dm_value / EURO_DM_RATE;
        gchar *result_text = g_strdup_printf("%.2f", euro_value);
        gtk_entry_set_text(GTK_ENTRY(widgets->euro_entry), result_text);
        g_free(result_text);
    } else { // MwSt.-Feld wurde geändert
        euro_value = strtod(gtk_entry_get_text(GTK_ENTRY(widgets->euro_entry)), NULL);
        dm_value = euro_value * EURO_DM_RATE;
    }
    
    // Nettowerte neu berechnen
    gdouble euro_netto_value = berechne_netto(euro_value, mwst_rate);
    gdouble dm_netto_value = berechne_netto(dm_value, mwst_rate);
    gdouble euro_mwst_value = euro_value - euro_netto_value;
    gdouble dm_mwst_value = dm_value - dm_netto_value;

    gchar *netto_text = g_strdup_printf("%.2f", euro_netto_value);
    gtk_label_set_text(GTK_LABEL(widgets->euro_netto_label), netto_text);
    g_free(netto_text);

    netto_text = g_strdup_printf("%.2f", dm_netto_value);
    gtk_label_set_text(GTK_LABEL(widgets->dm_netto_label), netto_text);
    g_free(netto_text);

    gchar *mwst_text = g_strdup_printf("%.2f", euro_mwst_value);
    gtk_label_set_text(GTK_LABEL(widgets->euro_mwst_label), mwst_text);
    g_free(mwst_text);

    mwst_text = g_strdup_printf("%.2f", dm_mwst_value);
    gtk_label_set_text(GTK_LABEL(widgets->dm_mwst_label), mwst_text);
    g_free(mwst_text);
}

// Callback-Funktion, wenn die Enter-Taste im DM-Feld gedrückt wird
static void on_dm_activate(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
    gdouble value = strtod(text, NULL);
    update_values((AppWidgets *)data, value, "dm");
}

// Callback-Funktion, wenn die Enter-Taste im Euro-Feld gedrückt wird
static void on_euro_activate(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
    gdouble value = strtod(text, NULL);
    update_values((AppWidgets *)data, value, "euro");
}

// Callback-Funktion, wenn die Enter-Taste im MWST-Feld gedrückt wird
static void on_mwst_activate(GtkWidget *widget, gpointer data) {
    update_values((AppWidgets *)data, 0.0, "mwst");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *euro_label, *dm_label;
    GtkWidget *print_button;
    GtkWidget *mwst_grid; 

    AppWidgets app_widgets;

    // GTK+ initialisieren
    gtk_init(&argc, &argv);
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *css_data = "label, entry, button { font-size: 14pt; font-family: Arial; }";
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);

    g_object_unref(provider);

    // Ein neues Fenster erstellen
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Der Linux-Eurorechner");

    // minimale Fenstergröße setzen
    
    GdkGeometry geometry;
    geometry.min_width = 750;
    geometry.min_height = 200;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL, &geometry, GDK_HINT_MIN_SIZE);
    
    //gtk_window_set_default_size(GTK_WINDOW(window), 750, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Piktogramm von einem festen Pfad laden
    GError *error = NULL;
    #ifdef __linux__
    // icon path: /usr/share/icons/hicolor/48x48/apps
    gchar *icon_path = g_build_filename("/usr", "share", "icons", "hicolor", "48x48", "apps", "eurorechner.png", NULL);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path, &error);
    if (error != NULL) {
        g_warning("Konnte das Icon nicht laden: %s", error->message);
        g_error_free(error);
    }
    g_free(icon_path);
    #endif // __linux__

    // Grid-Container erstellen
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Labels erstellen
    GtkWidget *netto_label = gtk_label_new("Netto:");
    GtkWidget *mwst_rate_label = gtk_label_new("MwSt (%):");
    euro_label = gtk_label_new("Euro:");
    dm_label = gtk_label_new("DM:");

    // Labels linksbündig ausrichten
    gtk_label_set_xalign(GTK_LABEL(netto_label), 0.0);
    gtk_label_set_xalign(GTK_LABEL(mwst_rate_label), 0.0);
    gtk_label_set_xalign(GTK_LABEL(euro_label), 0.0);
    gtk_label_set_xalign(GTK_LABEL(dm_label), 0.0);
    
    // Entry-Widgets erstellen
    app_widgets.euro_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app_widgets.euro_entry), "1.00");
    app_widgets.dm_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app_widgets.dm_entry), "1.96");
    app_widgets.mwst_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app_widgets.mwst_entry), "19.0");
    
    // Die neuen Netto- und Mehrwert-Labels erstellen
    app_widgets.euro_netto_label = gtk_label_new("");
    app_widgets.dm_netto_label = gtk_label_new("");
    app_widgets.euro_mwst_label = gtk_label_new("");
    app_widgets.dm_mwst_label = gtk_label_new("");

    update_values(&app_widgets, 1.0, "euro");
    update_values(&app_widgets, 1.96, "dm");
    
    // Ausrichtung der Eingabefelder auf rechtsbündig setzen
    gtk_entry_set_alignment(GTK_ENTRY(app_widgets.euro_entry), 1.0);
    gtk_entry_set_alignment(GTK_ENTRY(app_widgets.dm_entry), 1.0);
    gtk_entry_set_alignment(GTK_ENTRY(app_widgets.mwst_entry), 1.0);
    
    // Ausrichtung der Mehrwertsteuer-Labels auf rechtsbündig setzen
    gtk_label_set_xalign(GTK_LABEL(app_widgets.euro_netto_label), 1.0);
    gtk_label_set_xalign(GTK_LABEL(app_widgets.dm_netto_label), 1.0);
    gtk_label_set_xalign(GTK_LABEL(app_widgets.dm_mwst_label), 1.0);
    gtk_label_set_xalign(GTK_LABEL(app_widgets.euro_mwst_label), 1.0);

    // Ein neues Grid-Widget für die MwSt-Eingabe erstellen
    mwst_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(mwst_grid), 5);
    gtk_grid_attach(GTK_GRID(mwst_grid), mwst_rate_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(mwst_grid), app_widgets.mwst_entry, 1, 0, 1, 1);

    // Die Breite des MwSt-Eingabefeldes festlegen
    gtk_entry_set_width_chars(GTK_ENTRY(app_widgets.mwst_entry), 5);
    gtk_widget_set_hexpand(app_widgets.mwst_entry, FALSE);
    gtk_widget_set_halign(app_widgets.mwst_entry, GTK_ALIGN_END);
    
    // Neuer Drucken-Knopf
    print_button = gtk_button_new_with_label("Drucken");

    // Signale der Widgets laden
    g_signal_connect(app_widgets.euro_entry, "activate", G_CALLBACK(on_euro_activate), &app_widgets);
    g_signal_connect(app_widgets.dm_entry, "activate", G_CALLBACK(on_dm_activate), &app_widgets);
    g_signal_connect(app_widgets.mwst_entry, "activate", G_CALLBACK(on_mwst_activate), &app_widgets);
    g_signal_connect(print_button, "clicked", G_CALLBACK(on_print_activate), &app_widgets);

    // Widgets zum Haupt-Grid hinzufügen (neues Layout)
    gtk_grid_attach(GTK_GRID(grid), netto_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.euro_netto_label, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.dm_netto_label, 5, 0, 1, 1);
    
    // Das neue MwSt-Grid in das Hauptgitter einfügen
    gtk_grid_attach(GTK_GRID(grid), mwst_grid, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.euro_mwst_label, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), print_button, 3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.dm_mwst_label, 5, 1, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), euro_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.euro_entry, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), dm_label, 4, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app_widgets.dm_entry, 5, 2, 1, 1);

    // Alle Widgets anzeigen
    gtk_widget_show_all(window);

    // Hauptschleife starten
    gtk_main();

    return 0;
}