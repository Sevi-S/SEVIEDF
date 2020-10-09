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




#include "signal_chooser.h"



UI_SignalChooser::UI_SignalChooser(QWidget *w_parent, int job, int *sgnl_nr)
{
  task = job;

  signal_nr = sgnl_nr;

  mainwindow = (UI_Mainwindow *)w_parent;

  signalchooser_dialog = new QDialog(w_parent);

  if(task == 3)
  {
    signalchooser_dialog->setMinimumSize(435, 420);
    signalchooser_dialog->setMaximumSize(435, 420);
    signalchooser_dialog->setWindowTitle("Organize signals");
  }
  else
  {
    signalchooser_dialog->setMinimumSize(200, 420);
    signalchooser_dialog->setMaximumSize(200, 420);
    signalchooser_dialog->setWindowTitle("Signals");
  }
  signalchooser_dialog->setModal(true);
  signalchooser_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  list = new QListWidget(signalchooser_dialog);
  if(task == 3)
  {
    list->setGeometry(10, 10, 300, 365);
    list->setSelectionBehavior(QAbstractItemView::SelectRows);
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list->setToolTip("Double-click on an item to edit properties");
  }
  else
  {
    list->setGeometry(10, 10, 130, 365);
    list->setSelectionBehavior(QAbstractItemView::SelectRows);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
  }

  CloseButton = new QPushButton(signalchooser_dialog);
  CloseButton->setGeometry(10, 385, 130, 25);
  CloseButton->setText("Close");

  if(task == 3)
  {
    EditButton = new QPushButton(signalchooser_dialog);
    EditButton->setGeometry(325, 140, 100, 25);
    EditButton->setText("Edit");

    UpButton = new QPushButton(signalchooser_dialog);
    UpButton->setGeometry(325, 180, 100, 25);
    UpButton->setText("Up");

    DownButton = new QPushButton(signalchooser_dialog);
    DownButton->setGeometry(325, 220, 100, 25);
    DownButton->setText("Down");

    InvertButton = new QPushButton(signalchooser_dialog);
    InvertButton->setGeometry(325, 260, 100, 25);
    InvertButton->setText("Invert");

    DeleteButton = new QPushButton(signalchooser_dialog);
    DeleteButton->setGeometry(325, 300, 100, 25);
    DeleteButton->setText("Remove");
  }

  load_signalcomps();

  QObject::connect(CloseButton, SIGNAL(clicked()), signalchooser_dialog, SLOT(close()));

  if(task == 3)
  {
    if(list->count() > 0)
    {
      list->setCurrentRow(0);

      QObject::connect(EditButton,   SIGNAL(clicked()), this, SLOT(signalEdit()));
      QObject::connect(UpButton,     SIGNAL(clicked()), this, SLOT(signalUp()));
      QObject::connect(DownButton,   SIGNAL(clicked()), this, SLOT(signalDown()));
      QObject::connect(InvertButton, SIGNAL(clicked()), this, SLOT(signalInvert()));
      QObject::connect(DeleteButton, SIGNAL(clicked()), this, SLOT(signalDelete()));

      QObject::connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(item_activated(QListWidgetItem *)));
    }
  }
  else
  {
    QObject::connect(list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(call_sidemenu(QListWidgetItem *)));
  }

  signalchooser_dialog->exec();
}



void UI_SignalChooser::load_signalcomps(void)
{
  int i;

  QListWidgetItem *item;

  list->clear();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    if(task == 3)
    {
      item->setText(QString(mainwindow->signalcomp[i]->signallabel) + QString("  Alias: ") + QString(mainwindow->signalcomp[i]->alias));
    }
    else
    {
      item->setText(mainwindow->signalcomp[i]->signallabel);
    }
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }
}


void UI_SignalChooser::item_activated(QListWidgetItem *item)
{
  mainwindow->maincurve->exec_sidemenu(item->data(Qt::UserRole).toInt());

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::call_sidemenu(QListWidgetItem *)
{
  int i, signal_nr2;

  if(task == 3) return;

  signalchooser_dialog->hide();

  if(task == 0)
  {
    mainwindow->maincurve->exec_sidemenu(list->currentRow());
  }

  if(task == 1)
  {
    for(i=0; i<MAXSPECTRUMDOCKS; i++)
    {
      if(mainwindow->spectrumdock[i]->dock->isHidden())  break;
    }

    if(i<MAXSPECTRUMDOCKS)
    {
      mainwindow->spectrumdock[i]->init(list->currentRow());
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The maximum number of docked Power Spectrum windows has been reached.\n"
                                                                "Close one first.");
      messagewindow.exec();
    }
  }

  if(task == 5)
  {
    signal_nr2 = list->currentRow();

    if(mainwindow->signalcomp[signal_nr2]->ecg_filter != NULL)
    {
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfhdr->edfparam[mainwindow->signalcomp[signal_nr2]->edfsignal[0]].sf_int < 30)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency must be at least 30Hz and must be an integer value.");
      messagewindow.exec();
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Recording length must be at least 30 seconds.");
      messagewindow.exec();
      signalchooser_dialog->close();
      return;
    }

    for(i=0; i<MAXCDSADOCKS; i++)
    {
      if(mainwindow->cdsa_dock[i] == NULL)
      {
        UI_cdsa_window wndw(mainwindow, mainwindow->signalcomp[signal_nr2], i);

        break;
      }
    }
  }

  if(task == 2)
  {
    AdjustFilterSettings filtersettings(mainwindow->signalcomp[list->currentRow()], mainwindow->maincurve);
  }

  if(task == 4)
  {
    if(signal_nr != NULL)
    {
      *signal_nr = list->currentRow();
    }
  }

  signalchooser_dialog->close();
}


void UI_SignalChooser::signalEdit()
{
  int n,
      selected_signals[MAXSIGNALS];

  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  mainwindow->maincurve->exec_sidemenu(selected_signals[0]);

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalUp()
{
  int i, n,
      size,
      sigcomp_nr,
      selected_signals[MAXSIGNALS];

  struct signalcompblock *signalcomp;


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  size = list->count();

  if(size < (n + 1))
  {
    return;
  }

  if(selected_signals[0] < 1)
  {
    return;
  }

  for(i=0; i<n; i++)
  {
    sigcomp_nr = selected_signals[i];

    signalcomp = mainwindow->signalcomp[sigcomp_nr];

    mainwindow->signalcomp[sigcomp_nr] = mainwindow->signalcomp[sigcomp_nr - 1];

    mainwindow->signalcomp[sigcomp_nr - 1] = signalcomp;
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i] - 1)->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalDown()
{
  int i, n,
      size,
      sigcomp_nr,
      selected_signals[MAXSIGNALS];

  struct signalcompblock *signalcomp;


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  size = list->count();

  if(size < (n + 1))
  {
    return;
  }

  if(selected_signals[n-1] > (size - 2))
  {
    return;
  }

  for(i=(n-1); i>=0; i--)
  {
    sigcomp_nr = selected_signals[i];

    signalcomp = mainwindow->signalcomp[sigcomp_nr];

    mainwindow->signalcomp[sigcomp_nr] = mainwindow->signalcomp[sigcomp_nr + 1];

    mainwindow->signalcomp[sigcomp_nr + 1] = signalcomp;

    selected_signals[i] = sigcomp_nr;
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i] + 1)->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}


int UI_SignalChooser::get_selectionlist(int *slist)
{
  int i, j, n, tmp;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  selectedlist = list->selectedItems();

  n = selectedlist.size();

  if(n < 1)
  {
    return 0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);

    slist[i] = item->data(Qt::UserRole).toInt();

    for(j=i; j>0; j--)  // Sort the list!!
    {
      if(slist[j] > slist[j-1])
      {
        break;
      }

      tmp = slist[j];

      slist[j] = slist[j-1];

      slist[j-1] = tmp;
    }
  }

  return n;
}


void UI_SignalChooser::signalDelete()
{
  int i, j, k, n, p,
      sigcomp_nr,
      selected_signr[MAXSIGNALS];


  n = get_selectionlist(selected_signr);

  if(n < 1)
  {
    return;
  }

  for(k=0; k<n; k++)
  {
    sigcomp_nr = selected_signr[k];

    sigcomp_nr -= k;

    for(i=0; i<MAXSPECTRUMDOCKS; i++)
    {
      if(mainwindow->spectrumdock[i]->signalcomp == mainwindow->signalcomp[sigcomp_nr])
      {
        mainwindow->spectrumdock[i]->clear();
        mainwindow->spectrumdock[i]->dock->hide();
      }
    }

    for(i=0; i<MAXSPECTRUMDIALOGS; i++)
    {
      p = mainwindow->signalcomp[sigcomp_nr]->spectr_dialog[i];

      if(p != 0)
      {
        delete mainwindow->spectrumdialog[p - 1];

        mainwindow->spectrumdialog[p - 1] = NULL;
      }
    }

    for(i=0; i<MAXCDSADOCKS; i++)
    {
      p = mainwindow->signalcomp[sigcomp_nr]->cdsa_dock[i];

      if(p != 0)
      {
        delete mainwindow->cdsa_dock[p - 1];

        mainwindow->cdsa_dock[p - 1] = NULL;
      }
    }

    for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
    {
      p = mainwindow->signalcomp[sigcomp_nr]->avg_dialog[i];

      if(p != 0)
      {
        delete mainwindow->averagecurvedialog[p - 1];

        mainwindow->averagecurvedialog[p - 1] = NULL;
      }
    }

    for(i=0; i<MAXZSCOREDIALOGS; i++)
    {
      p = mainwindow->signalcomp[sigcomp_nr]->zscoredialog[i];

      if(p != 0)
      {
        delete mainwindow->zscoredialog[p - 1];

        mainwindow->zscoredialog[p - 1] = NULL;
      }
    }

    if(mainwindow->signalcomp[sigcomp_nr]->hascursor2)
    {
      mainwindow->maincurve->crosshair_2.active = 0;
      mainwindow->maincurve->crosshair_2.moving = 0;
    }

    if(mainwindow->signalcomp[sigcomp_nr]->hascursor1)
    {
      mainwindow->maincurve->crosshair_1.active = 0;
      mainwindow->maincurve->crosshair_2.active = 0;
      mainwindow->maincurve->crosshair_1.moving = 0;
      mainwindow->maincurve->crosshair_2.moving = 0;

      for(i=0; i<mainwindow->signalcomps; i++)
      {
        mainwindow->signalcomp[i]->hascursor2 = 0;
      }
    }

    if(mainwindow->signalcomp[sigcomp_nr]->hasruler)
    {
      mainwindow->maincurve->ruler_active = 0;
      mainwindow->maincurve->ruler_moving = 0;
    }

    for(j=0; j<mainwindow->signalcomp[sigcomp_nr]->filter_cnt; j++)
    {
      free(mainwindow->signalcomp[sigcomp_nr]->filter[j]);
    }

    mainwindow->signalcomp[sigcomp_nr]->filter_cnt = 0;

    if(mainwindow->signalcomp[sigcomp_nr]->spike_filter != NULL)
    {
      free_spike_filter(mainwindow->signalcomp[sigcomp_nr]->spike_filter);
    }

    mainwindow->signalcomp[sigcomp_nr]->spike_filter = NULL;

    for(j=0; j<mainwindow->signalcomp[sigcomp_nr]->ravg_filter_cnt; j++)
    {
      free_ravg_filter(mainwindow->signalcomp[sigcomp_nr]->ravg_filter[j]);
    }

    mainwindow->signalcomp[sigcomp_nr]->ravg_filter_cnt = 0;

    if(mainwindow->signalcomp[sigcomp_nr]->fir_filter != NULL)
    {
      free_fir_filter(mainwindow->signalcomp[sigcomp_nr]->fir_filter);

      mainwindow->signalcomp[sigcomp_nr]->fir_filter = NULL;
    }

    if(mainwindow->signalcomp[sigcomp_nr]->ecg_filter != NULL)
    {
      free_ecg_filter(mainwindow->signalcomp[sigcomp_nr]->ecg_filter);

      mainwindow->signalcomp[sigcomp_nr]->ecg_filter = NULL;

      strlcpy(mainwindow->signalcomp[sigcomp_nr]->signallabel, mainwindow->signalcomp[sigcomp_nr]->signallabel_bu, 512);
      mainwindow->signalcomp[sigcomp_nr]->signallabellen = mainwindow->signalcomp[sigcomp_nr]->signallabellen_bu;
      strlcpy(mainwindow->signalcomp[sigcomp_nr]->physdimension, mainwindow->signalcomp[sigcomp_nr]->physdimension_bu, 512);
    }

    for(j=0; j<mainwindow->signalcomp[sigcomp_nr]->fidfilter_cnt; j++)
    {
      free(mainwindow->signalcomp[sigcomp_nr]->fidfilter[j]);
      fid_run_free(mainwindow->signalcomp[sigcomp_nr]->fid_run[j]);
      fid_run_freebuf(mainwindow->signalcomp[sigcomp_nr]->fidbuf[j]);
      fid_run_freebuf(mainwindow->signalcomp[sigcomp_nr]->fidbuf2[j]);
    }

    mainwindow->signalcomp[sigcomp_nr]->fidfilter_cnt = 0;

    if(mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter)
    {
      plif_free_subtract_filter(mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter);

      mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter = NULL;
    }

    if(mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter_sav)
    {
      plif_free_subtract_filter(mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter_sav);

      mainwindow->signalcomp[sigcomp_nr]->plif_ecg_filter_sav = NULL;
    }

    free(mainwindow->signalcomp[sigcomp_nr]);

    for(i=sigcomp_nr; i<mainwindow->signalcomps - 1; i++)
    {
      mainwindow->signalcomp[i] = mainwindow->signalcomp[i + 1];
    }

    mainwindow->signalcomps--;
  }

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalInvert()
{
  int i, n,
      selected_signals[MAXSIGNALS];


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  for(i=0; i<n; i++)
  {
    mainwindow->signalcomp[selected_signals[i]]->polarity *= -1;

    mainwindow->signalcomp[selected_signals[i]]->screen_offset *= -1;
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i])->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::strip_types_from_label(char *label)
{
  int i,
      len;


  len = strlen(label);
  if(len<16)
  {
    return;
  }

  if((!(strncmp(label, "EEG ", 4)))
   ||(!(strncmp(label, "ECG ", 4)))
   ||(!(strncmp(label, "EOG ", 4)))
   ||(!(strncmp(label, "ERG ", 4)))
   ||(!(strncmp(label, "EMG ", 4)))
   ||(!(strncmp(label, "MEG ", 4)))
   ||(!(strncmp(label, "MCG ", 4))))
  {
    if(label[4]!=' ')
    {
      for(i=0; i<(len-4); i++)
      {
        label[i] = label[i+4];
      }

      for(; i<len; i++)
      {
        label[i] = ' ';
      }
    }
  }
}





