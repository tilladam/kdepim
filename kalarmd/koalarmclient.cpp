/*
    KOrganizer Alarm Daemon Client.

    This file is part of KOrganizer.
    Copyright (c) 2002 Cornelius Schumacher

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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <kdebug.h>
#include <klocale.h>

#include <libkcal/calendarlocal.h>
#include <libkcal/icalformat.h>

#include "alarmdockwindow.h"
#include "alarmdialog.h"

#include "koalarmclient.h"
#include "koalarmclient.moc"

KOAlarmClient::KOAlarmClient(QObject *parent, const char *name)
  : QObject(parent, name),
    DCOPObject("ac"),
    mSuspendTimer(this)
{
  kdDebug(5900) << "KOAlarmClient::KOAlarmClient()" << endl;

  mDocker = new AlarmDockWindow(this);
  mDocker->show();

  mAlarmDialog = new AlarmDialog;
  connect(mAlarmDialog, SIGNAL(suspendSignal(int)), SLOT(suspend(int)));
}

KOAlarmClient::~KOAlarmClient()
{
}

void KOAlarmClient::handleEvent( const QString &iCalendarString )
{
//  kdDebug(5900) << "KOAlarmClient::handleEvent()" << endl;
  
//  kdDebug(5900) << "-- iCalendar-String:" << iCalendarString << endl;

  CalendarLocal cal;
  ICalFormat format;
  format.fromString( &cal, iCalendarString );
  
  QPtrList<Event> events = cal.getAllEvents();

  Event *ev;
  for( ev = events.first(); ev; ev = events.next() ) {
    mAlarmDialog->appendEvent( new Event( *ev ) );
  }

  QPtrList<Todo> todos = cal.todos();

  Todo *todo;
  for( todo = todos.first(); todo; todo = todos.next() ) {
    mAlarmDialog->appendTodo( new Todo( *todo ) );
  }
  
  showAlarmDialog();
}

/* Schedule the alarm dialog for redisplay after a specified number of minutes */
void KOAlarmClient::suspend(int minutes)
{
//  kdDebug(5900) << "KOAlarmClient::suspend() " << minutes << " minutes" << endl;
  connect(&mSuspendTimer, SIGNAL(timeout()), SLOT(showAlarmDialog()));
  mSuspendTimer.start(1000*60*minutes, true);
}

/* Display the alarm dialog (showing KOrganizer-type events) */
void KOAlarmClient::showAlarmDialog()
{
  mAlarmDialog->show();
  mAlarmDialog->eventNotification();
}
