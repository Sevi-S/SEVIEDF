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



#include "annotations_dock.h"



static void process_events(void)
{
  qApp->processEvents();
}


UI_Annotationswindow::UI_Annotationswindow(struct edfhdrblock *e_hdr, QWidget *w_parent)
{
  QPalette palette;


  mainwindow = (UI_Mainwindow *)w_parent;

  edf_hdr = e_hdr;

  docklist = new QDockWidget("Annotations", w_parent);
  docklist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  docklist->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  relative = 1;

  mainwindow->annotations_onset_relative = 1;

  selected = -1;

  invert_filter = 0;

  hide_nk_triggers = 0;

  hide_bs_triggers = 0;

  dialog1 = new QDialog;

  checkbox1 = new QCheckBox("Relative ");
  checkbox1->setGeometry(2, 2, 10, 10);
  checkbox1->setTristate(false);
  checkbox1->setCheckState(Qt::Checked);

  label1 = new QLabel;
  label1->setText(" Filter:");

  search_line_edit = new QLineEdit;
  search_line_edit->setMaxLength(16);

  checkbox2 = new QCheckBox("Inv.");
  checkbox2->setGeometry(2, 2, 10, 10);
  checkbox2->setTristate(false);
  checkbox2->setCheckState(Qt::Unchecked);

  more_button = new QPushButton("More");
  more_button->setMaximumWidth(40);

  list = new QListWidget(dialog1);
  list->setFont(*mainwindow->monofont);
  list->setAutoFillBackground(true);
  list->setPalette(palette);
  list->setUniformItemSizes(true);

  show_between_act = new QAction("Set timescale from here to next annotation", list);
  hide_annot_act = new QAction("Hide", list);
  unhide_annot_act = new QAction("Unhide", list);
  hide_same_annots_act = new QAction("Hide similar", list);
  unhide_same_annots_act = new QAction("Unhide similar", list);
  unhide_all_annots_act = new QAction("Unhide all", list);
  average_annot_act = new QAction("Average", list);
  hide_all_NK_triggers_act = new QAction("Hide all Nihon Kohden triggers", list);
  hide_all_BS_triggers_act = new QAction("Hide all Biosemi triggers", list);
  unhide_all_NK_triggers_act = new QAction("Unhide all Nihon Kohden triggers", list);
  unhide_all_BS_triggers_act = new QAction("Unhide all Biosemi triggers", list);
  filt_ival_time_act = new QAction("Filter Interval Time", list);
  show_stats_act = new QAction("Heart Rate Variability", list);
  show_heart_rate_act = new QAction("Heart Rate", list);

  list->setContextMenuPolicy(Qt::ActionsContextMenu);
  list->insertAction(NULL, show_between_act);
  list->insertAction(NULL, hide_annot_act);
  list->insertAction(NULL, hide_same_annots_act);
  list->insertAction(NULL, unhide_annot_act);
  list->insertAction(NULL, unhide_same_annots_act);
  list->insertAction(NULL, unhide_all_annots_act);
  list->insertAction(NULL, average_annot_act);
  list->insertAction(NULL, hide_all_NK_triggers_act);
  list->insertAction(NULL, unhide_all_NK_triggers_act);
  list->insertAction(NULL, hide_all_BS_triggers_act);
  list->insertAction(NULL, unhide_all_BS_triggers_act);
  list->insertAction(NULL, filt_ival_time_act);
  list->insertAction(NULL, show_stats_act);
  list->insertAction(NULL, show_heart_rate_act);

  h_layout = new QHBoxLayout;
  h_layout->addWidget(checkbox1);
  h_layout->addWidget(label1);
  h_layout->addWidget(search_line_edit);
  h_layout->addWidget(checkbox2);
  h_layout->addWidget(more_button);

  v_layout = new QVBoxLayout(dialog1);
  v_layout->addLayout(h_layout);
  v_layout->addWidget(list);
  v_layout->setSpacing(1);

  delayed_list_filter_update_timer = new QTimer(this);
  delayed_list_filter_update_timer->setSingleShot(true);

  docklist->setWidget(dialog1);

  updateList();

  QObject::connect(list,                       SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(annotation_selected(QListWidgetItem *)));
  QObject::connect(docklist,                   SIGNAL(visibilityChanged(bool)),        this, SLOT(hide_editdock(bool)));
  QObject::connect(checkbox1,                  SIGNAL(stateChanged(int)),              this, SLOT(checkbox1_clicked(int)));
  QObject::connect(checkbox2,                  SIGNAL(stateChanged(int)),              this, SLOT(checkbox2_clicked(int)));
  QObject::connect(more_button,                SIGNAL(clicked(bool)),                  this, SLOT(more_button_clicked(bool)));
  QObject::connect(hide_annot_act,             SIGNAL(triggered(bool)),                this, SLOT(hide_annot(bool)));
  QObject::connect(unhide_annot_act,           SIGNAL(triggered(bool)),                this, SLOT(unhide_annot(bool)));
  QObject::connect(hide_same_annots_act,       SIGNAL(triggered(bool)),                this, SLOT(hide_same_annots(bool)));
  QObject::connect(unhide_same_annots_act,     SIGNAL(triggered(bool)),                this, SLOT(unhide_same_annots(bool)));
  QObject::connect(unhide_all_annots_act,      SIGNAL(triggered(bool)),                this, SLOT(unhide_all_annots(bool)));
  QObject::connect(average_annot_act,          SIGNAL(triggered(bool)),                this, SLOT(average_annot(bool)));
  QObject::connect(show_between_act,           SIGNAL(triggered(bool)),                this, SLOT(show_between(bool)));
  QObject::connect(hide_all_NK_triggers_act,   SIGNAL(triggered(bool)),                this, SLOT(hide_all_NK_triggers(bool)));
  QObject::connect(hide_all_BS_triggers_act,   SIGNAL(triggered(bool)),                this, SLOT(hide_all_BS_triggers(bool)));
  QObject::connect(unhide_all_NK_triggers_act, SIGNAL(triggered(bool)),                this, SLOT(unhide_all_NK_triggers(bool)));
  QObject::connect(unhide_all_BS_triggers_act, SIGNAL(triggered(bool)),                this, SLOT(unhide_all_BS_triggers(bool)));
  QObject::connect(filt_ival_time_act,         SIGNAL(triggered(bool)),                this, SLOT(filt_ival_time(bool)));
  QObject::connect(show_stats_act,             SIGNAL(triggered(bool)),                this, SLOT(show_stats(bool)));
  QObject::connect(show_heart_rate_act,        SIGNAL(triggered(bool)),                this, SLOT(show_heart_rate(bool)));
  QObject::connect(search_line_edit,           SIGNAL(textEdited(const QString)),      this, SLOT(filter_edited(const QString)));
  QObject::connect(delayed_list_filter_update_timer, SIGNAL(timeout()),                this, SLOT(delayed_list_filter_update()));
}


void UI_Annotationswindow::more_button_clicked(bool)
{
  QMessageBox messagewindow(QMessageBox::Information, "Info", "Right-click on an annotation for more options.");
  messagewindow.exec();
  return;
}


void UI_Annotationswindow::show_heart_rate(bool)
{
  int instance_num;

  char str[4096]="";

  struct annotation_list *annot_list;

  struct annotationblock *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  annot_list = &(edf_hdr->annot_list);
  if(annot_list == NULL)
  {
    snprintf(str, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  struct hrv_dock_param_struct dock_param;

  memset(&dock_param, 0, sizeof(struct hrv_dock_param_struct));

  for(instance_num=0; instance_num<MAXHRVDOCKS; instance_num++)
  {
    if(mainwindow->hrv_dock[instance_num] == NULL)
    {
      break;
    }
  }

  if(instance_num == MAXHRVDOCKS)
  {
    return;
  }

  dock_param.instance_num = instance_num;

  dock_param.edfhdr = edf_hdr;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.annot_name, annot->description, 32);

  mainwindow->hrv_dock[instance_num] = new UI_hrv_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hrv_dock[instance_num]->hrv_dock);

  mainwindow->insertToolBarBreak(mainwindow->hrv_dock[instance_num]->hrv_dock);

  edf_hdr->hrv_dock[instance_num] = instance_num + 1;

  QObject::connect(mainwindow, SIGNAL(annot_docklist_changed()), mainwindow->hrv_dock[instance_num], SLOT(update_curve()));

  mainwindow->enable_hrv_stats_toolbar(annot->description, annot_list);
}


void UI_Annotationswindow::show_stats(bool)
{
  char str[4096]="";

  struct annotation_list *annot_list;

  struct annotationblock *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor and try again.");
    messagewindow.exec();
    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  annot_list = &(edf_hdr->annot_list);
  if(annot_list == NULL)
  {
    snprintf(str, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    return;
  }

  UI_StatisticWindow stats_wndw(NULL, 0LL, mainwindow, annot_list, annot);
}


void UI_Annotationswindow::filt_ival_time(bool)
{
  struct annotation_list *annot_list;

  struct annotationblock *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor and try again.");
    messagewindow.exec();

    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());

  UI_AnnotFilterWindow filter_wndw(mainwindow, annot, mainwindow->annot_filter, edf_hdr);
}


void UI_Annotationswindow::hide_all_NK_triggers(bool)
{
  int i, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  hide_nk_triggers = 1;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hided_in_list = 1;

      annot->hided = 1;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_all_BS_triggers(bool)
{
  int i, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  hide_bs_triggers = 1;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hided_in_list = 1;

      annot->hided = 1;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_NK_triggers(bool)
{
  int i, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  hide_nk_triggers = 0;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hided_in_list = 0;

      annot->hided = 0;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_BS_triggers(bool)
{
  int i, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  hide_bs_triggers = 0;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hided_in_list = 0;

      annot->hided = 0;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::filter_edited(const QString)
{
  delayed_list_filter_update_timer->start(500);
}


void UI_Annotationswindow::delayed_list_filter_update()
{
  int i, j, n, len, sz;

  char filter_str[32];

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  QString text = search_line_edit->text();

  sz = edfplus_annotation_size(annot_list);

  if(sz < 1)  return;

  if(text.length() < 1)
  {
    for(i=0; i<sz; i++)
    {
      annot = edfplus_annotation_get_item(annot_list, i);

      if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) ||
           ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
      {
        annot->hided_in_list = 0;

        annot->hided = 0;
      }
    }

    updateList();

    mainwindow->maincurve->update();

    return;
  }

  strlcpy(filter_str, search_line_edit->text().toUtf8().data(), 32);

  len = strlen(filter_str);

  if(invert_filter == 0)
  {
    for(i=0; i<sz; i++)
    {
      annot = edfplus_annotation_get_item(annot_list, i);

      if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) ||
           ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
      {
        annot->hided_in_list = 1;

        if(mainwindow->annot_filter->hide_in_list_only)
        {
          annot->hided = 0;
        }
        else
        {
          annot->hided = 1;
        }

        n = strlen(annot->description) - len + 1;

        for(j=0; j<n; j++)
        {
          if(!(strncmp(filter_str, annot->description + j, len)))
          {
            annot->hided_in_list = 0;

            annot->hided = 0;

            break;
          }
        }
      }
    }
  }
  else
  {
    for(i=0; i<sz; i++)
    {
      annot = edfplus_annotation_get_item(annot_list, i);

      if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) ||
           ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
      {
        annot->hided_in_list = 0;

        annot->hided = 0;

        n = strlen(annot->description) - len + 1;

        for(j=0; j<n; j++)
        {
          if(!(strncmp(filter_str, annot->description + j, len)))
          {
            annot->hided_in_list = 1;

            if(!mainwindow->annot_filter->hide_in_list_only)
            {
              annot->hided = 1;
            }

            break;
          }
        }
      }
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::checkbox2_clicked(int state)
{
  int changed=0, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  sz = edfplus_annotation_size(annot_list);

  if(sz < 1)  return;

  if(state==Qt::Checked)
  {
    if(invert_filter == 0)  changed = 1;

    invert_filter = 1;
  }

  if(state==Qt::Unchecked)
  {
    if(invert_filter == 1)  changed = 1;

    invert_filter = 0;
  }

  if(changed == 0)  return;

  filter_edited(search_line_edit->text());
}


void UI_Annotationswindow::show_between(bool)
{
  int n, sz;

  long long displaytime;

  if(list->count() < 2)
  {
    return;
  }

  n = list->currentRow();

  if(n >= (list->count() - 1))
  {
    return;
  }

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= (sz - 1))  return;

  annot = edfplus_annotation_get_item(annot_list, n);

  displaytime = annot->onset;

  annot = edfplus_annotation_get_item(annot_list, n + 1);

  displaytime -= annot->onset;

  if(displaytime < 0)
  {
    displaytime *= -1;
  }

  if(displaytime < 1000)
  {
    return;
  }

  mainwindow->pagetime = displaytime;

  annotation_selected(list->currentItem(), 0);
}


void UI_Annotationswindow::hide_annot(bool)
{
  int n, sz;

  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);

  annot->hided_in_list = 1;

  annot->hided = 1;

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_annot(bool)
{
  int n, sz;

  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item(annot_list, n);

  annot->hided_in_list = 0;

  annot->hided = 0;

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_same_annots(bool)
{
  int i, n, sz;

  char str1[MAX_ANNOTATION_LEN],
       str2[MAX_ANNOTATION_LEN];


  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);

  strlcpy(str1, annot->description, MAX_ANNOTATION_LEN);

  remove_leading_spaces(str1);

  remove_trailing_spaces(str1);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str2, annot->description, MAX_ANNOTATION_LEN);

    remove_leading_spaces(str2);

    remove_trailing_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hided_in_list = 1;

      if(mainwindow->annot_filter->hide_in_list_only)
      {
        annot->hided = 0;
      }
      else
      {
        annot->hided = 1;
      }
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_same_annots(bool)
{
  int i, n, sz;

  char str1[MAX_ANNOTATION_LEN],
       str2[MAX_ANNOTATION_LEN];

  if(list->count() < 1)  return;

  n = list->currentRow();

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);

  strlcpy(str1, annot->description, MAX_ANNOTATION_LEN);

  remove_leading_spaces(str1);

  remove_trailing_spaces(str1);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str2, annot->description, MAX_ANNOTATION_LEN);

    remove_leading_spaces(str2);

    remove_trailing_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hided_in_list = 0;

      annot->hided = 0;
    }
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_annots(bool)
{
  int i, sz;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  struct annotationblock *annot;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    annot->hided = 0;

    annot->hided_in_list = 0;
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::average_annot(bool)
{
  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(!mainwindow->signalcomps)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "First add a signal to the screen.");
    messagewindow.exec();

    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor and try again.");
    messagewindow.exec();

    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  UI_AveragerWindow average_wndw(mainwindow, list->currentRow(), edf_hdr);
}


void UI_Annotationswindow::checkbox1_clicked(int state)
{
  if(state==Qt::Checked)
  {
    relative = 1;

    mainwindow->annotations_onset_relative = 1;
  }

  if(state==Qt::Unchecked)
  {
    relative = 0;

    mainwindow->annotations_onset_relative = 0;
  }

  updateList();

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_editdock(bool visible)
{
  if(visible==false)
  {
    delete mainwindow->annotationEditDock;

    mainwindow->annotationEditDock = NULL;
  }
}



void UI_Annotationswindow::updateList(void)
{
  char str[MAX_ANNOTATION_LEN + 32];

  int j,
      sz,
      jump=0,
      modified=0;

  QListWidgetItem *listitem;

  QString string;

  struct annotationblock *annot;

  struct annotation_list *annot_list;

  selected = -1;

#ifdef Q_OS_WIN32
  QFont specialfont("courier", 11, QFont::Normal, true);
#else
  QFont specialfont("andale mono", 12, QFont::Normal, true);
#endif

  specialfont.setPixelSize(12);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  list->clear();

  annot_list = &(edf_hdr->annot_list);

  sz = edfplus_annotation_size(annot_list);

  QMessageBox messagewindow(QMessageBox::NoIcon, "Busy", "Sorting annotations...");
  messagewindow.setStandardButtons(QMessageBox::NoButton);
  if(sz > 2000)
  {
    messagewindow.show();
  }

  edfplus_annotation_sort(annot_list, &process_events);

  messagewindow.setText("Populating list...");

  for(j=0; j<sz; j++)
  {
    if(!(j % 1000))
    {
      qApp->processEvents();
    }

    annot = edfplus_annotation_get_item(annot_list, j);

    if(annot->hided_in_list)
    {
      continue;
    }

    if(relative)
    {
      if((annot->onset - edf_hdr->starttime_offset) < 0LL)
      {
        snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "  -%2i:%02i:%02i.%04i",
                (int)((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION)/ 3600),
                (int)(((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
                (int)((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 60),
                (int)((-(annot->onset - edf_hdr->starttime_offset) % TIME_DIMENSION) / 1000LL));
      }
      else
      {
        snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "  %3i:%02i:%02i.%04i",
                (int)(((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION)/ 3600),
                (int)((((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
                (int)(((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 60),
                (int)(((annot->onset - edf_hdr->starttime_offset) % TIME_DIMENSION) / 1000LL));
      }
    }
    else
    {
      snprintf(str, MAX_ANNOTATION_LEN + 32, "  %3i:%02i:%02i.%04i",
              (int)((((annot->onset + edf_hdr->l_starttime) / TIME_DIMENSION)/ 3600) % 24),
              (int)((((annot->onset + edf_hdr->l_starttime) / TIME_DIMENSION) % 3600) / 60),
              (int)(((annot->onset + edf_hdr->l_starttime) / TIME_DIMENSION) % 60),
              (int)(((annot->onset + edf_hdr->l_starttime) % TIME_DIMENSION) / 1000LL));
    }

    str[MAX_ANNOTATION_LEN + 31] = 0;

    remove_trailing_zeros(str);

    string = QString::fromUtf8(annot->description);

    if(string.size() < 20)
    {
      string = string.leftJustified(20, ' ');
    }

    string.append(QString::fromLatin1(str));

    listitem = new QListWidgetItem(string, list);

    listitem->setData(Qt::UserRole, QVariant(j));

    if(annot->modified==1)
    {
      listitem->setFont(specialfont);

      listitem->setForeground(Qt::red);

      modified = 1;
    }

    if((annot->onset - edf_hdr->starttime_offset) < 0LL)
    {
      snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: -%i:%02i:%02i.%04i",
              (int)((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION)/ 3600),
              (int)(((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
              (int)((-(annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 60),
              (int)((-(annot->onset - edf_hdr->starttime_offset) % TIME_DIMENSION) / 1000LL));
    }
    else
    {
      snprintf(str, (MAX_ANNOTATION_LEN + 32) / 2, "onset: %2i:%02i:%02i.%04i",
              (int)(((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION)/ 3600),
              (int)((((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 3600) / 60),
              (int)(((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION) % 60),
              (int)(((annot->onset - edf_hdr->starttime_offset) % TIME_DIMENSION) / 1000LL));
    }

    if(annot->duration[0]!=0)
    {
      snprintf(str + strlen(str), (MAX_ANNOTATION_LEN + 32) / 2, "\nduration: %s",annot->duration);
    }

    str[MAX_ANNOTATION_LEN + 31] = 0;

    remove_trailing_zeros(str);

    strlcat(str, "\n\n", MAX_ANNOTATION_LEN + 32);

    string = QString::fromLatin1(str);

    string.append(QString::fromUtf8(annot->description));

    listitem->setToolTip(string);

    if(annot->selected)
    {
      selected = j;

      annot->selected = 0;

      if(annot->jump)
      {
        jump = 1;

        annot->jump = 0;
      }
    }
  }

  if(mainwindow->annot_editor_active)
  {
    if(selected>=0)
    {
      list->setCurrentRow(selected, QItemSelectionModel::ClearAndSelect);

      if(mainwindow->annotationEditDock != NULL)
      {
        mainwindow->annotationEditDock->set_selected_annotation(selected);
      }

      if(jump)
      {
        jump = 0;

        annotation_selected(list->currentItem());
      }

      selected = -1;
    }

    if(modified)
    {
      mainwindow->annotations_edited = 1;

      mainwindow->save_act->setEnabled(true);
    }
  }

  messagewindow.hide();

  QApplication::restoreOverrideCursor();

  mainwindow->annot_dock_updated();
}



void UI_Annotationswindow::annotation_selected(QListWidgetItem * item, int centered)
{
  int i=0, n, sz;

  long long temp;

  struct annotationblock *annot;

  struct annotation_list *annot_list = &(edf_hdr->annot_list);

  n = item->data(Qt::UserRole).toInt();

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  edfplus_annotation_cancel_all_selected_in_dock(annot_list);

  annot = edfplus_annotation_get_item(annot_list, n);

  annot->selected_in_dock = 1;

  if(mainwindow->annot_editor_active)
  {
    if(mainwindow->annotationEditDock != NULL)
    {
      mainwindow->annotationEditDock->set_selected_annotation(n);
    }
  }

  if(mainwindow->video_player->status == VIDEO_STATUS_PLAYING)
  {
    mainwindow->video_player_seek((int)((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION));

    return;
  }

  if(mainwindow->video_player->status == VIDEO_STATUS_PAUSED)
  {
    mainwindow->video_player_seek((int)((annot->onset - edf_hdr->starttime_offset) / TIME_DIMENSION));
  }

  if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      mainwindow->edfheaderlist[i]->viewtime = annot->onset;

      if(centered)
      {
        mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->pagetime / 2);
      }

      mainwindow->edfheaderlist[i]->viewtime -= edf_hdr->starttime_offset;
    }
  }

  if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edf_hdr->viewtime = annot->onset;

    if(centered)
    {
      edf_hdr->viewtime -= (mainwindow->pagetime / 2);
    }

    edf_hdr->viewtime -= edf_hdr->starttime_offset;
  }

  if((mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    temp = annot->onset - edf_hdr->viewtime;

    temp -= edf_hdr->starttime_offset;

    if(centered)
    {
      temp -= (mainwindow->pagetime / 2);
    }

    for(i=0; i<mainwindow->files_open; i++)
    {
      mainwindow->edfheaderlist[i]->viewtime += temp;
    }
  }

  if(mainwindow->annotationEditDock != NULL)
  {
    mainwindow->maincurve->setCrosshair_1_center();
  }

  mainwindow->setup_viewbuf();
}























