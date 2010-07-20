/*
    This file is part of Akonadi.

    Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
    USA.
*/

// READ THE README FILE

#include "mainwindow.h"

#include <akonadi/control.h>

#include "desctab.h"
#include "tab1widget.h"
#include "tab2widget.h"
#include "tab2_5widget.h"
#include "tab3widget.h"
#include "tab4widget.h"
#include "tab5widget.h"
#include "tab6widget.h"
#include "tab7widget.h"
#include "unreadmailswidget.h"
#include "recursiveitemlistwidget.h"
#include "entityfindingwidget.h"
#include "collectionmonitoredwidget.h"
#include "checkable2.h"
#include "unreadmailsincollectionswidget.h"


MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
  QTabWidget *tabWidget = new QTabWidget(this);

  Akonadi::Control::start();

  tabWidget->addTab(new Checkable2(tabWidget), "Checkable2");
  tabWidget->addTab(new DescTabWidget(tabWidget), "Desc");
  tabWidget->addTab(new Tab1Widget(tabWidget), "EntityTreeModel");
  tabWidget->addTab(new Tab2Widget(tabWidget), "setRootIndex");
  tabWidget->addTab(new Tab2_5Widget(tabWidget), "Type specific data");
  tabWidget->addTab(new Tab3Widget(tabWidget), "KSelectionProxyModel");
  tabWidget->addTab(new Tab4Widget(tabWidget), "KSelectionProxyModel Filtered");
  tabWidget->addTab(new Tab6Widget(tabWidget), "Checkable Collections");
  tabWidget->addTab(new Tab5Widget(tabWidget), "Categorized Items");
  tabWidget->addTab(new Tab7Widget(tabWidget), "Tagged Items");
  tabWidget->addTab(new UnreadMailsWidget(tabWidget), "Unread Mails");
  tabWidget->addTab(new RecursiveItemListWidget(tabWidget), "Items only");
  tabWidget->addTab(new EntityFindingWidget(tabWidget), "Find entities");
  tabWidget->addTab(new CollectionMonitoredWidget(tabWidget), "SetCollectionMonitored");
  tabWidget->addTab(new UnreadMailsInCollectionsWidget(tabWidget), "UnreadMails");

  setCentralWidget(tabWidget);
}
