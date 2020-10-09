//
// Created by Sevi Suonenlahti on 3.9.2020.
//

#include "startGyroAnalysis.h"

startGyroAnalysis::startGyroAnalysis(QWidget *w_parent) {
    QPalette palette;

    mainwindow = (UI_Mainwindow *) w_parent;

    //temp stuff for now
    dock = new QDockWidget("Tremor Data", w_parent);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    //Qt::LeftDockWidgetArea |
    //   dock->setAttribute(Qt::WA_TranslucentBackground);
    //  dock->setWindowFlags(Qt::FramelessWindowHint);
    dock->setMinimumHeight(250);
    dock->setMaximumHeight(300);
    dock->setMinimumWidth(800);


    auto color = mainwindow->maincurve->backgroundcolor;
    // color.setAlpha(10);
    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, color);// try to make the window transparent
    dock->setAutoFillBackground(true);//this is the only thing that makes the background appeat at all for somereason
    dock->setPalette(palette);


    //  dock = new QWidget();
    //dock->setObjectName(QString::fromUtf8("dock"));
    PateintName = new QLabel(dock);
    PateintName->setObjectName(QString::fromUtf8("PateintName"));
    PateintName->setGeometry(QRect(430, 30, 121, 15));
    PatientNumber = new QLabel(dock);
    PatientNumber->setObjectName(QString::fromUtf8("PatientNumber"));
    PatientNumber->setGeometry(QRect(430, 50, 121, 15));
    PatientNumber_2 = new QLabel(dock);
    PatientNumber_2->setObjectName(QString::fromUtf8("PatientNumber_2"));
    PatientNumber_2->setGeometry(QRect(430, 20, 200, 150));
    dateTimeEdit = new QLabel(dock);
    dateTimeEdit->setObjectName(QString::fromUtf8("dateTimeEdit"));
    dateTimeEdit->setGeometry(QRect(430, 150, 190, 25));
    pushButton = new QPushButton(dock);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setGeometry(QRect(430, 170, 150, 30));
    pushButton1 = new QPushButton(dock);
    pushButton1->setObjectName(QString::fromUtf8("pushButton1"));
    pushButton1->setGeometry(QRect(430, 210, 150, 30));
    TremorViewer = new QComboBox(dock);
    // TremorViewer->addItem(QString());
    // TremorViewer->addItem(QString());
    TremorViewer->setObjectName(QString::fromUtf8("TremorViewer"));
    TremorViewer->setGeometry(QRect(220, 30, 170, 30));
    StartTime = new QLabel(dock);
    StartTime->setObjectName(QString::fromUtf8("StartTime"));
    StartTime->setGeometry(QRect(310, 60, 111, 16));
    EndTime = new QLabel(dock);
    EndTime->setObjectName(QString::fromUtf8("EndTime"));
    EndTime->setGeometry(QRect(310, 100, 111, 16));
    startText = new QLabel(dock);
    startText->setObjectName(QString::fromUtf8("startText"));
    startText->setGeometry(QRect(230, 60, 111, 16));
    endText = new QLabel(dock);
    endText->setObjectName(QString::fromUtf8("endText"));
    endText->setGeometry(QRect(230, 100, 111, 16));
    //DockWidget->setWidget(dock);

    PateintName->setText("PatientName");
    PatientNumber->setText("PatientNum");
    PatientNumber_2->setText("Patient INFO");
    pushButton->setText("Create Graphs");
    pushButton1->setText("Select Area");
    //TremorViewer->setItemText(0,"Tremor 1");
    //TremorViewer->setItemText(1, "Tremor 2");

    startText->setText("Start Time = ");
    endText->setText("End Time = ");


    QObject::connect(pushButton,SIGNAL(clicked()),this, SLOT(printGraphs()));
    QObject::connect(pushButton1,SIGNAL(clicked()),this, SLOT(backToCrosshairs()));

    //ParseSave();
    dock->show();

}

void startGyroAnalysis::sendPythonSig()
{
    emit mainwindow->python();
}


void startGyroAnalysis::ParseSave(char* filename,int load)
{
    if(!loadFilename.size())
    {
        loadFilename = "testDoc1.txt";
    }

   // std::ifstream inFile;

 //   if(!loadFilename.size())
    //{
   //     std::ifstream inFile("testDoc1.txt");
  //  }
   // else
    //    {
            std::ifstream inFile(filename);
    //    }

    if (inFile.is_open()) {
        std::string line;
        int namecount = 0;
        int numcount = 0;
        int infocount = 0;
        int tremorNumCont = 0;
        int timeCont = 0;
        std::string pathsave;
        std::string pathpytfile;
        while (std::getline(inFile, line))
        {
            if(line.at(0)=='&')
            {
                patientData = line;
                for (int i = 0; i < patientData.length(); i++)
                {
                    patientData.at(i) = patientData.at(i+1);//remove the delimter

                    if(patientData.at(i)==',')
                    {
                        patientName.assign(patientData,0,namecount);
                        break;
                    }
                    else
                    {
                        namecount++;
                    }
                }
                for (int i = namecount + 1; i < patientData.length(); i++)
                {
                    patientData.at(i) = patientData.at(i+1);//remove the delimter

                    if(patientData.at(i)==',')
                    {
                        patientNumber.assign(patientData,namecount+2,numcount-1);
                        break;
                    }
                    else
                    {
                        numcount++;
                    }
                }
                for (int i = numcount + namecount + 2; i < patientData.length(); i++)
                {
                    patientData.at(i) = patientData.at(i+1);//remove the delimter

                    if(patientData.at(i)==',')
                    {
                        patientinfo.assign(patientData,namecount + numcount+3,infocount-1);
                        break;
                    }
                    else
                    {
                        infocount++;
                    }
                }
                patientDate.assign(patientData,namecount + numcount+ infocount +5,patientData.length()-(namecount + numcount+ infocount +3));
            }
            else if(line.at(0)=='@')
            {
                //  TremorViewer->addItem(QString());
                tremorNum = line;
                for (int i = 0; i < tremorNum.length(); i++)
                {
                    tremorNum[i] = tremorNum[i+1];
                }
                TremorViewer->addItem(QString(QString::fromStdString(tremorNum)));
                //   TremorViewer->setItemText(tremorNumCont+1,QString::fromStdString(tremorNum));
                //    TremorViewer->setItemText(tremorNumCont+1,"apples");
                tremorNumCont++;
            }
            else if(line.at(0)=='$')//needs to be ! in the future
            {
                tremorTime = line;
                // tremorStartTime = std::stoi(tremorTime[1]) when i make int list
                for (int i = 0; i < tremorTime.length(); i++)
                {
                    if(tremorTime.at(i)==',')
                    {
                        break;
                    }
                    timeCont++;
                }
                tremorStartTime.assign(tremorTime,1,timeCont-1);
                tremorendTime.assign(tremorTime,timeCont+1,tremorTime.length()-timeCont);
            }
           else if(line.at(0)=='%')//load the path files
            {
                pathsave = line;
                for (int i = 0; i < pathsave.length(); i++) {
                    pathsave[i] = pathsave[i+1];//remove the delimter
                }

                if(load == 1)
                {mainwindow->load_file_from_save(pathsave.c_str());}
            }
            else if(line.at(0)=='#')//load the path files
            {
                pathpytfile = line;
                for (int i = 0; i < pathpytfile.length(); i++) {
                    pathpytfile[i] = pathpytfile[i+1];//remove the delimter
                }

               strlcat(pyfileName,pathpytfile.c_str(),pathpytfile.length()+1); //pyfileName.assign(pathpytfile,1,pathpytfile.length()-1);
            }


            // using printf() in all tests for consistency
            // printf("%s\n", line.c_str());
        }

        inFile.close();
    }

    PateintName->setText(QString::fromStdString(patientName));
    PatientNumber->setText(QString::fromStdString(patientNumber));
    PatientNumber_2->setText(QString::fromStdString(patientinfo));
    dateTimeEdit->setText(QString::fromStdString(patientDate));

    StartTime->setText(QString::fromStdString(tremorStartTime));
    EndTime->setText(QString::fromStdString(tremorendTime));

    //  auto now = QtCore.QDate.fromString(QString::fromStdString(patientDate));
    //  dateTimeEdit.setDate(now);

    //  QString time_format = "yyyy-MM-dd  HH:mm:ss";
    //  QDateTime a = QDateTime::currentDateTime();
    //  QString as = a.toString(time_format);

    dock->update();
}

void startGyroAnalysis::loadData()
{

    strlcpy(path, QFileDialog::getOpenFileName(mainwindow, "Open file", QString::fromLocal8Bit(mainwindow->recent_opendir)).toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(path, ""))
    {
        return;
    }

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);
    get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);

    char ch1 = '/';
    int q = strlen(recent_opendir);
    int p = strlen(outputfilename);
    strlcat(recent_opendir,&ch1,MAX_PATH_LENGTH);
    strlcpy(fullSavePath,recent_opendir,q+2);
    strlcat(fullSavePath, outputfilename, p+q +2);

    ParseSave(fullSavePath,1);

}

void startGyroAnalysis::printGraphs()
{

    strlcpy(pyfile,mainwindow->UI_import_data->pyfilefin,strlen(mainwindow->UI_import_data->pyfilefin)+1);
    strlcpy(pypath,mainwindow->UI_import_data->pypathFin,strlen(mainwindow->UI_import_data->pypathFin)+1);

    //read through margots code these days and figure out the formating
    //these super ghetto methods need to be cleaned up
    //https://www.tutorialspoint.com/cplusplus/cpp_files_streams.htm - to figure out how to formating needs to be done


    //then find a way to read in

    //std::string thisDir= qApp->applicationDirPath();

    std::string filepy(pypath);

    if(strlen(pypath)<2)
    {
        //std::string saveFileTxt(fullSavePath);
        //std::string saveFileTxtwin = std::string(" ") + " \""+saveFileTxt+".txt"+"\"";
       // saveFileTxt = std::string(" ") + saveFileTxt;

       //int x = strlen(pyfileName)+strlen(fullSavePath);

      // char tem = '  ';

       std::string macpath = std::string(pyfileName) + " " + std::string(fullSavePath);
       printf("%s\n",macpath.c_str());

       std::string winpath = " \""+ std::string(pyfileName) +"\" \""+ std::string(fullSavePath) +"\"";
        printf("%s,\n",winpath.c_str());
      //  std::string winpy(pyfileName);
      //  winpy = " \"" + winpy + "\" ";
      //  printf("%s\n",winpy.c_str());
     //   printf("%s\n",saveFileTxtwin.c_str());
   //     strlcat(pyfileName,&tem,strlen(pyfileName)+1);
    //    strlcat(pyfileName,fullSavePath,x+1);

     //   std::string filename(pyfileName);

       // std::string tempfilename = filename;
       // int spacecount;
      /*  for (int i = 0; i < filename.length(); i++)
        {
            if(filename.at(i)=='p')
            {
                if(filename.at(i+1)=='y')
                {
                    filename[i+2] = ' ';
                    for (int i = spacecount; i < filename.length(); i++)
                    {
                        filename[i+3] =tempfilename[i+3];
                    }
                    printf("%s\n",filename.c_str());
                    break;
                }

            }
           spacecount++;
        }
        */
#if defined (Q_OS_MAC)
        std::string command = "python3 ";//use python 3 instead of python.................
        command += macpath;//this right here is key+
    //
        system(command.c_str());

#elif defined (Q_OS_WIN32)
        std::string commands;
        if(pyfile[strlen(pyfile)] == 'x' || pyfile[strlen(pyfile)] == 'e')
                {
                commands = "";
                }
            else
                {
                commands = "python ";
                }
          commands += winpath;

          system(commands.c_str());
#endif
    }
    else
        {
            get_directory_from_path(recent_opendir, mainwindow->UI_import_data->saveFile, MAX_PATH_LENGTH);

            int q = strlen(recent_opendir);
        //    strlcpy(fullSavePath,recent_opendir,q+2);
            // strlcat(fullSavePath, outputfilename, p+q +2);

            std::string saveFileTxt(recent_opendir);

          //  saveFileTxt = std::string(" ") + saveFileTxt;

#if defined (Q_OS_MAC)

        std::string filename = (filepy + "/");

        filename += pyfile;//find a way to run margots code on my own machine

        //std::string filename = "\"C:/Users/Margot Heijmans/windostest3/graphs.py\"";

        //std::string command = "python ";
        //need to import boost::filesystem library to make this work on windows
        // std::string filename = "/Users/Sevi_Pro/Desktop/seviEDFbrow/edfbrowser_170_source/cmake-build-debug/graphs.py";
        std::string command = "python3 ";//use python 3 instead of python.................

        command += filename;        //this right here is key+
        command += " " + saveFileTxt;
        system(command.c_str());

#elif defined (Q_OS_WIN32)
          std::string filenames = (filepy + "\\");
          filenames += pyfile;//find a way to run margots code on my own machine
            std::string commands;
            if(pyfile[strlen(pyfile)] == 'x' ||pyfile[strlen(pyfile)] == 'e')
                {
                commands = "";
                }
            else
                {
                commands = "python ";
                }
          filenames = "\"" + filenames + "\"";



          commands += filenames;
          commands += " \"" + saveFileTxt + "\"";
          system(commands.c_str());

#endif



        //char* string
        //snprintf(string, 128, "delta %+f", mainwindow->maincurve->crosshair_2.value - mainwindow->maincurve->crosshair_1.value); //to get the difference in time between the crosshairs
        //^^not super important eitherway
        //maybe override the crosshair functions to change the colors that are being used rn

        mainwindow->Escape_fun();


    }
}
void startGyroAnalysis::backToCrosshairs() {

    mainwindow->creategraphs =  new createGraphs(mainwindow);
    dock->close();
}


startGyroAnalysis::~startGyroAnalysis()
{
    dock->close();
    delete dock;
}