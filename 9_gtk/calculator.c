#include <gtk/gtk.h>
char current_input[256] = "";
double result = 0;
char operation = '\0';

void on_button_clicked(GtkWidget *widget, gpointer entry)
{
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));
    const char *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(label, "C") == 0)
    {
        result = 0;
        operation = '\0';
        current_input[0] = '\0';
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
    else if (g_strcmp0(label, "=") == 0)
    {
        double value = atof(current_input);
        if (operation == '+')
            result += value;
        else if (operation == '-')
            result -= value;
        else if (operation == '*')
            result *= value;
        else if (operation == '/')
            result /= value;
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%.2f", result);
        gtk_entry_set_text(GTK_ENTRY(entry), buffer);
        current_input[0] = '\0';
        operation = '\0';
    }
    else if (g_strcmp0(label, "+") == 0 || g_strcmp0(label, "-") == 0 || g_strcmp0(label, "*") == 0 || g_strcmp0(label, "/") == 0)
    {
        if (current_input[0] != '\0')
        {
            result = atof(current_input);
            operation = label[0];
            current_input[0] = '\0';
        }
    }
    else
    {
        strncat(current_input, label, sizeof(current_input) - strlen(current_input) - 1);
        gtk_entry_set_text(GTK_ENTRY(entry), current_input);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // 윈도우 생성
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK+ 계산기");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 350);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 메인 박스
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 텍스트 입력 상자
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1); // 오른쪽 정렬
    gtk_widget_set_size_request(entry, 200, 50);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

    // 버튼 그리드
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // 버튼 배열
    const char *buttons[] = {"7", "8", "9", "+", "4", "5", "6", "-", "1", "2", "3", "*", "C", "0", "=", "/"};
    for (int i = 0; i < 16; i++)
    {
        GtkWidget *button = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(button, 63, 63);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4, 1, 1);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
    }

    // 윈도우 표시
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
