/***************************************************************************
                          kngrouppropdlg.cpp  -  description
                             -------------------

    copyright            : (C) 1999 by Christian Thurner
    email                : cthurner@freepage.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlineedit.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>

#include <klocale.h>

#include "knuserwidget.h"
#include "utilities.h"
#include "kngroup.h"
#include "kngrouppropdlg.h"


KNGroupPropDlg::KNGroupPropDlg(KNGroup *group, QWidget *parent, const char *name )
	: KDialogBase(Tabbed, i18n("Properties of ").arg(group->name()),
	              Ok|Cancel|Help, Ok, parent, name),
	  grp(group), nChanged(false)
{

  // General tab ===============================================

  QWidget *page = addPage(i18n("&General"));
  QGridLayout *pageL=new QGridLayout(page,  4, 2, 5,5);

 	QLabel *l=new QLabel(i18n("Nickname:"), page);
 	pageL->addWidget(l,0,0);
	nick=new QLineEdit(page);
	if (grp->hasName())
	  nick->setText(grp->name());
	pageL->addWidget(nick,0,1);
	
	QGroupBox *gb=new QGroupBox(i18n("Statistics"), page);
	pageL->addMultiCellWidget(gb,2,2,0,2);
	QGridLayout *grpL=new QGridLayout(gb, 5,2, 20,10);
	
	l=new QLabel(i18n("Articles:"), gb);
	grpL->addWidget(l,0,0);
  l=new QLabel(QString::number(grp->count()),gb);
  grpL->addWidget(l,0,1);
	
  l=new QLabel(i18n("Unread articles:"), gb);
  grpL->addWidget(l,1,0);
  l=new QLabel(QString::number(grp->count()-grp->readCount()),gb);
  grpL->addWidget(l,1,1);
	
	l=new QLabel(i18n("New articles:"), gb);
  grpL->addWidget(l,2,0);
  l=new QLabel(QString::number(grp->newCount()),gb);
  grpL->addWidget(l,2,1);  	

	l=new QLabel(i18n("Threads with unread articles:"), gb);
	grpL->addWidget(l,3,0);
  l=new QLabel(QString::number(grp->statThrWithUnread()),gb);
  grpL->addWidget(l,3,1);	
	
	l=new QLabel(i18n("Threads with new articles:"), gb);
	grpL->addWidget(l,4,0);
  l=new QLabel(QString::number(grp->statThrWithNew()),gb);
  grpL->addWidget(l,4,1);	
		
	pageL->activate();

  // Specfic Identity tab =========================================	
	
	uw=new KNUserWidget(addVBoxPage(i18n("&Specific Identity")));	
		
	if (grp->user())
	  uw->setData(grp->user());
	
	
  restoreWindowSize("groupPropDLG", this, sizeHint());
}



KNGroupPropDlg::~KNGroupPropDlg()
{
	saveWindowSize("groupPropDLG", size());
}



void KNGroupPropDlg::apply()
{
	if( !(grp->name()==nick->text()) ) {
		grp->setName(nick->text());
		nChanged=true;
	}
	uw->applyData();	
}
