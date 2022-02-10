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
    az_speed = 1.0;
    el_speed = 1.0;
    ui->lineEdit->setText(QString::number(static_cast<double>(az_speed)));
    ui->lineEdit_2->setText(QString::number(static_cast<double>(el_speed)));

    connect(&startTest, &QTimer::timeout, this, &FileTest::MainTest);
    connect(&oneTest, &QTimer::timeout, this, &FileTest::OneTest);
}

FileTest::~FileTest()
{
    delete ui;
}

void FileTest::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    startTest.stop();
    oneTest.stop();
    ui->tbInfo->clear();
    emit sigCheckBoxTrue();
    emit sig_stop_move_all_drives();
}

void FileTest::SendPosition(float AZ, float EL)
{
    QByteArray Buffer;

    cmd_set_position_t cmd_set_position;
    cmd_set_position.Lenght = sizeof (cmd_set_position_t);
    cmd_set_position.Message_ID = CMD_SET_POSITION;
    cmd_set_position.AZ_Speed = az_speed;
    cmd_set_position.AZ_Position = AZ;
    cmd_set_position.EL_Speed = el_speed;
    cmd_set_position.EL_Position = EL;

    Buffer.append(reinterpret_cast<const char*>(&cmd_set_position), sizeof(cmd_set_position_t));

    emit sigSetPosition(&Buffer);
}

void FileTest::StopTests()
{
    ui->tbInfo->append("Тестирование завершено");
    ui->bStop->setEnabled(false);
    ui->bStart->setEnabled(true);
    ui->bOpenFile->setEnabled(true);
}

void FileTest::PutInfoRUN_TEST(int currentTest)
{
    ui->tbInfo->append("Тест " + QString::number(currentTest+1) + " выполняется...");
    ui->tbInfo->append("Азимут: " + QString::number(static_cast<double>(tests[currentTest].az))
                       + " Угол места: " + QString::number(static_cast<double>(tests[currentTest].el)));
}

void FileTest::PutInfoOK_FALSE(int currentTest, int ok_or_false)
{
    if(ok_or_false == 1)
    {
        ui->tbInfo->append("Тест " + QString::number(currentTest+1) + "<font color='green'> выполнен успешно");
    }else {
        ui->tbInfo->append("Тест " + QString::number(currentTest+1) + "<font color='red'> не выполнен ");
    }
}

void FileTest::MainTest()
{
    switch (state)
    {
        case file_test::TestOpuState::BEGIN:
        {
            emit GetStatus();
            break;
        }

        case file_test::TestOpuState::RUN_TEST:
        {
            PutInfoRUN_TEST(currentTest);

            float newAZ = tests.at(currentTest).az;
            float newEL = tests.at(currentTest).el;

            int ErrorAZ = static_cast<int>(abs(cmd_ans_status_2.CurrentAZ - tests.at(currentTest).az));
            int ErrorEL = static_cast<int>(abs(cmd_ans_status_2.CurrentEL - tests.at(currentTest).el));

            int maxError = 0;

            if(ErrorAZ >= ErrorEL)
            {
                maxError = ErrorAZ;
            }else {
                maxError = ErrorEL;
            }

            int speed = 0;

            if(az_speed >= el_speed)
            {
                speed = static_cast<int>(az_speed);
            }else {
                speed = static_cast<int>(el_speed);
            }

            int time = (maxError / speed) + 5;

            SendPosition(newAZ, newEL);
            state = file_test::TestOpuState::NOP;
            oneTest.start(time*1000);

            break;
        }

        case file_test::TestOpuState::TEST_OK:
        {
            PutInfoOK_FALSE(currentTest, 1);
            currentTest++;
            successTest++;
            ui->lSuccess->setText(QString::number(successTest));

            if(currentTest == tests.size())
            {
                state = file_test::TestOpuState::STOP_TEST;
            }else {
                state = file_test::TestOpuState::RUN_TEST;
            }
            break;
        }

        case file_test::TestOpuState::TEST_FALSE:
        {
            PutInfoOK_FALSE(currentTest, 0);
            currentTest++;
            falseTest++;

            if(currentTest == tests.size())
            {
                state = file_test::TestOpuState::STOP_TEST;
            }else {
                state = file_test::TestOpuState::RUN_TEST;
            }
            break;
        }

        case file_test::TestOpuState::STOP_TEST:
        {
            startTest.stop();
            StopTests();
            break;
        }

        case file_test::TestOpuState::NOP:
        {
            emit GetStatus();
            break;
        }
    }
}

void FileTest::OneTest()
{
    oneTest.stop();

    double ErrorAZ = fabs(static_cast<double>(cmd_ans_status_2.CurrentAZ - tests.at(currentTest).az));
    double ErrorEL = fabs(static_cast<double>(cmd_ans_status_2.CurrentEL - tests.at(currentTest).el));

    if(ErrorAZ <= 0.3 && ErrorEL <= 0.3)
    {
        state = file_test::TestOpuState::TEST_OK;
    }else {
        state = file_test::TestOpuState::TEST_FALSE;
    }
}

void FileTest::on_bOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Выбрать файл тестов", QDir::homePath(), "*.csv");
    ui->leFileName->setText(fileName);

    if(fileName.isEmpty())
        return;

    tests = readFile(fileName);
    ui->lNumberTest->setText(QString::number(tests.size()));

    ui->lSuccess->setText(QString::number(0));

    ui->tbInfo->clear();

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

    if(cmd_ans_status_2.StateDriveAZ == 0 && cmd_ans_status_2.StateDriveEL == 0 && state == file_test::TestOpuState::BEGIN)//если привода остановлены
    {
        state = file_test::TestOpuState::RUN_TEST;
    }
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

void FileTest::on_bStart_clicked()
{
    ui->bStop->setEnabled(true);
    ui->bStart->setEnabled(false);
    ui->bOpenFile->setEnabled(false);
    successTest = 0;
    ui->lSuccess->setText(QString::number(successTest));
    falseTest = 0;
    currentTest = 0;
    ui->tbInfo->clear();

    az_speed = ui->lineEdit->text().toFloat();
    el_speed = ui->lineEdit_2->text().toFloat();

    state = file_test::TestOpuState::BEGIN;
    startTest.start(1000);
}

void FileTest::on_bStop_clicked()
{
    startTest.stop();
    oneTest.stop();
    ui->bStop->setEnabled(false);
    ui->bStart->setEnabled(true);
    ui->bOpenFile->setEnabled(true);
    emit sig_stop_move_all_drives();
}
