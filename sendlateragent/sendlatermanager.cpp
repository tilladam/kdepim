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

#include "sendlatermanager.h"
#include "sendlaterinfo.h"

#include <KSharedConfig>
#include <KConfigGroup>
#include <KGlobal>

#include <QStringList>

SendLaterManager::SendLaterManager(QObject *parent)
    : QObject(parent)
{
    mConfig = KGlobal::config();
}

SendLaterManager::~SendLaterManager()
{
    qDeleteAll(mListSendLaterInfo);
}

void SendLaterManager::load()
{
    qDeleteAll(mListSendLaterInfo);
    mListSendLaterInfo.clear();

    const QStringList itemList = mConfig->groupList().filter( QRegExp( QLatin1String("SendLaterItem \\d+") ) );
    const int numberOfItems = itemList.count();
    for(int i = 0 ; i < numberOfItems; ++i) {
        KConfigGroup group = mConfig->group(itemList.at(i));
        SendLaterInfo *info = new SendLaterInfo(group);
        //TODO

    }
}

void SendLaterManager::sendDone()
{
    //TODO
    //Remove item if not recursive.

}

#include "sendlatermanager.moc"