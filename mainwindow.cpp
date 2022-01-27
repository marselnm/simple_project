#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText("10.0.0.74");//ip-по умолчанию
    ui->lineEdit_2->setText("10000");//порт согласно универсальному протоколу

    opu_socket = new QUdpSocket();
    HostAP.setAddress(ui->lineEdit->text());
    PortAP = static_cast<quint16>(ui->lineEdit_2->text().toInt());

    //заполнение QComboBox для коррекции положения
    ui->comboBox_cor_pos->addItem("AZ");
    ui->comboBox_cor_pos->addItem("EL");
    ui->comboBox_cor_pos->addItem("POL");

    //заполнение QComboBox для настройки позиции парковки
    ui->comboBox_park_pos->addItem("AZ");
    ui->comboBox_park_pos->addItem("EL");
    ui->comboBox_park_pos->addItem("POL");

    //заполнение QComboBox для реле
    ui->comboBox_relay->addItem("Реле 1");
    ui->comboBox_relay->addItem("Реле 2");
    ui->comboBox_state_relay->addItem("Выкл");
    ui->comboBox_state_relay->addItem("Вкл");

    //заполнение QComboBox для тестов
    ui->comboBox_test->addItem("TEST_1A");
    ui->comboBox_test->addItem("TEST_2A");
    ui->comboBox_test->addItem("TEST_1B");
    ui->comboBox_test->addItem("TEST_2B");
    ui->comboBox_test->addItem("SON_1A");
    ui->comboBox_test->addItem("SON_2A");
    ui->comboBox_test->addItem("SON_1B");
    ui->comboBox_test->addItem("SON_2B");
    ui->comboBox_test->addItem("RES_1A");
    ui->comboBox_test->addItem("RES_2A");
    ui->comboBox_test->addItem("RES_1B");
    ui->comboBox_test->addItem("RES_2B");
    ui->comboBox_test->addItem("RESES_STATS");
    ui->comboBox_test->addItem("TEST1_AZ_AL39");
    ui->comboBox_test->addItem("TEST2_AZ_AL39");
    ui->comboBox_test->addItem("TEST1_EL_AL39");
    ui->comboBox_test->addItem("TEST2_EL_AL39");

    //заполнение QComboBox для задани режима работы двигателя
    ui->comboBox_num_servo_mode->addItem("AZ");
    ui->comboBox_num_servo_mode->addItem("EL");
    ui->comboBox_num_servo_mode->addItem("POL");
    ui->comboBox_type_coord_sys->addItem("Тип 0");
    ui->comboBox_type_coord_sys->addItem("Тип 1");
    ui->comboBox_complate->addItem("Выкл");
    ui->comboBox_complate->addItem("Вкл");
    ui->comboBox_brake_auto->addItem("Выкл");
    ui->comboBox_brake_auto->addItem("Вкл");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PutCmdOnForm(char *data, int size)
{
    QString temp;
    for(int i = 0; i < size; i++)
    {
        temp.append("0x");
        temp.append(QString::number(*(data+i) & 0xFF, 16));
        temp.append(" ");
    }
    temp.append("\n");
    ui->textEdit->insertPlainText(temp);
}

void MainWindow::on_pushButton_ret_az_el_clicked()
{
    cmd_set_position_t cmd_set_position;
    cmd_set_position.Lenght = sizeof (cmd_set_position_t);
    cmd_set_position.Message_ID = CMD_SET_POSITION;
    cmd_set_position.AZ_Speed = ui->lineEdit_5->text().toFloat();
    cmd_set_position.AZ_Position = ui->lineEdit_6->text().toFloat();
    cmd_set_position.EL_Speed = ui->lineEdit_7->text().toFloat();
    cmd_set_position.EL_Position = ui->lineEdit_8->text().toFloat();

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_position), cmd_set_position.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_position),  cmd_set_position.Lenght);
    }
}

void MainWindow::on_pushButton_set_dev_clicked()
{
    cmd_set_cntrl_dev_t cmd_set_cntrl_dev;
    cmd_set_cntrl_dev.Lenght = sizeof (cmd_set_cntrl_dev_t);
    cmd_set_cntrl_dev.Message_ID = CMD_SET_CNTRL_DEV;
    cmd_set_cntrl_dev.Device_ID = -1;//устройство не задано

    if(ui->checkBox->isChecked())
    {
        cmd_set_cntrl_dev.Device_ID = 0x00;//от пульта
    }

    if(ui->checkBox_2->isChecked())
    {
        cmd_set_cntrl_dev.Device_ID = 0x01;//от ЭМВ
    }

    if(cmd_set_cntrl_dev.Device_ID == -1)
    {
        qDebug() << "Устройство не выбрано";
    }

    if(cmd_set_cntrl_dev.Device_ID != -1 && 1)//добавить проверку установки связи
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_cntrl_dev), cmd_set_cntrl_dev.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_cntrl_dev),  cmd_set_cntrl_dev.Lenght);
    }
}

void MainWindow::on_pushButton_reset_dev_clicked()
{
    cmd_reset_dev_t cmd_reset_dev;
    cmd_reset_dev.Lenght = sizeof (cmd_reset_dev_t);
    cmd_reset_dev.Message_ID = CMD_RESET_DEV;

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_reset_dev), cmd_reset_dev.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_reset_dev),  cmd_reset_dev.Lenght);
    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    cmd_stop_moving_t cmd_stop_moving;
    cmd_stop_moving.Lenght = sizeof (cmd_stop_moving_t);
    cmd_stop_moving.Message_ID = CMD_STOP_MOVING;
    cmd_stop_moving.Mask = 0;

    if(ui->checkBox_3->isChecked())
    {
        cmd_stop_moving.Mask = 0x01 << 0;//установить соответствующую маску
    }

    if(ui->checkBox_4->isChecked())
    {
        cmd_stop_moving.Mask = cmd_stop_moving.Mask | 0x01 << 1;//установить соответствующую маску
    }

    if(ui->checkBox_5->isChecked())
    {
        cmd_stop_moving.Mask = cmd_stop_moving.Mask | 0x01 << 2;//установить соответствующую маску
    }

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_stop_moving), cmd_stop_moving.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_stop_moving),  cmd_stop_moving.Lenght);
    }
}

void MainWindow::on_pushButton_new_pos_clicked()
{
    cmd_adjust_pos_sensor_t cmd_adjust_pos_sensor;
    cmd_adjust_pos_sensor.Lenght = sizeof (cmd_adjust_pos_sensor_t);
    cmd_adjust_pos_sensor.Message_ID = CMD_ADJUST_POS_SENSOR;
    cmd_adjust_pos_sensor.Axis = static_cast<uint8_t>(ui->comboBox_cor_pos->currentIndex() + 1);
    cmd_adjust_pos_sensor.Position = ui->lineEdit_3->text().toFloat();

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_adjust_pos_sensor), cmd_adjust_pos_sensor.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_adjust_pos_sensor),  cmd_adjust_pos_sensor.Lenght);
    }
}

void MainWindow::on_pushButton_move_park_clicked()
{
    cmd_move_park_position_t cmd_move_park_position;
    cmd_move_park_position.Lenght = sizeof (cmd_move_park_position_t);
    cmd_move_park_position.Message_ID = CMD_MOVE_PARK_POSITION;
    cmd_move_park_position.AZ_Speed = ui->lineEdit_4->text().toFloat();
    cmd_move_park_position.EL_Speed = ui->lineEdit_9->text().toFloat();

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_move_park_position), cmd_move_park_position.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_move_park_position),  cmd_move_park_position.Lenght);
    }
}


void MainWindow::on_pushButton_made_park_pos_clicked()
{
    cmd_adjust_park_position_t cmd_adjust_park_position;
    cmd_adjust_park_position.Lenght = sizeof (cmd_adjust_park_position_t);
    cmd_adjust_park_position.Message_ID = CMD_ADJUST_PARK_POSITION;
    cmd_adjust_park_position.Axis = static_cast<uint8_t>(ui->comboBox_park_pos->currentIndex() + 1);

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_adjust_park_position), cmd_adjust_park_position.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_adjust_park_position),  cmd_adjust_park_position.Lenght);
    }
}

void MainWindow::on_pushButton_relay_clicked()
{
    cmd_set_relay_state_t cmd_set_relay_state;
    cmd_set_relay_state.Lenght = sizeof (cmd_set_relay_state_t);
    cmd_set_relay_state.Message_ID = CMD_SET_RELAY_STATE;
    cmd_set_relay_state.RelayNum = static_cast<uint8_t>(ui->comboBox_relay->currentIndex() + 1);
    cmd_set_relay_state.RelayState = static_cast<uint8_t>(ui->comboBox_state_relay->currentIndex());

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_relay_state), cmd_set_relay_state.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_relay_state),  cmd_set_relay_state.Lenght);
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    cmd_test_sevo_connection_t cmd_test_sevo_connection;
    cmd_test_sevo_connection.Lenght = sizeof (cmd_test_sevo_connection_t);
    cmd_test_sevo_connection.Message_ID = CMD_TEST_SEVO_CONNECTION;
    cmd_test_sevo_connection.TypeOfTest = static_cast<uint8_t>(ui->comboBox_test->currentIndex() + 1);

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_test_sevo_connection), cmd_test_sevo_connection.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_test_sevo_connection),  cmd_test_sevo_connection.Lenght);
    }
}

void MainWindow::on_pushButton_set_servo_mode_clicked()
{
    cmd_set_servo_work_mode_t cmd_set_servo_work_mode;
    cmd_set_servo_work_mode.Lenght = sizeof (cmd_set_servo_work_mode_t);
    cmd_set_servo_work_mode.Message_ID = CMD_SET_SERVO_WORK_MODE;
    cmd_set_servo_work_mode.Axis = static_cast<uint8_t>(ui->comboBox_num_servo_mode->currentIndex() + 1);
    cmd_set_servo_work_mode.Mode_Axis = static_cast<uint8_t>(ui->comboBox_type_coord_sys->currentIndex());
    cmd_set_servo_work_mode.Mode_Complate = static_cast<uint8_t>(ui->comboBox_complate->currentIndex());
    cmd_set_servo_work_mode.Mode_Brake = static_cast<uint8_t>(ui->comboBox_brake_auto->currentIndex());
    cmd_set_servo_work_mode.Mode_Error = static_cast<uint8_t>(ui->lineEdit_10->text().toInt());

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_servo_work_mode), cmd_set_servo_work_mode.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_servo_work_mode),  cmd_set_servo_work_mode.Lenght);
    }
}

void MainWindow::on_pushButton_status_clicked()
{
    cmd_read_status_t cmd_read_status;
    cmd_read_status.Lenght = sizeof (cmd_read_status_t);
    cmd_read_status.Message_ID = CMD_READ_STATUS;

    if(1)//если связь была установлена
    {
        opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_read_status), cmd_read_status.Lenght, HostAP, PortAP);
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_read_status),  cmd_read_status.Lenght);
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2)
    {
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setCheckable(false);
    }else
    {
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setCheckable(true);
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == 2)
    {
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox->setCheckable(false);
    }else
    {
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox->setCheckable(true);
    }
}
