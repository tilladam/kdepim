/*
  Copyright (c) 2014 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#ifndef STORAGESERVICECONFIGUREDIALOG_H
#define STORAGESERVICECONFIGUREDIALOG_H

#include <KDialog>

namespace PimCommon {
class StorageServiceSettingsWidget;
class StorageServiceAbstract;
}
class KUrlRequester;
class StorageServiceConfigureDialog : public KDialog
{
    Q_OBJECT
public:
    explicit StorageServiceConfigureDialog(QWidget *parent=0);
    ~StorageServiceConfigureDialog();

    QMap<QString, PimCommon::StorageServiceAbstract *> listService() const;
    void setListService(const QMap<QString, PimCommon::StorageServiceAbstract *> &lst);


    void writeSettings();

private:
    void loadSettings();
    void readConfig();
    void writeConfig();
    PimCommon::StorageServiceSettingsWidget *mStorageSettings;
    KUrlRequester *mDownloadFolder;
};

#endif // STORAGESERVICECONFIGUREDIALOG_H