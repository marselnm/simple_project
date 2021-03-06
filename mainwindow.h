#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDebug>
#include <QTimer>

#include "struct_cmd.h"
#include "settingswrapper.h"
#include "file_test.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SendDataToOPU(char *data, int size);
    void CmdReadStatus();
    void PutCmdOnForm(char *data, int size);
    void PutAnsOnForm(char *data, int size);
    void IncCountCMD();
    void IncCountANS();
    void ShowCmdAnsStatus(cmd_ans_status_t *cmd_ans_status);
    void ShowCmdAnsNetparams(cmd_ans_netparams_t *cmd_ans_netparams);

signals:
    void SendStatusFotTesting(QByteArray *AnsData);

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

    void receive_message_from_opu();

    void on_pushButton_clean_edit_clicked();

    void on_checkBox_6_stateChanged(int arg1);

    void SendCmdReadStatusAuto();

    void on_test_from_file_triggered();

    void GetNewStatus();

    void SetPosition(QByteArray *position);

    void ReturnCheckBoxTrue();

    void on_pushButton_reset_error_clicked();

    void slot_stop_move_all_drives();

    void on_pushButton_set_moment_clicked();

    void slot_reset_error_drive(int NumDrive);

private:
    Ui::MainWindow *ui;
    SettingsWrapper* settings_;
    QUdpSocket *opu_socket;
    QHostAddress HostAP;
    quint16 PortAP;
    int CountCMD;
    int CountAns;
    int Connect;
    cmd_ans_status_t cmd_ans_status;
    QTimer *timeupdate;
    cmd_ans_netparams_t cmd_ans_netparams;
    FileTest *test_from_file;
};

#endif // MAINWINDOW_H
