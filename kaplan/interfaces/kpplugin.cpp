/*
   This file is part of Kontact

   Copyright (c) 2001 Matthias Hoelzer-Kluepfel <mhk@kde.org>
   Copyright (c) 2002 Daniel Molkentin <molkentin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

// $Id$

#include <dcopclient.h>

#include "kpcore.h"

#include "kpplugin.h"

using namespace Kaplan;

class Plugin::Private
{
public:
	Kaplan::Core *core;
    DCOPClient *dcopClient;
    QCString name;
};


Plugin::Plugin(Kaplan::Core *core, QObject *parent, const char *name)
: QObject(parent, name)
{
	d = new Kaplan::Plugin::Private;
    d->name = name;
	d->core = core;
    d->dcopClient = 0L;
}


Plugin::~Plugin()
{
        delete d->dcopClient;
	delete d;
}


Kaplan::Core *Plugin::core() const
{
	return d->core;
}

DCOPClient * Plugin::dcopClient() const
{
    if (!d->dcopClient)
    {
        d->dcopClient = new DCOPClient();
        d->dcopClient->registerAs(d->name, false);
    }

    return d->dcopClient;
}


#include "kpplugin.moc"

// vim: ts=4 sw=4 et
