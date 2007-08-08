#ifndef KEYRINGSETUP_H
#define KEYRINGSETUP_H
/* keyringsetup.h                       KPilot
**
** Copyright (C) 2007 Bertjan Broeksema
**
** This file defines the widget and behavior for the config dialog
** of the Keyring conduit.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

#include "plugin.h"

#include "ui_keyringwidget.h"

class KAboutData;

class KeyringWidgetSetup : public ConduitConfigBase
{
public:
	KeyringWidgetSetup( QWidget * );
	virtual ~KeyringWidgetSetup();
	virtual void load();
	virtual void commit();
	static ConduitConfigBase *create( QWidget * );

private: // Functions
	void savePassword();
	
	QString loadPassword();

private: // Members
	Ui::KeyringWidget fUi;
	KAboutData *fAbout;
};

#endif

