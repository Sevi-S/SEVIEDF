//
// Created by Sevi Suonenlahti on 3.9.2020.
//

#include "createGraphs.h"

createGraphs::createGraphs(QWidget *w_parent) {
    QPalette palette;

    counter = 0;
    mainwindow = (UI_Mainwindow *) w_parent;

    //temp stuff for now
    dock = new QDockWidget("generate the graphs for your tremors here", w_parent);
    dock->setAllowedAreas( Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable| QDockWidget::DockWidgetFloatable);
    //Qt::LeftDockWidgetArea |
    //   dock->setAttribute(Qt::WA_TranslucentBackground);
    //  dock->setWindowFlags(Qt::FramelessWindowHint);
    dock->setMinimumHeight(250);
    dock->setMaximumHeight(300);
    dock->setMinimumWidth(500);


    auto color = mainwindow->maincurve->backgroundcolor;
    // color.setAlpha(10);
    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, color);// try to make the window transparent
    dock->setAutoFillBackground(true);//this is the only thing that makes the background appeat at all for somereason
    dock->setPalette(palette);

    // AddButtonCreate = new QPushButton(dock);
    // AddButtonCreate->setGeometry(10, 100, 100, 25);
    // AddButtonCreate->setText("Create");

    AddButtonCross = new QPushButton(dock);
    AddButtonCross->setGeometry(10, 150, 100, 25);
    AddButtonCross->setText("Select Area");

    AddButtonGet = new QPushButton(dock);
    AddButtonGet->setGeometry(10, 180, 100, 25);
    AddButtonGet->setText("Confirm");

    //  QObject::connect(AddButtonCreate,SIGNAL(clicked()),this, SLOT(testFunc()));
    QObject::connect(AddButtonCross,SIGNAL(clicked()),this, SLOT(selectArea()));
    QObject::connect(AddButtonGet,SIGNAL(clicked()),this, SLOT(getArea()));

    dock->show();
}

void createGraphs::selectArea()
{
    int p,i,z=0;
    if(!mainwindow->files_open)  return;
    if(!mainwindow->signalcomps)  return;

    for(i=0; i<mainwindow->signalcomps; i++)
    {
        p = mainwindow->get_filenum(mainwindow->signalcomp[i]->edfhdr);
        int sf = mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].sf_int;
        if (sf == 200)
        {

            mainwindow->maincurve->exec_sidemenu(z); // find  away to not be which main curve but which file->>
                mainwindow->maincurve->sidemenuButton2->click();

            mainwindow->maincurve->exec_sidemenu(z+1); //(changed from z-1 to p, so that the crosshairs will now be in the same file -> can get begingin and end data for the tremor)
                 mainwindow->maincurve->sidemenuButton2->click();
            break;
        }
        else z++;
    }
}

void createGraphs::getArea()
{
    long double pos1 = (mainwindow->maincurve->crosshair_1.time_relative)* 0.0000001;
    long double pos2 = (mainwindow->maincurve->crosshair_2.time_relative)* 0.0000001;    //THIS IS THE MAIN CROSSHAIR
    long double EmgCrossLocInTimeStr = 0.1;
    long double EmgCrossLocInTimeEnd = 0.1;
    //holy shit - we need the crosshair values for both crosshairs for both files...
    //convert emg time to gyrotime.. and get the values
    //send over pathinfo and names etc.. from import dat file

    int sf = 0;
    for(int i=0; i<mainwindow->signalcomps; i++)
    {
        sf = mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].sf_int;
        int filen = mainwindow->get_filenum(mainwindow->signalcomp[i]->edfhdr);
        long double strT = 0.1;
        long double gryoViTime = 0.1;
        if (sf ==256)//hard coded 256 for EMG sample rate, will change to !200 if the sample rate gets changed
        {
            strT = (mainwindow->edfheaderlist[filen]->viewtime) * 0.0000001;//convert the view time into sample number, then add the number of samples between the original targets->profit

            if(filen == 0)
            {
                gryoViTime = (mainwindow->edfheaderlist[filen+1]->viewtime)* 0.0000001;
            }
            else if(filen <= 1)
            {
                gryoViTime = (mainwindow->edfheaderlist[filen-1]->viewtime)* 0.0000001;
            }

            long double crossHLocInTimeOnScreen = pos1 - gryoViTime;
            long long winSize = pos2 - pos1;// put in directly

            EmgCrossLocInTimeStr = strT + crossHLocInTimeOnScreen;
            EmgCrossLocInTimeEnd = EmgCrossLocInTimeStr + winSize;

            break;
        }

    }

    counter = counter +1;

    std::ofstream myfile;
    myfile.open (mainwindow->UI_import_data->saveFile, std::ios::out|std::ios::app);//std::ios::trunc at first, need to append not truncate tho
    //use app instead of trunc

    myfile << "@"<< "Tremor number: " << counter<<"\n";
    myfile << "$"<<pos1 << ", " << pos2<<"\n";
    myfile << "!"<< EmgCrossLocInTimeStr << ", " << EmgCrossLocInTimeEnd <<"\n"; //put start time and end time here, remeber to uncomment the final print

    myfile.close();

    mainwindow->Escape_fun();

    mainwindow->startgyroanalysis = new startGyroAnalysis(mainwindow);
    printf("%s",mainwindow->UI_import_data->saveFile);
    mainwindow->startgyroanalysis->ParseSave(mainwindow->UI_import_data->saveFile,0);

}

createGraphs::~createGraphs()
{
    dock->close();
    delete dock;
}