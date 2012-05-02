/*
  Copyright (c) 2012 Montel Laurent <montel@kde.org>

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

#ifndef BACKUPDATA_H
#define BACKUPDATA_H

#include <QObject>
#include "util.h"
#include "abstractdata.h"
namespace KPIMIdentities {
  class Identity;
  class IdentityManager;
}

class BackupData : public AbstractData
{
  Q_OBJECT
public:
  explicit BackupData(Util::BackupTypes typeSelected,const QString& filename);
  ~BackupData();
private:
  void backupTransports();
  void backupResources();
  void backupMails();
  void backupConfig();
  void backupIdentity();
  void backupAkonadiDb();

  qint64 writeFile(const char* data, qint64 len);
  KPIMIdentities::IdentityManager *mIdentityManager;

};

#endif // BACKUPDATA_H
