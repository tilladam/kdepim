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

#ifndef OAUTH2JOB_H
#define OAUTH2JOB_H

#include <QObject>
#include "storageservice/job/storageserviceabstractjob.h"
#include <QPointer>
class QNetworkReply;
namespace PimCommon {
class StorageAuthViewDialog;
class OAuth2Job : public PimCommon::StorageServiceAbstractJob
{
    Q_OBJECT
public:
    explicit OAuth2Job(QObject *parent=0);
    ~OAuth2Job();

    void requestTokenAccess();
    QNetworkReply *uploadFile(const QString &filename, const QString &uploadAsName, const QString &destination);
    void listFolder(const QString &folder = QString());
    void accountInfo();
    void createFolder(const QString &foldername, const QString &destination);
    void shareLink(const QString &root, const QString &path);
    void initializeToken(const QString &refreshToken, const QString &token);
    void createServiceFolder();
    QNetworkReply *downloadFile(const QString &name, const QString &fileId, const QString &destination);
    void deleteFile(const QString &filename);
    void deleteFolder(const QString &foldername);
    void renameFolder(const QString &source, const QString &destination);
    void renameFile(const QString &oldName, const QString &newName);
    void moveFolder(const QString &source, const QString &destination);
    void moveFile(const QString &source, const QString &destination);
    void copyFile(const QString &source, const QString &destination);
    void copyFolder(const QString &source, const QString &destination);
    virtual void refreshToken();

Q_SIGNALS:
    void authorizationDone(const QString &refreshToken, const QString &token, qint64 expireTime);

private slots:
    void slotSendDataFinished(QNetworkReply *reply);
    void slotRedirect(const QUrl &url);

protected:
    virtual void parseCreateServiceFolder(const QString &data);
    virtual void parseListFolder(const QString &data);
    virtual void parseRedirectUrl(const QUrl &url);
    virtual void parseAccessToken(const QString &data);
    virtual void getTokenAccess(const QString &authorizeCode);
    virtual void parseUploadFile(const QString &data);
    virtual void parseCreateFolder(const QString &data);
    virtual void parseAccountInfo(const QString &data);
    virtual void parseDeleteFile(const QString &data);
    virtual void parseDeleteFolder(const QString &data);
    virtual void parseCopyFile(const QString &data);
    virtual void parseCopyFolder(const QString &data);
    virtual void parseRenameFile(const QString &data);
    virtual void parseRenameFolder(const QString &data);
    virtual void parseMoveFolder(const QString &data);
    virtual void parseMoveFile(const QString &data);
    virtual void parseShareLink(const QString &data);
    virtual void parseDownloadFile(const QString &data);

    QString mServiceUrl;
    QUrl mAuthUrl;
    QString mClientId;
    QString mClientSecret;
    QString mRedirectUri;
    QString mRefreshToken;
    QString mToken;
    QString mScope;
    QString mAuthorizePath;
    QString mPathToken;
    QString mFolderInfoPath;
    QString mCurrentAccountInfoPath;
    QString mApiUrl;
    QString mFileInfoPath;
    QPointer<PimCommon::StorageAuthViewDialog> mAuthDialog;

private:
    void shareLink(const QString &fileId);
};
}

#endif // OAUTH2JOB_H