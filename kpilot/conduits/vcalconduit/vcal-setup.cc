/* vcal-setup.cc                        KPilot
**
** Copyright (C) 2002-2003 Reinhold Kainhofer
** Copyright (C) 2001 by Dan Pilone
**
** This file defines the setup dialog for the vcal-conduit plugin.
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
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

#include "options.h"

#include "vcal-setup.moc"

#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcombobox.h>

#include <kconfig.h>
//#include <kfiledialog.h>
#include <kurlrequester.h>

#include "korganizerConduit.h"
#include "vcal-factory.h"


VCalWidgetSetup::VCalWidgetSetup(QWidget *w, const char *n,
	const QStringList & a) :
	ConduitConfig(w,n,a)
{
	FUNCTIONSETUP;

	fConfigWidget = new VCalWidget(widget());
	setTabWidget(fConfigWidget->tabWidget);
	addAboutPage(false,VCalConduitFactory::about());

	fConfigWidget->tabWidget->adjustSize();
	fConfigWidget->resize(fConfigWidget->tabWidget->size());

	// This is a little hack to force the config dialog to the
	// correct size, since the designer dialog is so small.
	//
	//
//	QSize s = fConfigWidget->size() + QSize(SPACING,SPACING);
//	fConfigWidget->resize(s);
//	fConfigWidget->setMinimumSize(s);

	fConfigWidget->fCalendarFile->setMode( KFile::File | KFile::LocalOnly );
	fConfigWidget->fCalendarFile->setFilter("*.vcs *.ics|ICalendars\n*.*|All files (*.*)");
}

VCalWidgetSetup::~VCalWidgetSetup()
{
	FUNCTIONSETUP;
}

/* virtual */ void VCalWidgetSetup::commitChanges()
{
	FUNCTIONSETUP;

	if (!fConfig) return;
	KConfigGroupSaver s(fConfig,configGroup() );

	// General page
	fConfig->writeEntry(VCalConduitFactoryBase::calendarType,
		fConfigWidget->fSyncDestination->id(
			fConfigWidget->fSyncDestination->selected()));
	fConfig->writeEntry(VCalConduitFactoryBase::calendarFile,
		fConfigWidget->fCalendarFile->url());

	fConfig->writeEntry(VCalConduitFactoryBase::archive,
		fConfigWidget->fArchive->isChecked());

	// Conflicts page
	fConfig->writeEntry(VCalConduitFactoryBase::conflictResolution,
		fConfigWidget->fConflictResolution->currentItem()+SyncAction::eCROffset);

}

/* virtual */ void VCalWidgetSetup::readSettings()
{
	FUNCTIONSETUP;

	if (!fConfig) return;
	KConfigGroupSaver s(fConfig, configGroup());

	// General page
	fConfigWidget->fSyncDestination->setButton(
		fConfig->readNumEntry(VCalConduitFactoryBase::calendarType, 0));
	fConfigWidget->fCalendarFile->setURL( fConfig->readEntry(
		VCalConduitFactoryBase::calendarFile,QString::null));

	fConfigWidget->fArchive->setChecked(
		fConfig->readBoolEntry(VCalConduitFactoryBase::archive, true));

	// Conflicts page
	fConfigWidget->fConflictResolution->setCurrentItem(
		fConfig->readNumEntry(VCalConduitFactoryBase::conflictResolution,
		SyncAction::eUseGlobalSetting)-SyncAction::eCROffset);


}
