/*
    This file is part of libkpimexchange
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>

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
#ifndef KDEPIM_EXCHANGE_CLIENT_H
#define KDEPIM_EXCHANGE_CLIENT_H

#include <qstring.h>
#include <kio/job.h>

#include <libkcal/event.h>
#include <libkcal/icalformat.h>
#include <libkcal/incidence.h>

class DwString;
class DwEntity;
class KCal::Calendar;

namespace KPIM {

class ExchangeAccount;
class ExchangeDownload;
class ExchangeUpload;
class ExchangeDelete;

class ExchangeClient : public QObject {
    Q_OBJECT
  public:
    ExchangeClient( ExchangeAccount* account );
    ~ExchangeClient();

    /**
     * Associate this client with a window given by @p window.
     */
    void setWindow(QWidget *window);

    /**
     * Returns the window this client is associated with.
     */
    QWidget *window() const;
        
    // synchronous functions
    enum { ResultOK, UnknownError };

    int downloadSynchronous( KCal::Calendar* calendar, QDate& start, QDate& end, bool showProgress);
    int uploadSynchronous( KCal::Event* event );
    int removeSynchronous( KCal::Event* event );

    // Will be removed in the near future
    QPtrList<KCal::Event> events( KCal::Calendar* calendar, const QDate& qd );

  public slots:
    void download( KCal::Calendar* calendar, QDate& start, QDate& end, bool showProgress);
    void upload( KCal::Event* event );
    void remove( KCal::Event* event );
    void test();

  private slots:
    void slotDownloadFinished( ExchangeDownload* worker );
    void slotUploadFinished( ExchangeUpload* worker );
    void slotRemoveFinished( ExchangeDelete* worker );
    void slotSyncFinished( int result );
    void slotTestResult( KIO::Job * job );

  signals:
    void startDownload();
    void finishDownload();

    // Don't rely on the result for now - it's probably lying everything's OK
    // even when it isn't
    void uploadFinished( int result );
    void downloadFinished( int result );
    void removeFinished( int result );

  private:
    void test2();

    enum { WaitingForResult, HaveResult };
   
    int mClientState;
    int mSyncResult;
    QWidget* mWindow;
    ExchangeAccount* mAccount;
};

}

#endif
