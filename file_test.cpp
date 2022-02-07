#include "file_test.h"
#include "ui_file_test.h"

#include <QFileDialog>
#include <cmath>
#include <QDebug>
#include <QThread>


FileTest::FileTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileTest)
{
    ui->setupUi(this);

    connect(&statusTimer, &QTimer::timeout, this, &FileTest::read_status);


}

FileTest::~FileTest()
{
    delete ui;
}

void FileTest::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    statusTimer.stop();
}

void FileTest::on_bOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выбрать файл тестов", QDir::homePath(), "*.csv");
    ui->leFileName->setText(fileName);

    if(fileName.isEmpty())
        return;

    tests = readFile(fileName);
    ui->lNumberTest->setText(QString::number(tests.size()));


//    QVector<file_test::TestOpu>::iterator it = tests.begin(); // создаем итератор и переводим его в начало списка
//    while (it != tests.end()) {
//         qDebug()  << it->az << it->el << it->pol;
//         ++it;
//    }

    if(tests.size() > 0)
        ui->bStart->setEnabled(true);
}

void FileTest::read_status()
{
    emit GetStatus();
}

void FileTest::GetStatusFotTesting(QByteArray *AnsData)
{
    memcpy(reinterpret_cast<char*>(&cmd_ans_status_2), AnsData->data(), sizeof(cmd_ans_status_t));
}

QVector<file_test::TestOpu> FileTest::readFile(QString title){

    QVector<file_test::TestOpu> tests;

    QFile file(title);
    file.open(QFile::ReadOnly);

    while (!file.atEnd()) {

        QByteArray array = file.readLine();
        QString str(array);
        if(str.indexOf("\"") >= 0)
            continue;

        QStringList list = str.trimmed().replace(' ', ';').split(QRegExp("[\n\r;,]+"));

        if(list.size() < 3){
            list.append("");
            list.append("");
        }

        file_test::TestOpu test;

        bool okAz, okEl, okPol;
        test.az = static_cast<QString>(list.at(0)).toFloat(&okAz);
        test.el = static_cast<QString>(list.at(1)).toFloat(&okEl);
        test.pol = static_cast<QString>(list.at(2)).toFloat(&okPol);

        if((okAz || okEl || okPol) && checkValues(test))
            tests.append(test);
    }

    file.close();

    return tests;
}

bool FileTest::checkValues(file_test::TestOpu test)
{
    if(test.az < minAz || test.az > maxAz)
        return false;

    if(test.el < minEl || test.el > maxEl)
        return false;

    if(test.pol < minPol || test.pol > maxPol)
        return false;

    return true;
}

//void FileTest::newStatus(const ca_messages::out::Status &status)
//{
//    if(!isVisible())
//        return;

//    if(!startFlag)
//        return;

////    qDebug() << "status" << status.opu_az_state_drive << status.opu_el_state_drive << status.position.azimuth << status.position.elevation;

//    position.azimuth = status.position.azimuth;
//    position.elevation = status.position.elevation;
//    position.polarization = status.position.polarization;

//    if(state == file_test::TestOpuState::WAIT_STOP_STATUS && status.opu_az_state_drive == 0 && status.opu_el_state_drive == 0){
////        qDebug() << "стоим";

//        if(fabs(position.azimuth - tests[currentTest].az) <= errorAngle &&
//                fabs(position.elevation - tests[currentTest].el) <= errorAngle &&
//                (status.device == ca_messages::Device::NAP_KU ?
//                 (fabs(position.polarization - tests[currentTest].pol) <= errorAngle) : true)){


//            updateResult(currentTest, true);

//            ca_messages::in::StopMove command;
//            ca_messages::Message message;
//            message.append(reinterpret_cast<char*>(&command), ca_messages::sizeOfStopMove);
//            emit messageReady(message);
//            state = file_test::TestOpuState::WAIT_STOP_MOVE;

//            currentTest++;

//        } else {
//             runTest(currentTest);
//        }

//        return;
//    }

//    if(state == file_test::TestOpuState::WAIT_STOP_STATUS) {
////        qDebug() << "запрашиваем новый статус";

////        statusTimer.start(500);
//        return;
//    }

//    if(state == file_test::TestOpuState::WAIT_START_TEST && status.opu_az_state_drive == 0 && status.opu_el_state_drive == 0) {
////        qDebug() << status.opu_az_state_drive << status.opu_el_state_drive;
//        return;
//    }

//    if(state == file_test::TestOpuState::WAIT_START_TEST && (status.opu_az_state_drive == 1 || status.opu_el_state_drive == 1)) {

////        qDebug() << "начинаем тест";

//        waitstartTimer.stop();

//        uint dAz = ceil(fabs(position.azimuth - tests[currentTest].az));
//        uint dEl = ceil(fabs(position.elevation - tests[currentTest].el));
//        uint dPol = ceil(fabs(position.polarization - tests[currentTest].pol));

//        uint maxValue = std::max(std::max(dAz, dEl), dPol);

////        qDebug() << "waittime" <<maxValue;
//        testTimer.stop();
//        testTimer.start(maxValue * 1000 + waittime);

//        state = file_test::TestOpuState::RUN_TEST;

//        return;
//    }



//    if(state == file_test::TestOpuState::RUN_TEST && status.opu_az_state_drive == 0 && status.opu_el_state_drive == 0) {
//        checkPosition = true;
//    }


//    if(state == file_test::TestOpuState::RUN_TEST && checkPosition == true){

//        float diff = position.azimuth - tests[currentTest].az;
//        if(diff > 359)
//            diff-=360;
//        else if(diff < -359)
//            diff+=360;

//        if(status.opu_az_state_drive != 0){
//            updateResult(currentTest, false);
//            ui->tbInfo->append("opu_az_state_drive:" + QString::number(status.opu_az_state_drive));
//        }
//        else if(status.opu_el_state_drive != 0){
//            updateResult(currentTest, false);
//            ui->tbInfo->append("opu_az_state_drive:" + QString::number(status.opu_el_state_drive));
//        }
//        else if(fabs(diff) > errorAngle)
//        {    updateResult(currentTest, false);
//        ui->tbInfo->append("current az:" + QString::number(position.azimuth) +
//                           " az:" + QString::number(tests[currentTest].az));
//        }
//        else if(fabs(position.elevation - tests[currentTest].el) > errorAngle){
//            updateResult(currentTest, false);
//            ui->tbInfo->append("current el:" + QString::number(position.elevation) +
//                               " el:" + QString::number(tests[currentTest].el));
//        }
//        else if(status.device == ca_messages::Device::NAP_KU && fabs(position.polarization - tests[currentTest].pol) > errorAngle)
//        {
//            updateResult(currentTest, false);
//            ui->tbInfo->append("current pol:" + QString::number(position.polarization) +
//                               " pol:" + QString::number(tests[currentTest].pol));
//        } else {
//            updateResult(currentTest, true);
//        }
//        currentTest++;

////        QThread::msleep(500);

////qDebug() << "stop move" << status.opu_az_state_drive << status.opu_el_state_drive;
//        ca_messages::in::StopMove command;
//        ca_messages::Message message;
//        message.append(reinterpret_cast<char*>(&command), ca_messages::sizeOfStopMove);
//        emit messageReady(message);
//        state = file_test::TestOpuState::WAIT_STOP_MOVE;

//        checkPosition = false;
//        testTimer.stop();
//    }

//    if(currentTest == static_cast<uint>(tests.size()))
//    {
//        restart();
//    }


//}

//void FileTest::newOk(const ca_messages::out::Ok &ok)
//{
//    if(!isVisible())
//        return;

//    if(ok.command == ca_messages::CMD_CHECK_LINK)
//        return;

//    if(ok.command == ca_messages::CMD_STOP_MOVE && state == file_test::TestOpuState::WAIT_STOP_MOVE)
//        state = file_test::TestOpuState::WAIT_STOP_STATUS;

//    if(ok.command == ca_messages::CMD_ROTATE_MANUAL)// && state == TestOpuState::WAIT_START_TEST)
//    {
//        state = file_test::TestOpuState::WAIT_START_TEST;
////        qDebug() << "state" << (uint8_t)state;
//        waitstartTimer.start(waitStartTime*1000);
//    }

////    qDebug() << "ok"<< ok.command ;
//}

//void FileTest::newError(const ca_messages::out::Error &error)
//{
//    if(!isVisible())
//        return;

//    if(error.command == ca_messages::CMD_CHECK_LINK)
//        return;

//    restart();
//    errorTest();
//}

//void FileTest::errorTest()
//{
//    ui->tbInfo->setStyleSheet("background-color: rgb(255, 0, 0);");
//    ui->tbInfo->append("Ошибка выполнения теста");
//}

//void FileTest::restart()
//{

//    startFlag = false;
//    checkPosition = false;

//    successTest = 0;

//    ui->bStop->setEnabled(false);
//    ui->bStart->setEnabled(true);
//    ui->bOpenFile->setEnabled(true);

//    currentTest = 0;

//    statusTimer.stop();
//    waitstartTimer.stop();

//}

//void FileTest::updateResult(uint test, bool result)
//{
//    if(test == static_cast<uint>(tests.size()))
//        return;

//    if(result){
//        ui->tbInfo->append("Тест " + QString::number(test+1) + " выполнен успешно");
//        successTest++;
//        ui->lSuccess->setText(QString::number(successTest));
//    }
//    else
//        ui->tbInfo->append("Тест " + QString::number(test+1) + " не выполнен");
//}

//void FileTest::runTest(uint test)
//{

//    testTimer.stop();
//    if(test >= static_cast<uint>(tests.size()))
//    {
//        state = file_test::TestOpuState::FINISH_TEST;
//        restart();
//        return;
//    }

//    ui->tbInfo->append("Тест " + QString::number(test+1) + " выполняется...");
//    ui->tbInfo->append("Азимут: " + QString::number(tests[test].az) + " Угол места: "
//                       + QString::number(tests[test].el) + " Поляризация: "
//                       + QString::number(tests[test].pol));

//    ca_messages::in::RotateManual command;
//    command.az = tests[test].az;
//    command.el = tests[test].el;
//    command.pol = tests[test].pol;
//    ca_messages::Message message;
//    message.append(reinterpret_cast<char*>(&command), ca_messages::sizeOfRotateManual);
//    emit messageReady(message);

////    qDebug() << "rotate manual:" << command.az << command.el << command.pol;
////    state = TestOpuState::WAIT_START_TEST;
//}



void FileTest::on_bStart_clicked()
{
    ui->bStop->setEnabled(true);
    ui->bStart->setEnabled(false);
    ui->bOpenFile->setEnabled(false);
    statusTimer.start(500);
}

//void FileTest::on_bStop_clicked()
//{
//    restart();
//}

//void FileTest::startTest()
//{
//    statusTimer.start(500);

//    ca_messages::in::StopMove command;
//    ca_messages::Message message;
//    message.append(reinterpret_cast<char*>(&command), ca_messages::sizeOfStopMove);
//    emit messageReady(message);

//    state = file_test::TestOpuState::WAIT_STOP_MOVE;

//    startFlag = true;

//    ui->tbInfo->clear();
//    ui->lSuccess->setText("");
//}


