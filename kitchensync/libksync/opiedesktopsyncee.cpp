/*
    This file is part of KitchenSync.

    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2002 Holger Freyther <zecke@handhelds.org>

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

#include "opiedesktopsyncee.h"

using namespace KSync;

OpieDesktopSyncEntry::OpieDesktopSyncEntry( const QStringList& category,
                                            const QString& file,
                                            const QString& name,
                                            const QString& type,
                                            const QString& size,
                                            Syncee *parent )
    : SyncEntry( parent ), mCategory( category ),  mFile( file ),
      mName( name ), mType( type ), mSize( size )
{
}

OpieDesktopSyncEntry::OpieDesktopSyncEntry( const OpieDesktopSyncEntry& opie )
    : SyncEntry( opie )
{
    mName = opie.mName;
    mType = opie.mType;
    mSize = opie.mSize;
    mFile = opie.mFile;
    mCategory = opie.mCategory;
}

OpieDesktopSyncEntry::~OpieDesktopSyncEntry()
{
}

QString OpieDesktopSyncEntry::name()
{
    return mName;
}

QString OpieDesktopSyncEntry::file() const
{
    return mFile;
}

QString OpieDesktopSyncEntry::fileType() const
{
    return mType;
}

QString OpieDesktopSyncEntry::size() const
{
    return mSize;
}

QStringList OpieDesktopSyncEntry::category() const
{
    return mCategory;
}

QString OpieDesktopSyncEntry::id()
{
    return mFile;
}

QString OpieDesktopSyncEntry::type() const
{
    return QString::fromLatin1("OpieDesktopSyncEntry");
}

QString OpieDesktopSyncEntry::timestamp()
{
    return QString::null;
}

bool OpieDesktopSyncEntry::equals( SyncEntry* entry )
{
    OpieDesktopSyncEntry* opEntry;
    opEntry = dynamic_cast<OpieDesktopSyncEntry*> (entry );
    if (opEntry == 0 )
        return false;
    if ( mFile == opEntry->mFile &&
         mName == opEntry->mName &&
         mType == opEntry->mType &&
         mSize == opEntry->mSize &&
         mCategory == opEntry->mCategory )
        return true;
    else
        return false;
}

SyncEntry* OpieDesktopSyncEntry::clone()
{
    return new OpieDesktopSyncEntry( *this );
}

OpieDesktopSyncee::OpieDesktopSyncee()
    : Syncee()
{
    mList.setAutoDelete( true );
}

OpieDesktopSyncee::~OpieDesktopSyncee()
{
}

QString OpieDesktopSyncee::type() const
{
    return QString::fromLatin1("OpieDesktopSyncee");
}

Syncee* OpieDesktopSyncee::clone()
{
    OpieDesktopSyncee* syncee = new OpieDesktopSyncee();
    syncee->setSyncMode( syncMode() );
    syncee->setFirstSync( firstSync() );
    syncee->setSupports( bitArray() );
    syncee->setSource( source() );
    OpieDesktopSyncEntry* entry;
    for ( entry = mList.first(); entry != 0; entry =mList.next() ) {
        syncee->addEntry( entry->clone() );
    }
    return syncee;
}

void OpieDesktopSyncee::addEntry( SyncEntry* entry )
{
    OpieDesktopSyncEntry* opEntry;
    opEntry = dynamic_cast<OpieDesktopSyncEntry*> (entry );
    if (opEntry == 0l )
        return;
    opEntry->setSyncee( this);
    mList.append( opEntry );
}

void OpieDesktopSyncee::removeEntry( SyncEntry* entry )
{
    OpieDesktopSyncEntry* opEntry;
    opEntry = dynamic_cast<OpieDesktopSyncEntry*> (entry );
    if ( opEntry == 0l )
        return;
    mList.remove( opEntry ); // is the case useless?
}

SyncEntry* OpieDesktopSyncee::firstEntry()
{
    return mList.first();
}

SyncEntry* OpieDesktopSyncee::nextEntry()
{
    return mList.next();
}

SyncEntry::PtrList OpieDesktopSyncee::added()
{
    return voidi();
}

SyncEntry::PtrList OpieDesktopSyncee::modified()
{
    return voidi();
}

SyncEntry::PtrList OpieDesktopSyncee::removed()
{
    return voidi();
}

SyncEntry::PtrList OpieDesktopSyncee::voidi()
{
    SyncEntry::PtrList list;
    return list;
}
