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
#include "archivemailinfo.h"

#include <KLocale>
#include <QDir>

ArchiveMailInfo::ArchiveMailInfo()
    : mLastDateSaved(QDate())
    , mArchiveAge( 1 )
    , mArchiveType( MailCommon::BackupJob::Zip )
    , mArchiveUnit( ArchiveMailInfo::ArchiveDays )
    , mSaveCollectionId(-1)
    , mMaximumArchiveCount(0)
    , mSaveSubCollection(false)
    , mIsEnabled(true)
{
}

ArchiveMailInfo::ArchiveMailInfo(const KConfigGroup &config)
    : mLastDateSaved(QDate())
    , mArchiveAge( 1 )
    , mArchiveType( MailCommon::BackupJob::Zip )
    , mArchiveUnit( ArchiveMailInfo::ArchiveDays )
    , mSaveCollectionId(-1)
    , mMaximumArchiveCount(0)
    , mSaveSubCollection(false)
    , mIsEnabled(true)
{
    readConfig(config);
}

ArchiveMailInfo::ArchiveMailInfo(const ArchiveMailInfo &info)
{
    mLastDateSaved = info.lastDateSaved();
    mArchiveAge = info.archiveAge();
    mArchiveType = info.archiveType();
    mArchiveUnit = info.archiveUnit();
    mSaveCollectionId = info.saveCollectionId();
    mMaximumArchiveCount = info.maximumArchiveCount();
    mSaveSubCollection = info.saveSubCollection();
    mPath = info.url();
    mIsEnabled = info.isEnabled();
}


ArchiveMailInfo::~ArchiveMailInfo()
{
}

ArchiveMailInfo& ArchiveMailInfo::operator=( const ArchiveMailInfo &old )
{
    mLastDateSaved = old.lastDateSaved();
    mArchiveAge = old.archiveAge();
    mArchiveType = old.archiveType();
    mArchiveUnit = old.archiveUnit();
    mSaveCollectionId = old.saveCollectionId();
    mMaximumArchiveCount = old.maximumArchiveCount();
    mSaveSubCollection = old.saveSubCollection();
    mPath = old.url();
    mIsEnabled = old.isEnabled();
    return (*this);
}

QString normalizeFolderName(const QString &folderName)
{
    QString adaptFolderName(folderName);
    adaptFolderName.replace(QLatin1Char('/'),QLatin1Char('_'));
    return adaptFolderName;
}

QString ArchiveMailInfo::dirArchive() const
{
    const QDir dir(url().path());
    QString dirPath = url().path();
    if (!dir.exists()) {
        dirPath = QDir::homePath();
        kDebug()<<" Path doesn't exist"<<dir.path();
    }
    return dirPath;
}

KUrl ArchiveMailInfo::realUrl(const QString &folderName) const
{
    const int numExtensions = 4;
    // The extensions here are also sorted, like the enum order of BackupJob::ArchiveType
    const char *extensions[numExtensions] = { ".zip", ".tar", ".tar.bz2", ".tar.gz" };
    const QString dirPath = dirArchive();

    const QString path = dirPath + QLatin1Char( '/' ) + i18nc( "Start of the filename for a mail archive file" , "Archive" )
            + QLatin1Char( '_' ) + normalizeFolderName(folderName) + QLatin1Char( '_' )
            + QDate::currentDate().toString( Qt::ISODate ) + QString::fromLatin1(extensions[mArchiveType]);
    KUrl real(path);
    return real;
}

QStringList ArchiveMailInfo::listOfArchive(const QString &folderName) const
{
    const int numExtensions = 4;
    // The extensions here are also sorted, like the enum order of BackupJob::ArchiveType
    const char *extensions[numExtensions] = { ".zip", ".tar", ".tar.bz2", ".tar.gz" };
    const QString dirPath = dirArchive();

    QDir dir(dirPath);

    QStringList nameFilters;
    nameFilters << i18nc( "Start of the filename for a mail archive file" , "Archive" ) + QLatin1Char( '_' ) +
                   normalizeFolderName(folderName) + QLatin1Char( '_' ) + QLatin1String("*") + QString::fromLatin1(extensions[mArchiveType]);
    const QStringList lst = dir.entryList ( nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed );
    return lst;
}

bool ArchiveMailInfo::isValid() const
{
    return (mSaveCollectionId!=-1);
}


void ArchiveMailInfo::setArchiveAge( int age )
{
    mArchiveAge = age;
}

int ArchiveMailInfo::archiveAge() const
{
    return mArchiveAge;
}

void ArchiveMailInfo::setArchiveUnit( ArchiveMailInfo::ArchiveUnit unit )
{
    mArchiveUnit = unit;
}

ArchiveMailInfo::ArchiveUnit ArchiveMailInfo::archiveUnit() const
{
    return mArchiveUnit;
}

void ArchiveMailInfo::setArchiveType( MailCommon::BackupJob::ArchiveType type )
{
    mArchiveType = type;
}

MailCommon::BackupJob::ArchiveType ArchiveMailInfo::archiveType() const
{
    return mArchiveType;
}

void ArchiveMailInfo::setLastDateSaved( const QDate &date )
{
    mLastDateSaved = date;
}

QDate ArchiveMailInfo::lastDateSaved() const
{
    return mLastDateSaved;
}

void ArchiveMailInfo::readConfig(const KConfigGroup &config)
{
    mPath = config.readEntry("storePath", KUrl());

    if (config.hasKey(QLatin1String("lastDateSaved"))) {
        mLastDateSaved = QDate::fromString(config.readEntry("lastDateSaved"),Qt::ISODate);
    }
    mSaveSubCollection = config.readEntry("saveSubCollection",false);
    mArchiveType = static_cast<MailCommon::BackupJob::ArchiveType>( config.readEntry( "archiveType", ( int )MailCommon::BackupJob::Zip ) );
    mArchiveUnit = static_cast<ArchiveUnit>( config.readEntry( "archiveUnit", ( int )ArchiveDays ) );
    Akonadi::Collection::Id tId = config.readEntry("saveCollectionId",mSaveCollectionId);
    mArchiveAge = config.readEntry("archiveAge",1);
    mMaximumArchiveCount = config.readEntry("maximumArchiveCount",0);
    if ( tId >= 0 ) {
        mSaveCollectionId = tId;
    }
    mIsEnabled = config.readEntry("enabled", true);
}

void ArchiveMailInfo::writeConfig(KConfigGroup & config )
{
    if (!isValid()) {
        return;
    }
    config.writeEntry("storePath",mPath);

    if (mLastDateSaved.isValid()) {
        config.writeEntry("lastDateSaved", mLastDateSaved.toString(Qt::ISODate) );
    }

    config.writeEntry("saveSubCollection",mSaveSubCollection);
    config.writeEntry("archiveType", ( int )mArchiveType );
    config.writeEntry("archiveUnit", ( int )mArchiveUnit );
    config.writeEntry("saveCollectionId",mSaveCollectionId);
    config.writeEntry("archiveAge",mArchiveAge);
    config.writeEntry("maximumArchiveCount",mMaximumArchiveCount);
    config.writeEntry("enabled",mIsEnabled);
    config.sync();
}

KUrl ArchiveMailInfo::url() const
{
    return mPath;
}

void ArchiveMailInfo::setUrl(const KUrl &url)
{
    mPath = url;
}

bool ArchiveMailInfo::saveSubCollection() const
{
    return mSaveSubCollection;
}

void ArchiveMailInfo::setSaveSubCollection( bool saveSubCol )
{
    mSaveSubCollection = saveSubCol;
}

void ArchiveMailInfo::setSaveCollectionId(Akonadi::Collection::Id collectionId)
{
    mSaveCollectionId = collectionId;
}

Akonadi::Collection::Id ArchiveMailInfo::saveCollectionId() const
{
    return mSaveCollectionId;
}

int ArchiveMailInfo::maximumArchiveCount() const
{
    return mMaximumArchiveCount;
}

void ArchiveMailInfo::setMaximumArchiveCount( int max )
{
    mMaximumArchiveCount = max;
}

bool ArchiveMailInfo::isEnabled() const
{
    return mIsEnabled;
}

void ArchiveMailInfo::setEnabled(bool b)
{
    mIsEnabled = b;
}

