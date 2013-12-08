/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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

#include "storageserviceabstractjob.h"
#include <QNetworkAccessManager>
#include <QDebug>

using namespace PimCommon;

StorageServiceAbstractJob::StorageServiceAbstractJob(QObject *parent)
    : QObject(parent),
      mNetworkAccessManager(new QNetworkAccessManager(this)),
      mActionType(NoneAction),
      mError(false)
{
    qDebug()<<"StorageServiceAbstractJob::StorageServiceAbstractJob() "<<this;
}

StorageServiceAbstractJob::~StorageServiceAbstractJob()
{
    qDebug()<<"StorageServiceAbstractJob::~StorageServiceAbstractJob() "<<this;
}

void StorageServiceAbstractJob::slotError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug()<<" Error "<<error<<" reply"<<reply->errorString();
    mError = true;
}