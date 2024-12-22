#include <gtk/gtk.h>

void quit(GtkWidget *window, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *label1, *label2, *label3;
    GtkWidget *hbox;
    GtkWidget *vbox;

    // GTK �ʱ�ȭ
    gtk_init(&argc, &argv);

    // ������ ���� �� ����
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "The Window Title");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);

    // �� ����
    label1 = gtk_label_new("labe1 1");
    label2 = gtk_label_new("label 2");
    label3 = gtk_label_new("label 3");

    // �ڽ� �����̳� ����
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // hbox�� �� �߰�
    gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);

    // vbox�� hbox�� �� �߰�
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label3, FALSE, FALSE, 5);

    // �����쿡 vbox �߰�
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // ��� ���� ǥ��
    gtk_widget_show_all(window);

    // GTK ���� ���� ����
    gtk_main();

    return 0;
}
