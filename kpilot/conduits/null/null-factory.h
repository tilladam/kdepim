#ifndef _KPILOT_NULL_FACTORY_H
#define _KPILOT_NULL_FACTORY_H
/* null-factory.h                       KPilot
**
** Copyright (C) 2001 by Dan Pilone
**
** This file defines the factory for the null-conduit plugin.
** It also defines the class for the behavior of the setup dialog.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
** MA 02139, USA.
*/
 
/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

#include <klibloader.h>

#include "plugin.h"

class NullWidget;
class KInstance;
class KAboutData;

class NullWidgetSetup : public ConduitConfig
{
Q_OBJECT
public:
	NullWidgetSetup(QWidget *,const char *,const QStringList &);
	virtual ~NullWidgetSetup();

	virtual void readSettings();

protected:
	virtual void commitChanges();

private:
	NullWidget *fConfigWidget;
} ;

class NullConduitFactory : public KLibFactory
{
Q_OBJECT

public:
	NullConduitFactory(QObject * = 0L,const char * = 0L);
	virtual ~NullConduitFactory();

	static KAboutData *about() { return fAbout; } ;

	// Configuration keys
	static const char * const group;
	static const char * const databases,
		* const message,
		* const failImmediately;

protected:
	virtual QObject* createObject( QObject* parent = 0,
		const char* name = 0,
		const char* classname = "QObject",
		const QStringList &args = QStringList() );
private:
	KInstance *fInstance;
	static KAboutData *fAbout;
} ;

extern "C"
{

void *init_libnullconduit();

} ;

// $Log$
// Revision 1.5  2002/08/21 19:24:50  adridg
// Tail end of the license change: fixup wording and LGPL the NULL conduit.
//
// Revision 1.4  2001/12/29 15:43:46  adridg
// Various config buglets
//
// Revision 1.3  2001/12/18 07:43:25  adridg
// Actually do a (null) sync
//
// Revision 1.2  2001/10/08 22:25:41  adridg
// Moved to libkpilot and lib-based conduits
//
// Revision 1.1  2001/10/04 23:51:55  adridg
// Nope. One more really final commit to get the alpha to build. Dirk, otherwise just remove the conduits/ subdir from kdepim/kpilot/Makefile.am
//

#endif
