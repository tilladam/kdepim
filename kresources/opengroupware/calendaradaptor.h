 /*
    This file is part of kdepim.

    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2004 Till Adam <adam@kde.org>

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
#ifndef KCAL_CALENDARADAPTOR_H
#define KCAL_CALENDARADAPTOR_H

#include "groupwaredataadaptor.h"


#include <libkcal/incidence.h>

#include <kurl.h>

namespace KPIM {
class GroupwareUploadItem;
};

namespace KCal {
class ResourceCached;
class CalendarAdaptor;

class CalendarUploadItem : public KPIM::GroupwareUploadItem
{
  public:
    CalendarUploadItem( CalendarAdaptor *adaptor, KCal::Incidence *incidence, UploadType type );
    virtual ~CalendarUploadItem() {}

  protected:
    CalendarUploadItem( UploadType type ) : KPIM::GroupwareUploadItem( type ) {}
};


class CalendarAdaptor : public KPIM::GroupwareDataAdaptor
{
  public:
    CalendarAdaptor();

    /**
      Set resource.
    */
    void setResource( KCal::ResourceCached *v )
    {
      mResource = v;
    }
    /**
      Get resource. See setResource().
    */
    KCal::ResourceCached *resource() const
    {
      return mResource;
    }

    virtual QString mimeType() const;
    bool localItemExists( const QString &localId );
    bool localItemHasChanged( const QString &localId );
    void deleteItem( const QString &localId );
    QString addItem( KIO::TransferJob *job, const QString &rawText,
           QString &fingerprint, const QString &localId,
           const QString &storageLocation );
    QString extractUid( KIO::TransferJob *job, const QString &data );
    void clearChange( const QString &uid );

    virtual KCal::Incidence::List parseData( KIO::TransferJob *job,
                                             const QString &rawText );
    virtual KPIM::GroupwareUploadItem *newUploadItem( KCal::Incidence*it,
           KPIM::GroupwareUploadItem::UploadType type );
    virtual void uploadFinished( KIO::TransferJob *trfjob, KPIM::GroupwareUploadItem *item );

  private:
    KCal::ResourceCached *mResource;

    QStringList mAddedItems;
    QStringList mChangedItems;
    QStringList mDeletedItems;
};

};

#endif
