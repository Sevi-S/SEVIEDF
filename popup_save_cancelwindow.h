/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2020 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/




#ifndef POPUP_CANCELSAVEFORM1_H
#define POPUP_CANCELSAVEFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <string.h>

#include "global.h"
#include "utils.h"



class UI_CancelSavewindow : public QObject
{
  Q_OBJECT

public:
  UI_CancelSavewindow(const char *, const char *,
                      const char *, const char *,
                      int *);

private:

QDialog      *MessageDialog;

QLabel       *Label1;

QPushButton  *pushButton1,
             *pushButton2;

int *buttonnr;


private slots:

void button1Clicked();
void button2Clicked();

};



#endif


