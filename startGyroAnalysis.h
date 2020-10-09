//
// Created by Sevi Suonenlahti on 3.9.2020.
//

#ifndef EDFBROWSER_MASTER_STARTGYROANALYSIS_H
#define EDFBROWSER_MASTER_STARTGYROANALYSIS_H

#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QProgressDialog>
#include <QCheckBox>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <QToolTip>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <limits.h>
#include <string>
#include <cstring>

#include <fstream>
#include <iostream>

#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include "global.h"
#include "mainwindow.h"
#include "signalcurve.h"
#include "utils.h"
#include "filter.h"
#include "spike_filter.h"
#include "ravg_filter.h"
#include "flywheel.h"
#include "fft_wrap.h"

#include <fstream>

#include "third_party/fidlib/fidlib.h"

#include "sync_manager.h"
#include "createGraphs.h"

class UI_Mainwindow;

class startGyroAnalysis : public QObject {
Q_OBJECT
public:
    startGyroAnalysis(QWidget *w_parent);


    UI_Mainwindow *mainwindow;

    ~startGyroAnalysis();

    void loadData();
    void ParseSave(char* filename,int load);

signals:
    void python();
private:

    QWidget *dockWidgetContents;
    QDockWidget* dock;
    QLabel *PateintName;
    QLabel *PatientNumber;
    QLabel *PatientNumber_2;
    QLabel *dateTimeEdit;
    QPushButton *pushButton;
    QPushButton *pushButton1;
    QComboBox *TremorViewer;
    QLabel *StartTime;
    QLabel *EndTime;
    QLabel *startText;
    QLabel *endText;

    int counter;

    std::string patientData;
    std::string patientName;
    std::string patientNumber;
    std::string patientinfo;
    std::string patientDate;
    std::string tremorNum;
    std::string tremorTime;
    std::string tremorStartTime;
    std::string tremorendTime;

    std::string loadFilename;
    char pyfileName[MAX_PATH_LENGTH];
    char pypath[MAX_PATH_LENGTH],pyfile[MAX_PATH_LENGTH],path[MAX_PATH_LENGTH],recent_opendir[MAX_PATH_LENGTH],outputfilename[MAX_PATH_LENGTH],fullSavePath[MAX_PATH_LENGTH];

private slots:

    void sendPythonSig();
    void printGraphs();
    void backToCrosshairs();
    //  void selectArea();
    //   void getArea();

public slots:


};


#endif//EDFBROWSER_MASTER_STARTGYROANALYSIS_H