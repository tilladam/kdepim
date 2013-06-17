/*
  Copyright (c) 2012-2013 Montel Laurent <montel@kde.org>
  
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

#include "exportalarmjob.h"

#include "messageviewer/utils/kcursorsaver.h"

#include <Akonadi/AgentManager>

#include <KLocale>
#include <KStandardDirs>
#include <KTemporaryFile>

#include <QWidget>
#include <QFile>
#include <QDir>

ExportAlarmJob::ExportAlarmJob(QWidget *parent, BackupMailUtil::BackupTypes typeSelected, ArchiveStorage *archiveStorage,int numberOfStep)
    : AbstractImportExportJob(parent, archiveStorage, typeSelected, numberOfStep)
{
}

ExportAlarmJob::~ExportAlarmJob()
{

}

void ExportAlarmJob::start()
{
    mArchiveDirectory = archive()->directory();
    if (mTypeSelected & BackupMailUtil::Resources) {
        backupResources();
        increaseProgressDialog();
        if (wasCanceled()) {
            return;
        }
    }
    if (mTypeSelected & BackupMailUtil::Config) {
        backupConfig();
        increaseProgressDialog();
        if (wasCanceled()) {
            return;
        }
    }
}


void ExportAlarmJob::backupResources()
{
    showInfo(i18n("Backing up resources..."));
    MessageViewer::KCursorSaver busy( MessageViewer::KBusyPtr::busy() );

    Akonadi::AgentManager *manager = Akonadi::AgentManager::self();
    const Akonadi::AgentInstance::List list = manager->instances();
    foreach( const Akonadi::AgentInstance &agent, list ) {
        const QString identifier = agent.identifier();
        if (identifier.contains(QLatin1String("akonadi_kalarm_resource_"))) {
            const QString identifier = agent.identifier();
            const QString archivePath = BackupMailUtil::alarmPath() + identifier + QDir::separator();

            KUrl url = BackupMailUtil::resourcePath(agent);
            if (!url.isEmpty()) {
                const QString filename = url.fileName();
                const bool fileAdded  = archive()->addLocalFile(url.path(), archivePath + filename);
                if (fileAdded) {
                    const QString errorStr = BackupMailUtil::storeResources(archive(), identifier, archivePath);
                    if (!errorStr.isEmpty())
                        Q_EMIT error(errorStr);
                    Q_EMIT info(i18n("\"%1\" was backuped.",filename));
                }
                else
                    Q_EMIT error(i18n("\"%1\" file cannot be added to backup file.",filename));
            }
        }
    }

    Q_EMIT info(i18n("Resources backup done."));
}

void ExportAlarmJob::backupConfig()
{
    showInfo(i18n("Backing up config..."));
    MessageViewer::KCursorSaver busy( MessageViewer::KBusyPtr::busy() );
    const QString kalarmStr(QLatin1String("kalarmrc"));
    const QString kalarmrc = KStandardDirs::locateLocal( "config", kalarmStr);
    if (QFile(kalarmrc).exists()) {
        KSharedConfigPtr kalarm = KSharedConfig::openConfig(kalarmrc);

        KTemporaryFile tmp;
        tmp.open();

        KConfig *kalarmConfig = kalarm->copyTo( tmp.fileName() );

        //TODO adapt collection
        kalarmConfig->sync();
        backupFile(tmp.fileName(), BackupMailUtil::configsPath(), kalarmStr);
    }


    Q_EMIT info(i18n("Config backup done."));

}

QString ExportAlarmJob::componentName() const
{
    return QLatin1String("KAlarm");
}

#include "exportalarmjob.moc"
