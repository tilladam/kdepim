/*
    This file is part of the KOrganizer alarm daemon.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

#include "simplekalarmd.h"

#include "alarmdialog.h"

#include <libkcal/calendarlocal.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kuniqueapplication.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kconfig.h>

#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>

using namespace KCal;

SimpleKalarmd::SimpleKalarmd()
{
  mAlarmDialog = new AlarmDialog( 0 );

  mCalendarDict.setAutoDelete( true );

  setPixmap( BarIcon( "misc" ) );

  mTimer = new QTimer( this );
  connect( mTimer, SIGNAL( timeout() ), SLOT( checkAlarms() ) );

  KConfig *cfg = KGlobal::config();
  cfg->setGroup( "Check" );
  int interval = cfg->readNumEntry( "Interval", 60 );
  kdDebug() << "simplekalarmd check interval: " << interval << " seconds."
            << endl;

  mTimer->start( 1000 * interval );  // interval in seconds
}

SimpleKalarmd::~SimpleKalarmd()
{
}

void SimpleKalarmd::closeEvent( QCloseEvent * )
{
  kapp->quit();
}

void SimpleKalarmd::checkAlarms()
{
  kdDebug() << "SimpleKalarmd::checkAlarms()" << endl;

  QString calFile = locateLocal( "appdata", "calendars" );

  kdDebug() << "  calendar file: " << calFile << endl;

  QFile file( calFile );
  if ( !file.open( IO_ReadOnly ) ) {
    return;
  }
  
  QTextStream ts( &file );
  QString line = ts.readLine();
  while( !line.isNull() ) {
    checkCalendar( line );
    line = ts.readLine();
  }
  
  file.close();
}

void SimpleKalarmd::checkCalendar( const QString &calFile )
{
  kdDebug() << "checkCalendar(): '" << calFile << "'" << endl;

  KConfig *cfg = KGlobal::config();

  QFileInfo fi( calFile );
  if ( !fi.exists() ) {
    kdDebug() << " File does not exist." << endl;
    return;
  }

  cfg->setGroup( "CalendarsLastModified" );
  QDateTime lastModified = fi.lastModified();

  CalendarLocal *cal = mCalendarDict.find( calFile );
  if ( !cal ) {
    kdDebug() << "NEW" << endl;
    cal = new CalendarLocal;

    KConfig cfg( locate( "config", "korganizerrc" ) );
    cfg.setGroup( "Time & Date" );
    QString tz = cfg.readEntry( "TimeZoneId", "" );
    kdDebug() << "TimeZone: " << tz << endl;
    cal->setTimeZoneId( tz );

    mCalendarDict.insert( calFile, cal );

    cal->load( calFile );
  } else {
    QDateTime oldLastModified = cfg->readDateTimeEntry( calFile );
  
    if ( !lastModified.isValid() || oldLastModified != lastModified ) {
      kdDebug() << "RELOAD" << endl;

      cal->close();
      cal->load( calFile );
    } else {
      kdDebug() << "NO reload" << endl;
    }
  }

  cfg->writeEntry( calFile, lastModified );

  cfg->setGroup( "CalendarsLastChecked" );
  QDateTime lastChecked = cfg->readDateTimeEntry( calFile );
  QDateTime from = lastChecked.addSecs( 1 );
  QDateTime to = QDateTime::currentDateTime();

  kdDebug() << "Check: " << from.toString() << " - " << to.toString() << endl;

  QValueList<Alarm *> alarms = cal->alarms( from, to );
  
  bool newEvents = false;
  QValueList<Alarm *>::ConstIterator it;
  for( it = alarms.begin(); it != alarms.end(); ++it ) {
    kdDebug() << "ALARM: " << (*it)->parent()->summary() << endl;
    Event *event = cal->event( (*it)->parent()->uid() );
    if ( event ) {
      mAlarmDialog->appendEvent( event );
      newEvents = true;
    }
  }
  if ( newEvents ) {
    mAlarmDialog->show();
    mAlarmDialog->eventNotification();
  }

  cfg->writeEntry( calFile, to );
}

class MyApp : public KUniqueApplication
{
  public:
    MyApp() : mClient( 0 ) {}

    int newInstance()
    {
      // Check if we already have a running alarm daemon widget
      if ( mClient ) return 0;

//      KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

      mClient = new SimpleKalarmd;
      mClient->show();

      return 0;
    }
    
  private:
    SimpleKalarmd *mClient;
};


static const KCmdLineOptions options[] =
{
   { 0L, 0L, 0L }
};

int main( int argc, char **argv )
{
  KLocale::setMainCatalogue( "kalarmdgui" );
  KAboutData aboutData( "simplekalarmd",
      I18N_NOOP("Simple KOrganizer Alarm Daemon"),
      "0.1", I18N_NOOP("Simple KOrganizer Alarm Daemon"),
      KAboutData::License_GPL,
      "(c) 2002 Cornelius Schumacher\n",
      0, "http://pim.kde.org");
  aboutData.addAuthor( "Cornelius Schumacher", I18N_NOOP("Maintainer"),
                       "schumacher@kde.org");

  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options );
  KUniqueApplication::addCmdLineOptions();

  if ( !MyApp::start() ) return 0;

  MyApp app;

  return app.exec();
}

#include "simplekalarmd.moc"
