//
// Created by Sevi Suonenlahti on 3.9.2020.
//

#include "sync_manager.h"

sync_manager::sync_manager(QWidget *w_parent) {
    QPalette palette;

    mainwindow = (UI_Mainwindow *) w_parent;
    dock = new QDockWidget("Sync the runs", w_parent);
    dock->setAllowedAreas( Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable| QDockWidget::DockWidgetFloatable);
    //Qt::LeftDockWidgetArea |
    //   dock->setAttribute(Qt::WA_TranslucentBackground);
    //  dock->setWindowFlags(Qt::FramelessWindowHint);
    dock->setMinimumHeight(250);
    dock->setMaximumHeight(250);
    dock->setMinimumWidth(400);

    auto color = mainwindow->maincurve->backgroundcolor;
    // color.setAlpha(10);
    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, color);// try to make the window transparent
    dock->setAutoFillBackground(true);//this is the only thing that makes the background appeat at all for somereason
    dock->setPalette(palette);
    //  dock->setWindowOpacity(0.2); -> should be something like this but autofill kills it

    SyncDialog = new QDialog(w_parent);


    label2 = new QLabel(SyncDialog);
    label2->setGeometry(10, 40, 300, 25);
    label2->setText("Synchronize the EMG data with the Gyroscopes");

    AddButtonBack = new QPushButton(SyncDialog);
    AddButtonBack->setGeometry(10, 150, 100, 25);
    AddButtonBack->setText("Back");

    AddButtonCross = new QPushButton(SyncDialog);
    AddButtonCross->setGeometry(10, 200, 150, 25);
    AddButtonCross->setText("Synchronize");


    totOffSet1 = new QLabel(SyncDialog);
    totOffSet1->setGeometry(100, 70, 250, 25);

    label3 = new QLabel(SyncDialog);
    label3->setGeometry(10, 100, 250, 25);
    // label3->setText("Click and drag the blue line");
    label3->setText("Then select from the drop down menu, sync by crosshairs");


    label4 = new QLabel(SyncDialog);
    label4->setGeometry(10, 130, 300, 25);
    //  label4->setText("Note - The blue line will be moved to the red line");
    label4->setText("To synchronize right click the curve or label on the left side to enable crosshairs");

    QObject::connect(AddButtonBack, SIGNAL(clicked()),this, SLOT(openImporter()));
       QObject::connect(AddButtonCross, SIGNAL(clicked()), this, SLOT(doSyncStuff()));
    connect(mainwindow, SIGNAL(syncByCrosshairs()), this, SLOT(itsHappening()));//to find out every time the sync by crosshairs has been called
    //use similar method to emit signal from here to simulate click in mainwindow


    dock ->setWidget(SyncDialog);
    dock->show();


    crossHairs();

}

void sync_manager::openImporter()
{
    SyncDialog->close();
    dock->hide();
    mainwindow->UI_import_data->ImportDialog->show();
//    mainwindow->import_gyrodata->ImportDialog->show();
    //   ImportDialog->show();
    mainwindow->Escape_fun();
}

void sync_manager::fileList()
{
    filelist->clear();
    for(i=0; i<mainwindow->files_open; i++){
        new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);}
    //maybe find way to remove main file from the lists-> look for file that has annotation and ignore that one?
}

void sync_manager::itsHappening()
{
    crosshairOffset = mainwindow->maincurve->crosshair_1.edf_hdr->viewtime - mainwindow->maincurve->crosshair_2.edf_hdr->viewtime;
    totalOffset = crosshairOffset - (initialOffset / 0.0000001);
    totalOffset = totalOffset * 0.0000001;

    QString printOffset2 = QString("The total offset is: %1 seconds").arg(totalOffset);
    totOffSet1->setText(printOffset2);

    if(totalOffset != 0) {
        std::ofstream myfile;
        myfile.open("testDoc1.txt",
                    std::ios::out | std::ios::app);//std::ios::trunc at first, need to append not truncate tho
        myfile << "?" << totalOffset << "\n";
        myfile.close();
    }
    //need to print this offset to the doc -> use it in python
}
void sync_manager::doSyncStuff()
{
    mainwindow->sync_by_crosshairs();
    itsHappening();

  //  crossHairs();
}


void sync_manager::crossHairs()
{
    int x,q=0;
    if(!mainwindow->files_open)  return;
    if(!mainwindow->signalcomps)  return;


    for(i=0; i<mainwindow->signalcomps; i++) {
        //x = mainwindow->signalcomp[i]->filenum;
        //    x = mainwindow->get_filenum(mainwindow->signalcomp[i]);


        x = mainwindow->get_filenum(mainwindow->signalcomp[i]->edfhdr);

        q++;
        if(x != 0)
        {
                      mainwindow->maincurve->exec_sidemenu(x-1); // find  away to not be which main curve but which file->>
                      mainwindow->maincurve->sidemenuButton2->click();

                   mainwindow->maincurve->exec_sidemenu(q-1); // find  away to not be which main curve but which file->>
                   mainwindow->maincurve->sidemenuButton2->click();
            break;
        }
    }
    if (mainwindow->maincurve->crosshair_1.active &&mainwindow->maincurve->crosshair_2.active)
    {    //important check or it will crash constantly
       // int filen = mainwindow->get_filenum(mainwindow->signalcomp[i]->edfhdr);
        if (mainwindow->viewtime_sync != VIEWTIME_USER_DEF_SYNCED) {
             // initialOffset = mainwindow->edfheaderlist[filen]->viewtime -
            //                  mainwindow->edfheaderlist[mainwindow->maincurve->crosshair_2.file_num]->viewtime;

            initialOffset = mainwindow->maincurve->crosshair_1.edf_hdr->viewtime - mainwindow->maincurve->crosshair_2.edf_hdr->viewtime;
            initialOffset = initialOffset * 0.0000001; //this is causing the crash
        }
    }
      itsHappening();
}

//enable the crosshairs immidiatly, then use them to find the timings aswell,
//use it to move the entire file instead of just a channel

//make a function that will press the sync button, use that to constantly track the timings of the files

sync_manager::~sync_manager()
{
    SyncDialog->close();
    mainwindow->Escape_fun();
    delete SyncDialog;
}