#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDebug>

#include "struct_cmd.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void PutCmdOnForm(char *data, int size);

private slots:
    void on_pushButton_ret_az_el_clicked();

    void on_pushButton_set_dev_clicked();

    void on_pushButton_reset_dev_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_new_pos_clicked();

    void on_checkBox_6_stateChanged(int arg1);

    void on_checkBox_7_stateChanged(int arg1);

    void on_checkBox_8_stateChanged(int arg1);

    void on_pushButton_move_park_clicked();

    void on_pushButton_made_park_pos_clicked();

    void on_checkBox_9_stateChanged(int arg1);

    void on_checkBox_10_stateChanged(int arg1);

    void on_checkBox_11_stateChanged(int arg1);

    void on_pushButton_relay_clicked();

    void on_pushButton_test_clicked();

    void on_pushButton_set_servo_mode_clicked();

    void on_pushButton_status_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *opu_socket;
    QHostAddress HostAP;
    quint16 PortAP;
};

#endif // MAINWINDOW_H
