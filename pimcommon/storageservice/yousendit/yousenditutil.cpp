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

#include "yousenditutil.h"
#include <qjson/parser.h>
#include <QVariant>

QStringList PimCommon::YouSendItUtil::getListFolder(const QString &data)
{
    QJson::Parser parser;
    bool ok;
    QStringList listFolder;
    const QMap<QString, QVariant> info = parser.parse(data.toUtf8(), &ok).toMap();
    if (info.contains(QLatin1String("folders"))) {
        QVariantMap mapFolder = info.value(QLatin1String("folders")).toMap();
        QVariantList folders = mapFolder.value(QLatin1String("folder")).toList();
        Q_FOREACH (const QVariant &v, folders) {
            QVariantMap map = v.toMap();
            if (map.contains(QLatin1String("name"))) {
                listFolder.append(map.value(QLatin1String("name")).toString());
            }
        }
    }
    return listFolder;
}