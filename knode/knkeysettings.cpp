/***************************************************************************
                          knkeysettings.cpp  -  description
                             -------------------

    copyright            : (C) 2000 by Christian Thurner
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

#include <klocale.h>
#include <kkeydialog.h>
#include <qlayout.h>

#include "knkeysettings.h"
#include "knglobals.h"

KNKeySettings::KNKeySettings(QWidget *parent) : KNSettingsWidget(parent)
{
	kc=new KKeyChooser(xTop->actionCollection()->keyDict(), this);
	
	stdBtn=new QPushButton(i18n("Reset"), this);
	stdBtn->setFixedSize(stdBtn->sizeHint());
	connect(stdBtn, SIGNAL(clicked()), kc, SLOT(allDefault()));

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(kc);
  layout->addWidget(stdBtn);

	layout->setResizeMode(QLayout::Minimum);
}

KNKeySettings::~KNKeySettings()
{
}

void KNKeySettings::apply()
{
	kc->listSync();
}

