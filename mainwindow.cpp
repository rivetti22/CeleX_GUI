#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSound>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    timer = new QTimer(this);
    saveTimer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateCountDown()));
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(saveWait()));

    //CELEX
    if (NULL == pCeleX5)
        return;

    pCeleX5->openSensor(CeleX5::CeleX5_MIPI);
    pCeleX5->setFpnFile(FPN_PATH);
    pCeleX5->setSensorFixedMode(CeleX5::Event_In_Pixel_Timestamp_Mode);
    pCeleX5->disableFrameModule();
    pCeleX5->disableIMUModule();
    pCeleX5->disableEventCountSlice();
    pCeleX5->enableEventDenoising();

    m_pServer = pCeleX5->getSensorDataServer();
    m_pServer->registerData(this, CeleX5DataManager::CeleX_Frame_Data);

}

MainWindow::~MainWindow()
{
    delete pCeleX5;
    pCeleX5 = NULL;
    delete ui;
}

void MainWindow::updateCountDown()
{
    if (counter > 0){
        ui->countDown->setText(QString("%1").arg(counter));
        if (counter == 5) QSound::play("../GUI/countdown.wav");
        counter--;
    }
    else{
        ui->countDown->setStyleSheet("color: red; border: 2px solid red; border-radius: 20px;");
        ui->countDown->setText(QString("Recording"));
        ui->recordButton->setText(QString("STOP RECORDING"));
        recording = true;
        timer->stop();
        counter = 5;
        ui->recordButton->setEnabled(true);
    }

}

void MainWindow::on_recordButton_clicked()
{
    if (ui->filename->text() != NULL){
        if (!recording){
            timer->start(1000);
            ui->recordButton->setEnabled(false);
        }
        else{
            //Reset flags
            QSound::play("../GUI/censor_beep.wav");
            recording = false;
            ui->stackedWidget->setCurrentIndex(1);

            //Timer that triggers the storage of event data in text file
            saveTimer->start(1000);
        }
    }
}

void MainWindow::saveWait()
{
    saveTimer->stop();

    //Write file
    ui->savingLabel->setText(QString("Saving file..."));
    ui->savingLabel->repaint();

    std::ofstream file;
    file.open ("../../Recordings/Original/" + ui->filename->text().toStdString() + ".txt", std::ios::out);

    for (int i = 0; i < recVec.size() ; i++) {
        for (int j = 0; j < recVec[i].size() ; j++) {
            file << recVec[i][j].tInPixelIncreasing << "\t" << MAT_COLS - recVec[i][j].col - 1 << "\t" << MAT_ROWS - recVec[i][j].row - 1 << std::endl;
        }
    }

    file.close();

    //Clean vector of events
    recVec.clear();

    //Return to first page
    ui->stackedWidget->setCurrentIndex(0);

    //Update GUI
    ui->countDown->setStyleSheet("color: white; border: 2px solid white; border-radius: 20px;");
    ui->countDown->setText(QString("Not Recording"));
    ui->recordButton->setText(QString("RECORD"));
}

void MainWindow::onFrameDataUpdated(CeleX5ProcessedData *pSensorData)
{
    if(NULL == pSensorData) return;

    //Capture buffer of events
    cv::Mat evMat = cv::Mat::zeros(cv::Size(MAT_COLS, MAT_ROWS), CV_8UC1);
    std::vector<EventData> vecEvent;
    pCeleX5->getEventDataVector(vecEvent);
    int datasize = vecEvent.size();

    //Parsing of event information
    for (int i = 0; i < datasize; i++)
    {
        evMat.at<uchar>(MAT_ROWS - vecEvent[i].row - 1, MAT_COLS - vecEvent[i].col - 1) = 255;
    }

    if (datasize > 0){
        //Convert to QImage
        qtEvMat = QImage((const unsigned char*) (evMat.data), evMat.cols, evMat.rows, QImage::Format_Grayscale8); //QImage::Format_RGB888

        //Show image
        ui->frame->setPixmap(QPixmap::fromImage(qtEvMat));
        ui->frame->resize(ui->frame->pixmap()->size());

        if (recording) recVec.push_back(vecEvent);
    }
}

void MainWindow::on_downButton_clicked()
{
    if (ui->filename->text() != NULL && (ui->everCheckBox->isChecked() || ui->windCheckBox->isChecked())){
        std::stringstream ss;
        if (ui->everCheckBox->isChecked()) ss << "gnome-terminal -- /bin/sh -c 'cd ../../Downsampling/build-downsampling-Desktop_Qt_5_15_2_GCC_64bit-Debug ; ./downsampling " <<
                                                 ui->filename->text().toStdString() + ".txt everyI" << " ; sleep 5 ; exit ; exec bash'";
        else if (ui->windCheckBox->isChecked()) ss << "gnome-terminal -- /bin/sh -c 'cd ../../Downsampling/build-downsampling-Desktop_Qt_5_15_2_GCC_64bit-Debug ; ./downsampling " <<
                                                 ui->filename->text().toStdString() + ".txt window" << " ; sleep 5; exit ; exec bash'";
        system(ss.str().c_str());
    }
}

void MainWindow::on_playButton_clicked()
{
    if (ui->filename->text() != NULL){
        std::stringstream ss;
        ss << "gnome-terminal -- /bin/sh -c 'cd ../../DataPlayback/build-dataPlayback-Desktop_Qt_5_15_2_GCC_64bit-Debug ; ./dataPlayback " <<
              ui->filename->text().toStdString() + ".txt" << " ;  sleep 5 ; exit ; exec bash'";
        system(ss.str().c_str());
    }
}

void MainWindow::on_everCheckBox_clicked()
{
    ui->windCheckBox->setChecked(false);
}

void MainWindow::on_windCheckBox_clicked()
{
    ui->everCheckBox->setChecked(false);
}
