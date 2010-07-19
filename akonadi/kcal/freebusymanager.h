/*
  This file is part of the Groupware/KOrganizer integration.

  Requires the Qt and KDE widget libraries, available at no cost at
  http://www.trolltech.com and http://www.kde.org respectively

  Copyright (c) 2002-2004 Klarälvdalens Datakonsult AB
        <info@klaralvdalens-datakonsult.se>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt.  If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
*/
#ifndef FREEBUSYMANAGER_H
#define FREEBUSYMANAGER_H

#include "akonadi-kcal_next_export.h"

#include <kcal/icalformat.h>
#include <kcal/freebusycache.h>

#include <KUrl>

#include <QPointer>
#include <QByteArray>
#include <QObject>
#include <QString>

class KJob;
class QTimerEvent;

namespace KIO {
  class Job;
}
namespace KCal {
  class FreeBusy;
}
namespace Akonadi {

class Calendar;
class FreeBusyManagerPrivate;

class AKONADI_KCAL_NEXT_EXPORT FreeBusyManager : public QObject, public KCal::FreeBusyCache
{
  Q_OBJECT
  public:
    FreeBusyManager( QObject *parent );
    virtual ~FreeBusyManager();

    void setCalendar( Akonadi::Calendar * );

    /// KOrganizer publishes the free/busy list
    void publishFreeBusy( QWidget *parentWidget = 0 );

    /**
      KOrganizer downloads somebody else's free/busy list
      The call is asynchronous, and upon download, the
      receivers slot specified by member will be called.
      The slot should be of type "member(const QString&, KCal::FreeBusy*)"
        @param email Address of the person for which the F/B list should be
                     retrieved.
        @return true if a download is initiated, and false otherwise
    */
    bool retrieveFreeBusy( const QString &email, bool forceDownload,
                           QWidget *parentWidget = 0 );

    void cancelRetrieval();

    /**
      Load freebusy information belonging to email.
    */
    KCal::FreeBusy *loadFreeBusy( const QString &email );
    /**
      Store freebusy information belonging to email.
    */
    bool saveFreeBusy( KCal::FreeBusy *freebusy, const KCal::Person &person );
//    bool saveFreeBusy( KCal::FreeBusy *, const QString &email );

    /**
      Return URL of freeBusy information for given email address.
    */
    KUrl freeBusyUrl( const QString &email ) const;

    /**
      Return directory used for stroing free/busy information.
    */
    QString freeBusyDir();

    /**
      Change the broken Url status
      mBrokenUrl is used to show the 'broken url popup' only once
    */
    void setBrokenUrl( bool isBroken );

  public slots:
    // When something changed in the calendar, we get this called
    void slotPerhapsUploadFB();

  signals:
    /**
      This signal is emitted to return results of free/busy requests.
    */
    void freeBusyRetrieved( KCal::FreeBusy *, const QString &email );

  private:
    FreeBusyManagerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE( FreeBusyManager )
    Q_DISABLE_COPY( FreeBusyManager )
    Q_PRIVATE_SLOT( d_ptr, void slotUploadFreeBusyResult( KJob * ) )
    Q_PRIVATE_SLOT( d_ptr, void processFailedDownload( const KUrl url, const QString &errorMessage ) )
    Q_PRIVATE_SLOT( d_ptr, void processFinishedDownload( const KUrl url, const QByteArray &freeBusyData ) )
    Q_PRIVATE_SLOT( d_ptr, bool processRetrieveQueue() )
};

}

#endif
