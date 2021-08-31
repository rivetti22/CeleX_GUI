#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPixmap>
#include <QImage>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <celex5/celex5datamanager.h>
#include <celex5/celex5processeddata.h>

#define MAT_COLS 1280
#define MAT_ROWS 800
#define FPN_PATH "../../../Samples/config/FPN_2.txt"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public CeleX5DataManager
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    qint8 counter = 5;
    bool recording = false;                                 //Start recording flag
    std::vector<std::vector<EventData> > recVec;            //Vector that stores event information
    int SKIP_INTERVAL = 2;

    //CELEX
    CX5SensorDataServer *m_pServer;

public slots:
    void updateCountDown();

private slots:
    void on_recordButton_clicked();
    void saveWait();

    //CELEX
    virtual void onFrameDataUpdated(CeleX5ProcessedData* pSensorData);//overrides Observer operation

    void on_downButton_clicked();
    void on_playButton_clicked();

    void on_everCheckBox_clicked();

    void on_windCheckBox_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *saveTimer;

    //CELEX
    CeleX5 *pCeleX5 = new CeleX5;

    QImage qtEvMat;

};
#endif // MAINWINDOW_H
