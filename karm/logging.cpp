#include "logging.h"
#include "task.h"
#include "preferences.h"
#include <qdatetime.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <iostream>

#define QS(c) QString::fromLatin1(c)

//
//                            L O G    E V E N T S
//
QString KarmLogEvent::escapeXML( QString string )
{
  QString result = QString(string);
  result.replace( QRegExp(QS("&")),  QS("&amp;")  );
  result.replace( QRegExp(QS("<")),  QS("&lt;")   );
  result.replace( QRegExp(QS(">")),  QS("&gt;")   );
  result.replace( QRegExp(QS("'")),  QS("&apos;") );
  result.replace( QRegExp(QS("\"")), QS("&quot;") );
  // protect also our task-separator
  result.replace( QRegExp(QS("/")),  QS("&slash;") );

  return result;
}

// Common LogEvent stuff
//
void KarmLogEvent::loadCommonFields( Task *task)
{
  eventTime =  QDateTime::currentDateTime();
  
  QListViewItem *item = task;
  fullName = escapeXML(task->name());
  while( ( item = item->parent() ) )
  {
    fullName = escapeXML(((Task *)item)->name()) + fullName.prepend('/');
  }
}

// Start LogEvent
//
StartLogEvent::StartLogEvent( Task *task )
{
  loadCommonFields(task);
}

QString StartLogEvent::toXML()
{
  return QString("<starting        "
                 " date=\"%1\" "
                 " task=\"%2\" />\n"
                ).arg( eventTime.toString() ).arg(
                       fullName );
}

// Stop LogEvent
//
StopLogEvent::StopLogEvent( Task *task )
{
  loadCommonFields(task);
}

QString StopLogEvent::toXML()
{
  return QString("<stopping        "
                 " date=\"%1\" "
                 " task=\"%2\" />\n"
                ).arg( eventTime.toString() ).arg(
                       fullName );
}

// Rename LogEvent
//
RenameLogEvent::RenameLogEvent( Task *task, QString& old )
{
  loadCommonFields(task);
  oldName = old;
}

QString RenameLogEvent::toXML()
{
  return QString("<renaming        "
                 " date=\"%1\" "
                 " task=\"%2\" "
                 " old_name=\"%3\" />\n"
                ).arg( eventTime.toString() ).arg(
                       fullName ).arg(
                       oldName );
}

// Set Session Time LogEvent
// 
SessionTimeLogEvent::SessionTimeLogEvent( Task *task, long total, long change)
{
  loadCommonFields(task);
  delta = change;
  newTotal = total;
}

QString SessionTimeLogEvent::toXML()
{
  return QString("<new_session_time"
                 " date=\"%1\" "
                 " task=\"%2\" "
                 " new_total=\"%3\""
                 " change=\"%4\" />\n"
                ).arg( eventTime.toString() ).arg(
                       fullName ).arg(
                       newTotal ).arg(
                       delta );
}

// Set Total Time LogEvent
// 
TotalTimeLogEvent::TotalTimeLogEvent( Task *task, long total, long change)
{
  loadCommonFields(task);
  delta = change;
  newTotal = total;
}

QString TotalTimeLogEvent::toXML()
{
  return QString("<new_total_time  "
                 " date=\"%1\" "
                 " task=\"%2\" "
                 " new_total=\"%3\""
                 " change=\"%4\" />\n"
                ).arg( eventTime.toString() ).arg(
                       fullName ).arg(
                       newTotal ).arg(
                       delta );
}

//
//                              L O G G I N G
//
Logging *Logging::_instance = 0;

Logging::Logging()
{
  _preferences = Preferences::instance();
}

void Logging::start( Task *task)
{
  KarmLogEvent* event = new StartLogEvent(task);
  log(event);
}

void Logging::stop( Task *task)
{
  KarmLogEvent* event = new StopLogEvent(task);
  log(event);
}

void Logging::newTotalTime( Task *task, long minutes, long change)
{
  KarmLogEvent* event = new TotalTimeLogEvent(task, minutes, change);
  log(event);
}

void Logging::newSessionTime( Task *task, long minutes, long change)
{
  KarmLogEvent* event = new SessionTimeLogEvent(task, minutes, change);
  log(event);
}

void Logging::rename( Task *task, QString& oldName)
{
  KarmLogEvent* event = new RenameLogEvent(task, oldName);
  log(event);
}

void Logging::log( KarmLogEvent* event)
{
  if(_preferences->timeLogging()) {
    QFile f(_preferences->timeLog());

    if ( f.open( IO_WriteOnly | IO_Append) ) {
      QTextStream out( &f );        // use a text stream
      out << event->toXML();
      f.close();
    } else {
      std::cerr << "Couldn't write to time-log file";
    }
  }
}

Logging *Logging::instance()
{
  if (_instance == 0) {
    _instance = new Logging();
  }
  return _instance;
}


Logging::~Logging()
{
}

