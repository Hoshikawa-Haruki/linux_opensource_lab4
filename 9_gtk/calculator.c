#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

char current_input[256] = "";
double result = 0;
char operation = '\0';

void on_button_clicked(GtkWidget *widget, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));

    const char *current_text = gtk_entry_get_text(entry);
    if (g_strcmp0(label, "C") == 0) {
        result = 0;
        operation = '\0';
        current_input[0] = '\0';
        gtk_entry_set_text(entry, "");
    } else if (g_strcmp0(label, "=") == 0) {
        double value = atof(current_input);
        if (operation == '+') result += value;
        else if (operation == '-') result -= value;
        else if (operation == '*') result *= value;
        else if (operation == '/') result /= value;

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%.2f", result);
        gtk_entry_set_text(entry, buffer);

        current_input[0] = '\0';
        operation = '\0';
    } else if (strchr("+-*/", label[0])) {
        if (current_input[0] != '\0') {
            result = atof(current_input);
            operation = label[0];
            current_input[0] = '\0';
        }
    } else {
        strncat(current_input, label, sizeof(current_input) - strlen(current_input) - 1);
        gtk_entry_set_text(entry, current_input);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Load the UI from XML
    GtkBuilder *builder = gtk_builder_new_from_file("calculator.xml");

    // Get main window and connect "destroy" signal
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Get entry widget
    GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry"));

    // Connect buttons to the signal handler
    for (int i = 0; i <= 9; i++) {
        char button_id[20];
        snprintf(button_id, sizeof(button_id), "button_%d", i);
        GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, button_id));
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
    }

    const char *operators[] = {"plus", "minus", "multiply", "divide", "clear", "equals"};
    for (int i = 0; i < 6; i++) {
        char button_id[20];
        snprintf(button_id, sizeof(button_id), "button_%s", operators[i]);
        GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, button_id));
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
    }

    // Show the window and run the GTK main loop
    gtk_widget_show_all(window);
    gtk_main();

    g_object_unref(builder);
    return 0;
}
