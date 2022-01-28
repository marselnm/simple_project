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
    void IncCountCMD();

private slots:
    void on_pushButton_ret_az_el_clicked();

    void on_pushButton_set_dev_clicked();

    void on_pushButton_reset_dev_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_new_pos_clicked();

    void on_pushButton_move_park_clicked();

    void on_pushButton_made_park_pos_clicked();

    void on_pushButton_relay_clicked();

    void on_pushButton_test_clicked();

    void on_pushButton_set_servo_mode_clicked();

    void on_pushButton_status_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_pushButton_check_link_clicked();

    void on_pushButton_clean_cmd_clicked();

    void on_pushButton_get_dev_info_clicked();

    void on_pushButton_get_mac_clicked();

    void on_pushButton_get_netparam_clicked();

    void on_pushButton_change_netparam_clicked();

    void on_pushButton_connect_clicked();

    void on_pushButton_clean_answ_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *opu_socket;
    QHostAddress HostAP;
    quint16 PortAP;
    int CountCMD;
    int CountAns;
    int Connect;
};

#endif // MAINWINDOW_H
