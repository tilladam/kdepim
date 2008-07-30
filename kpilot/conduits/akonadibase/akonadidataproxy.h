#ifndef AKONADIDATAPROXY_H
#define AKONADIDATAPROXY_H
/* akonadidataproxy.h			KPilot
**
** Copyright (C) 2008 by Bertjan Broeksema <b.broeksema@kdemail.net>
** Copyright (C) 2008 by Jason "vanRijn" Kasper <vr@movingparts.net>
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
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

#include "dataproxy.h"

#include <akonadi/item.h>
#include <QtCore/QDateTime>

class AkonadiDataProxyPrivate;
class AkonadiRecord;
class IDMapping;

class KPILOT_EXPORT AkonadiDataProxy : public DataProxy
{
public:
	AkonadiDataProxy( Akonadi::Entity::Id id, const IDMapping& mapping  );
	
	/* virtual */ ~AkonadiDataProxy();

	/**
	 * Returns true when the proxy was able to open the underlying data store 
	 * in read/write mode.
	 */
	/* virtual */ bool isOpen() const;

	/**
	 * Loads all records from underlying data source, sets the startcount of the
	 * counter and resets the iterator.
	 */
	/* virtual */ void loadAllRecords();

	/**
	 * Notifies the proxy that the synchronization is finished and that
	 * no modifications will be done after this.
	 */
	/* virtual */ void syncFinished();

protected: // Functions
	/**
	 * Generates a unique id for a new record.
	 */
	/* virtual */ QString generateUniqueId();
	
	/**
	 * Commits created record @p rec to the datastore. Sets the id that the
	 * data store created for this record to rec.
	 */
	/* virtual */ void commitCreate( Record *rec );
	
	/**
	 * Commits updated record @p rec to the datastore. Must return rec->id() even
	 * if it's unchanged.
	 */
	/* virtual */ void commitUpdate( Record *rec );
	
	/**
	 * Delete record @p rec from the datastore.
	 */
	/* virtual */ void commitDelete( Record *rec );

	/**
	 * Creates a new akonadi record for @param i and sets the last sync time
	 * @param dt to the record.
	 */
  virtual AkonadiRecord* createAkonadiRecord( const Akonadi::Item& i
                                            , const QDateTime& dt ) const = 0;
	
	/**
	 * Creates a dummy record with given id. These are for the records that where
	 * deleted from the collection after the last sync. The returned record
	 * <em>must</em> must return true on both modified() and isDeleted().
	 */
	virtual AkonadiRecord* createDeletedAkonadiRecord( const QString& id ) const = 0;

private:
	QSharedDataPointer<AkonadiDataProxyPrivate> d;
};

#endif
