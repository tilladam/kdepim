/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "notealarmattribute.h"

#include <kglobal.h>

#include <QByteArray>

static const KCatalogLoader loader( QLatin1String("libnoteshared") );

using namespace NoteShared;
NoteAlarmAttribute::NoteAlarmAttribute()
    : Akonadi::Attribute()
{

}

NoteAlarmAttribute::~NoteAlarmAttribute()
{

}

NoteAlarmAttribute* NoteAlarmAttribute::clone() const
{
    NoteAlarmAttribute *attr = new NoteAlarmAttribute();
    attr->setDateTime(dateTime());
    return attr;
}

void NoteAlarmAttribute::deserialize(const QByteArray& data)
{
    QDataStream s( data );
    s >> mDateTime;
}

QByteArray NoteAlarmAttribute::serialized() const
{
    QByteArray result;
    QDataStream s( &result, QIODevice::WriteOnly );
    s << mDateTime;
    return result;
}

QByteArray NoteAlarmAttribute::type() const
{
    return "NoteAlarmAttribute";
}

void NoteAlarmAttribute::setDateTime(const KDateTime &dateTime)
{
    mDateTime = dateTime;
}

KDateTime NoteAlarmAttribute::dateTime() const
{
    return mDateTime;
}


