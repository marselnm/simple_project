#ifndef CA_FILETESTOPUDIALOG_H
#define CA_FILETESTOPUDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QFile>
#include <QSettings>

#include "struct_cmd.h"

#define AZ_SPEED 1
#define EL_SPEED 1

namespace file_test {
enum class TestOpuState
{
    BEGIN,
    RUN_TEST,
    TEST_OK,
    TEST_FALSE,
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
    void StopTests();
    void PutInfoRUN_TEST(int currentTest);
    void PutInfoOK_FALSE(int currentTest, int ok_or_false);

public slots:
    void GetStatusFotTesting(QByteArray *AnsData);

private slots:
    void on_bOpenFile_clicked();
    void MainTest();
    void OneTest();
    void on_bStart_clicked();
    void read_status();
    void on_bStop_clicked();

signals:
    void GetStatus();
    void sigSetPosition(QByteArray *position);
    void sigCheckBoxTrue();
    void sig_stop_move_all_drives();

private:
    Ui::FileTest *ui;
    QVector<file_test::TestOpu> tests;
    QVector<file_test::TestOpu> readFile(QString title);
    bool checkValues(file_test::TestOpu test);
    cmd_ans_status_t cmd_ans_status_2;
    QTimer statusTimer;
    file_test::TestOpuState state;
    int currentTest;
    QTimer startTest;
    QTimer oneTest;
    int successTest;
    int falseTest;
    float az_speed;
    float el_speed;

    static constexpr float minAz = -180.;
    static constexpr float maxAz = 180.;

    static constexpr float minEl = -54.;
    static constexpr float maxEl = 90.;

    static constexpr float minPol = -180.;
    static constexpr float maxPol = 180.;

    constexpr static float errorAngle = 0.3;

    bool checkPosition = false;

    QTimer waitstartTimer;
    QTimer testTimer;
};

#endif // CA_FILETESTOPUDIALOG_H









