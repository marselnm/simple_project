#ifndef CA_FILETESTOPUDIALOG_H
#define CA_FILETESTOPUDIALOG_H

#include <QDialog>
#include <QTimer>

#include "struct_cmd.h"

#define AZ_SPEED 5
#define EL_SPEED 5

namespace file_test {
enum class TestOpuState
{
    GET_STATUS,//запросить статус антенны



    WAIT_STOP_MOVE,
    WAIT_STOP_STATUS,
    WAIT_START_TEST,
    RUN_TEST,
    FINISH_TEST,
    STOP_TEST,
    NOP//ничего не делать
};

struct TestOpu
{
    float az;
    float el;
    float pol;
    TestOpu() {}
};
}

namespace Ui {
class FileTest;
}

class FileTest : public QDialog
{
    Q_OBJECT

public:
    explicit FileTest(QWidget *parent = nullptr);
    ~FileTest();
    void closeEvent(QCloseEvent *event);
    void SendPosition(float AZ, float EL);



public slots:
    void GetStatusFotTesting(QByteArray *AnsData);
    //void newStatus(const ca_messages::out::Status& status);
    //void newOk(const ca_messages::out::Ok &ok);
    //void newError(const ca_messages::out::Error &error);

private slots:
    void on_bOpenFile_clicked();
    void MainTest();
    //void on_bStart_clicked();
    //void on_bStop_clicked();

    void on_bStart_clicked();
    void read_status();

signals:
    void GetStatus();
    void sigSetPosition(QByteArray *position);

private:
    Ui::FileTest *ui;
    QVector<file_test::TestOpu> tests;
    QVector<file_test::TestOpu> readFile(QString title);
    bool checkValues(file_test::TestOpu test);
    cmd_ans_status_t cmd_ans_status_2;
    QTimer statusTimer;
    file_test::TestOpuState state;
    int currentTest = 0;
    QTimer startTest;

    static constexpr float minAz = -180.;
    static constexpr float maxAz = 180.;

    static constexpr float minEl = -54.;
    static constexpr float maxEl = 90.;

    static constexpr float minPol = -180.;
    static constexpr float maxPol = 180.;

    bool startFlag = false;
    //ca_messages::out::Position position;
    //file_test::TestOpuState state;


    constexpr static float errorAngle = 0.3;
    constexpr static uint waittime = 500;
    constexpr static uint waitStartTime = 10;

    bool checkPosition = false;

    QTimer waitstartTimer;

    QTimer testTimer;

    void errorTest();
    void restart();
    void updateResult(uint test, bool result);
    void runTest(uint test);


    uint successTest = 0;
};

#endif // CA_FILETESTOPUDIALOG_H
