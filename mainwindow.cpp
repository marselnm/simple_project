#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SettingsWrapper settings("settings.ini", this);
    settings.load();

    CountCMD = 0;
    CountAns = 0;
    Connect = 0;//соединение не было установлено
    ui->pushButton_connect->setStyleSheet("QPushButton{background-color:red;}");

    ui->lineEdit->setText("172.16.2.49");//ip-по умолчанию
    ui->lineEdit_2->setText("10000");//порт согласно универсальному протоколу

    HostAP.setAddress(ui->lineEdit->text());
    PortAP = static_cast<quint16>(ui->lineEdit_2->text().toInt());

    opu_socket = new QUdpSocket();
    opu_socket->bind(HostAP, 9999);
    connect(opu_socket, &QUdpSocket::readyRead, this, &MainWindow::receive_message_from_opu);

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

    //заполнение QComboBox для записи настроек сети
    ui->comboBox_ram_or_flash->addItem("ПЗУ");
    ui->comboBox_ram_or_flash->addItem("ОЗУ");

    //настройка таймера для обнослений
    timeupdate = new QTimer();
    timeupdate->setInterval(1000);
    connect(timeupdate, &QTimer::timeout, this, &MainWindow::SendCmdReadStatusAuto);
}

MainWindow::~MainWindow()
{
    SettingsWrapper settings("settings.ini", this);
    settings.save(); // сохраняем значения QSpinBox и QLineEdit
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

void MainWindow::PutAnsOnForm(char *data, int size)
{
    QString temp;
    for(int i = 0; i < size; i++)
    {
        temp.append("0x");
        temp.append(QString::number(*(data+i) & 0xFF, 16));
        temp.append(" ");
    }
    temp.append("\n");
    ui->textEdit_2->insertPlainText(temp);
}

void MainWindow::IncCountCMD()
{
    CountCMD++;
    ui->label_24->setNum(CountCMD);
}

void MainWindow::IncCountANS()
{
    CountAns++;
    ui->label_25->setNum(CountAns);
}

void MainWindow::ShowCmdAnsStatus(cmd_ans_status_t *cmd_ans_status)
{
    QString message_status;

    message_status.append("<b>Управ. уст-во:</b> ");

    QString value = cmd_ans_status->Device_ID ? "ЭВМ  " : "пульт  ";
    message_status.append(value);

    message_status.append("<b>Реле 1:</b> ");
    value = cmd_ans_status->StateRelay1 ? "Вкл  " : "Выкл  ";
    message_status.append(value);

    message_status.append("<b>Реле 2:</b> ");
    value = cmd_ans_status->StateRelay2 ? "Вкл  " : "Выкл  ";
    message_status.append(value);

    message_status.append("<br><br><b>Азимут текущий:</b> " + QString::number(static_cast<double>(cmd_ans_status->CurrentAZ)) + " ");
    message_status.append("<b>Заданный:</b> " + QString::number(static_cast<double>(cmd_ans_status->SetAZ)) + " ");
    message_status.append("<b>Парковочный:</b> " + QString::number(static_cast<double>(cmd_ans_status->ParkAZ)) + " ");
    message_status.append("<br><b>Состояние:</b> ");

    switch (cmd_ans_status->StateDriveAZ)
    {
        case 0x00:
        {
            message_status.append("остановлен");
            break;
        }
        case 0x01:
        {
            message_status.append("движение");
            break;
        }
        case 0x02:
        {
            message_status.append("не определено");
            break;
        }
    }

    message_status.append("<br><br><b>Угол места текущий:</b> " + QString::number(static_cast<double>(cmd_ans_status->CurrentEL)) + " ");
    message_status.append("<b>Заданный:</b> " + QString::number(static_cast<double>(cmd_ans_status->SetEL)) + " ");
    message_status.append("<b>Парковочный:</b> " + QString::number(static_cast<double>(cmd_ans_status->ParkEL)) + " ");
    message_status.append("<br><b>Состояние:</b> ");

    switch (cmd_ans_status->StateDriveEL)
    {
        case 0x00:
        {
            message_status.append("остановлен");
            break;
        }
        case 0x01:
        {
            message_status.append("движение");
            break;
        }
        case 0x02:
        {
            message_status.append("не определено");
            break;
        }
    }


    message_status.append("<br><br><b>Концевики AZ:</b> ");

    int KVOaz = (cmd_ans_status->EndSwitchesAZ >> 0) & 0x01;
    if(KVOaz){
        message_status.append("КВO:" + QString::number(KVOaz) + " ");
    }else {
        message_status.append("КВО:<font color='red'>" + QString::number(KVOaz) + "</font>" + " ");
    }

    int KVPaz = (cmd_ans_status->EndSwitchesAZ >> 1) & 0x01;
    if(KVPaz){
        message_status.append("КВП:" + QString::number(KVPaz) + " ");
    }else {
        message_status.append("КВП:<font color='red'>" + QString::number(KVPaz) + "</font>" + " ");
    }

    int KVRaz = (cmd_ans_status->EndSwitchesAZ >> 2) & 0x01;
    if(KVRaz){
        message_status.append("КВР:" + QString::number(KVRaz) + " ");
    }else {
        message_status.append("КВР:<font color='red'>" + QString::number(KVRaz) + "</font>" + " ");
    }


    message_status.append("<b>Концевики EL:</b> ");
    int KVOel = (cmd_ans_status->EndSwitchesEL >> 0) & 0x01;
    if(KVOel){
        message_status.append("КВO:" + QString::number(KVOel) + " ");
    }else {
        message_status.append("КВО:<font color='red'>" + QString::number(KVOel) + "</font>" + " ");
    }

    int KVPel = (cmd_ans_status->EndSwitchesEL >> 1) & 0x01;
    if(KVPel){
        message_status.append("КВП:" + QString::number(KVPel) + " ");
    }else {
        message_status.append("КВП:<font color='red'>" + QString::number(KVPel) + "</font>" + " ");
    }

    int KVRel = (cmd_ans_status->EndSwitchesEL >> 2) & 0x01;
    if(KVRel){
        message_status.append("КВР:" + QString::number(KVRel) + " ");
    }else {
        message_status.append("КВР:<font color='red'>" + QString::number(KVRel) + "</font>" + " ");
    }

    message_status.append("<br><b>Привод AZ:</b> ");

    if(cmd_ans_status->ConnectAZ == 0x01)
    {
        message_status.append("Подкл. ");
    }else
    {
        message_status.append("<font color='red'>Нет связи</font> ");
    }

    message_status.append("<b>Привод EL:</b> ");

    if(cmd_ans_status->ConnectEL == 0x01)
    {
        message_status.append("Подкл.");
    }else
    {
        message_status.append("<font color='red'>Нет связи</font>");
    }

    message_status.append("<br><br><b>Темп. AZ:</b> " + QString::number(cmd_ans_status->TemperatureAZ) + " град ");
    message_status.append("<b>Темп. EL:</b> " + QString::number(cmd_ans_status->TemperatureEL) + " град ");

    message_status.append("<br><b>Скорость AZ:</b> " + QString::number(static_cast<double>(cmd_ans_status->SpeedAZ)) + " ");
    message_status.append("<b>Скорость EL:</b> " + QString::number(static_cast<double>(cmd_ans_status->SpeedEL)));

    message_status.append("<br><b>Момент AZ:</b> " + QString::number(static_cast<double>(cmd_ans_status->DriveMomAZ)) + " ");
    message_status.append("<b>Момент EL:</b> " + QString::number(static_cast<double>(cmd_ans_status->DriveMomEL)));

    message_status.append("<br><br><b>Код ошибки AZ:</b> 0x" + QString::number(cmd_ans_status->ErrosCodeAZ & 0xFFFF, 16) + " ");
    message_status.append("<b>Код ошибки EL:</b> 0x" + QString::number(cmd_ans_status->ErrosCodeEL & 0xFFFF, 16));

    message_status.append("<br><b>Сигналы привода AZ:</b> ");
    int SONaz = (cmd_ans_status->SignalAZ >> 0) & 0x01;
    message_status.append("SON:" + QString::number(SONaz) + " ");
    int LSNaz = (cmd_ans_status->SignalAZ >> 1) & 0x01;
    message_status.append("LSN:" + QString::number(LSNaz) + " ");
    int LSPaz = (cmd_ans_status->SignalAZ >> 2) & 0x01;
    message_status.append("LSP:" + QString::number(LSPaz) + " ");
    int EM2az = (cmd_ans_status->SignalAZ >> 3) & 0x01;
    message_status.append("EM2:" + QString::number(EM2az) + " ");
    int MBRaz = (cmd_ans_status->SignalAZ >> 4) & 0x01;
    message_status.append("MBR:" + QString::number(MBRaz) + " ");
    int RESaz = (cmd_ans_status->SignalAZ >> 5) & 0x01;
    message_status.append("RES:" + QString::number(RESaz) + " ");
    int OUT1az = (cmd_ans_status->SignalAZ >> 6) & 0x01;
    message_status.append("OUT1:" + QString::number(OUT1az) + " ");
    int PEDaz = (cmd_ans_status->SignalAZ >> 7) & 0x01;
    message_status.append("PED:" + QString::number(PEDaz));

    message_status.append("<br><b>Сигналы привода EL:</b> ");
    int SONel = (cmd_ans_status->SignalEL >> 0) & 0x01;
    message_status.append("SON:" + QString::number(SONel) + " ");
    int LSNel = (cmd_ans_status->SignalEL >> 1) & 0x01;
    message_status.append("LSN:" + QString::number(LSNel) + " ");
    int LSPel = (cmd_ans_status->SignalEL >> 2) & 0x01;
    message_status.append("LSP:" + QString::number(LSPel) + " ");
    int EM2el = (cmd_ans_status->SignalEL >> 3) & 0x01;
    message_status.append("EM2:" + QString::number(EM2el) + " ");
    int MBRel = (cmd_ans_status->SignalEL >> 4) & 0x01;
    message_status.append("MBR:" + QString::number(MBRel) + " ");
    int RESel = (cmd_ans_status->SignalEL >> 5) & 0x01;
    message_status.append("RES:" + QString::number(RESel) + " ");
    int OUT1el = (cmd_ans_status->SignalEL >> 6) & 0x01;
    message_status.append("OUT1:" + QString::number(OUT1el) + " ");
    int PEDel = (cmd_ans_status->SignalEL >> 7) & 0x01;
    message_status.append("PED:" + QString::number(PEDel));

    message_status.append("<br><b>Допустимая ошибка AZ:</b> " + QString::number(static_cast<double>(cmd_ans_status->ErrorAZ)));
    message_status.append("<br><b>Допустимая ошибка EL:</b> " + QString::number(static_cast<double>(cmd_ans_status->ErrorEL)));

    message_status.append("<br><b>Режим двигателя AZ:</b> ");
    value = cmd_ans_status->DiriveModeAZ ? "FLAG_Brake" : "FLAG_Complete";
    message_status.append(value);

    message_status.append("<br><b>Режим двигателя EL:</b> ");
    value = cmd_ans_status->DiriveModeEL ? "FLAG_Brake" : "FLAG_Complete";
    message_status.append(value);

    message_status.append("<br><b>AZ: Отправл. команды:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountCmdSendDrAZ)) + " ");
    message_status.append("<b>Ошиб.nответы:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountErrorAnsDrAZ)) + " ");
    message_status.append("<b>Пропущ. ответы:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountMissedAnsDrAZ)));

    message_status.append("<br><b>EL: Отправл. команды:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountCmdSendDrEL)) + " ");
    message_status.append("<b>Ошиб. ответы:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountErrorAnsDrEL)) + " ");
    message_status.append("<b>Пропущ. ответы:</b> " + QString::number(static_cast<double>(cmd_ans_status->CountMissedAnsDrEL)));

    ui->textEdit_3->setText(message_status);
}

void MainWindow::ShowCmdAnsNetparams(cmd_ans_netparams_t *cmd_ans_netparams)
{
    QString message_netparams;
    message_netparams.append("<b>Сетевые параметры:</b>");
    message_netparams.append("<br>Адрес шлюза: " + QHostAddress(cmd_ans_netparams->Gateway).toString());
    message_netparams.append("<br>Макска подсети: " + QHostAddress(cmd_ans_netparams->Netmask).toString());
    message_netparams.append("<br>IP-адрес: " + QHostAddress(cmd_ans_netparams->IPdevice).toString());
    ui->textEdit_3->setText(message_netparams);
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_position), cmd_set_position.Lenght, HostAP, PortAP);

        if(temp == cmd_set_position.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_position),  cmd_set_position.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_cntrl_dev), cmd_set_cntrl_dev.Lenght, HostAP, PortAP);

        if(temp == cmd_set_cntrl_dev.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_cntrl_dev),  cmd_set_cntrl_dev.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_reset_dev_clicked()
{
    cmd_simple_t cmd_simple;
    cmd_simple.Lenght = sizeof (cmd_simple_t);
    cmd_simple.Message_ID = CMD_RESET_DEV;

    if(1)//если связь была установлена
    {         
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_simple), cmd_simple.Lenght, HostAP, PortAP);

        if(temp == cmd_simple.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_simple),  cmd_simple.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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

    if(1 && cmd_stop_moving.Mask != 0)//если связь была установлена
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_stop_moving), cmd_stop_moving.Lenght, HostAP, PortAP);

        if(temp == cmd_stop_moving.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_stop_moving),  cmd_stop_moving.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_adjust_pos_sensor), cmd_adjust_pos_sensor.Lenght, HostAP, PortAP);

        if(temp == cmd_adjust_pos_sensor.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_adjust_pos_sensor),  cmd_adjust_pos_sensor.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_move_park_position), cmd_move_park_position.Lenght, HostAP, PortAP);

        if(temp == cmd_move_park_position.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_move_park_position),  cmd_move_park_position.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_adjust_park_position), cmd_adjust_park_position.Lenght, HostAP, PortAP);

        if(temp == cmd_adjust_park_position.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_adjust_park_position),  cmd_adjust_park_position.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_relay_state), cmd_set_relay_state.Lenght, HostAP, PortAP);

        if(temp == cmd_set_relay_state.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_relay_state),  cmd_set_relay_state.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    cmd_test_servo_connection_t cmd_test_servo_connection;
    cmd_test_servo_connection.Lenght = sizeof (cmd_test_servo_connection_t);
    cmd_test_servo_connection.Message_ID = CMD_TEST_SERVO_CONNECTION;
    cmd_test_servo_connection.TypeOfTest = static_cast<uint8_t>(ui->comboBox_test->currentIndex() + 1);

    if(1)//если связь была установлена
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_test_servo_connection), cmd_test_servo_connection.Lenght, HostAP, PortAP);

        if(temp == cmd_test_servo_connection.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_test_servo_connection),  cmd_test_servo_connection.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_set_servo_work_mode), cmd_set_servo_work_mode.Lenght, HostAP, PortAP);

        if(temp == cmd_set_servo_work_mode.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_set_servo_work_mode),  cmd_set_servo_work_mode.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_status_clicked()
{
    cmd_read_status_t cmd_read_status;
    cmd_read_status.Lenght = sizeof (cmd_read_status_t);
    cmd_read_status.Message_ID = CMD_READ_STATUS;

    if(1)//если связь была установлена
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_read_status), cmd_read_status.Lenght, HostAP, PortAP);

        if(temp == cmd_read_status.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_read_status),  cmd_read_status.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
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

void MainWindow::on_pushButton_check_link_clicked()
{
    cmd_simple_t cmd_simple;
    cmd_simple.Lenght = sizeof (cmd_simple_t);
    cmd_simple.Message_ID = CMD_CHECK_LINK;

    qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_simple), cmd_simple.Lenght, HostAP, PortAP);

    if(temp == cmd_simple.Lenght)
    {
        PutCmdOnForm(reinterpret_cast<char*>(&cmd_simple),  cmd_simple.Lenght);
        IncCountCMD();
    }else {
        qDebug() << "Не удалось отправить";
    }
}

void MainWindow::on_pushButton_clean_cmd_clicked()
{
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_get_dev_info_clicked()
{
    cmd_simple_t cmd_simple;
    cmd_simple.Lenght = sizeof (cmd_simple_t);
    cmd_simple.Message_ID = CMD_GET_DEVICE_INFO;

    if(1)
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_simple), cmd_simple.Lenght, HostAP, PortAP);

        if(temp == cmd_simple.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_simple),  cmd_simple.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_get_mac_clicked()
{
    cmd_simple_t cmd_simple;
    cmd_simple.Lenght = sizeof (cmd_simple_t);
    cmd_simple.Message_ID = CMD_GET_MAC_ADDRESS;

    if(1)
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_simple), cmd_simple.Lenght, HostAP, PortAP);

        if(temp == cmd_simple.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_simple),  cmd_simple.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_get_netparam_clicked()
{
    cmd_simple_t cmd_simple;
    cmd_simple.Lenght = sizeof (cmd_simple_t);
    cmd_simple.Message_ID = CMD_GET_NETPARAMS;

    if(1)
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_simple), cmd_simple.Lenght, HostAP, PortAP);

        if(temp == cmd_simple.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_simple),  cmd_simple.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_change_netparam_clicked()
{
    cmd_change_netparams_t cmd_change_netparams;
    cmd_change_netparams.Lenght = sizeof (cmd_change_netparams_t);
    cmd_change_netparams.Message_ID = CMD_CHANGE_NETPARAMS;
    cmd_change_netparams.Gateway = QHostAddress(ui->lineEdit_11->text()).toIPv4Address();
    cmd_change_netparams.Netmask = QHostAddress(ui->lineEdit_12->text()).toIPv4Address();
    cmd_change_netparams.IPdevice = QHostAddress(ui->lineEdit_13->text()).toIPv4Address();

    if(ui->comboBox_ram_or_flash->currentIndex() == 0)
    {
        cmd_change_netparams.RAMofFlash = 0x00;
    }else
    {
        cmd_change_netparams.RAMofFlash = 0xFF;
    }

    if(1)
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_change_netparams), cmd_change_netparams.Lenght, HostAP, PortAP);

        if(temp == cmd_change_netparams.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_change_netparams),  cmd_change_netparams.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}

void MainWindow::on_pushButton_connect_clicked()
{
    HostAP.setAddress(ui->lineEdit->text());
    PortAP = static_cast<quint16>(ui->lineEdit_2->text().toInt());

    Connect = 1;
    ui->pushButton_connect->setStyleSheet("QPushButton{background-color:green;}");
}

void MainWindow::on_pushButton_clean_answ_clicked()
{
    ui->textEdit_2->clear();
}

void MainWindow::receive_message_from_opu()
{
    QByteArray Buffer;
    Buffer.clear();
    Buffer.resize(static_cast<int>(opu_socket->pendingDatagramSize()));
    opu_socket->readDatagram(Buffer.data(), Buffer.size());
    PutAnsOnForm(Buffer.data(),  Buffer.size());
    IncCountANS();

    switch (Buffer.at(2))
    {
        case (static_cast<char>(CMD_ANS_STATUS)):
        {
            if(Buffer.size() == sizeof(cmd_ans_status_t))
            {
                memcpy(reinterpret_cast<char*>(&cmd_ans_status), Buffer.data(), sizeof(cmd_ans_status_t));
                ShowCmdAnsStatus(&cmd_ans_status);
            }
            break;
        }
        case (static_cast<char>(CMD_ANS_NETPARAMS)):
        {
            if(Buffer.size() == sizeof(cmd_ans_netparams_t))
            {
                memcpy(reinterpret_cast<char*>(&cmd_ans_netparams), Buffer.data(), sizeof(cmd_ans_netparams));
                ShowCmdAnsNetparams(&cmd_ans_netparams);
            }
            break;
        }
    }
}

void MainWindow::on_pushButton_clean_edit_clicked()
{
    ui->textEdit_3->clear();
}

void MainWindow::on_checkBox_6_stateChanged(int arg1)
{
    if(arg1 == 2)
    {
        timeupdate->start();
    }else
    {
        timeupdate->stop();
    }
}

void MainWindow::SendCmdReadStatusAuto()
{
    cmd_read_status_t cmd_read_status;
    cmd_read_status.Lenght = sizeof (cmd_read_status_t);
    cmd_read_status.Message_ID = CMD_READ_STATUS;

    if(1)//если связь была установлена
    {
        qint64 temp = opu_socket->writeDatagram(reinterpret_cast<const char*>(&cmd_read_status), cmd_read_status.Lenght, HostAP, PortAP);

        if(temp == cmd_read_status.Lenght)
        {
            PutCmdOnForm(reinterpret_cast<char*>(&cmd_read_status),  cmd_read_status.Lenght);
            IncCountCMD();
        }else {
            qDebug() << "Не удалось отправить";
        }
    }
}
