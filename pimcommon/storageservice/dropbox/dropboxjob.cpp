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


#include "dropboxjob.h"
#include "storageservice/storageauthviewdialog.h"
#include "storageservice/storageserviceabstract.h"
#include "storageservice/storageserviceutils.h"
#include <KLocalizedString>

#include <qjson/parser.h>
#include <QFile>
#include <QFileInfo>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include <QPointer>
#include <QFile>

using namespace PimCommon;

DropBoxJob::DropBoxJob(QObject *parent)
    : PimCommon::StorageServiceAbstractJob(parent)
{
    mOauthconsumerKey = QLatin1String("e40dvomckrm48ci");
    mOauthSignature = QLatin1String("0icikya464lny9g&");
    mOauthVersion = QLatin1String("1.0");
    mOauthSignatureMethod = QLatin1String("PLAINTEXT");
    mTimestamp = QString::number(QDateTime::currentMSecsSinceEpoch()/1000);
    mNonce = PimCommon::StorageServiceUtils::generateNonce(8);
    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotSendDataFinished(QNetworkReply*)));
}

DropBoxJob::~DropBoxJob()
{

}

void DropBoxJob::initializeToken(const QString &accessToken, const QString &accessTokenSecret, const QString &accessOauthSignature)
{
    mOauthToken = accessToken;
    mOauthTokenSecret = accessTokenSecret;
    mAccessOauthSignature = accessOauthSignature;
}

void DropBoxJob::requestTokenAccess()
{
    mActionType = RequestToken;
    QNetworkRequest request(QUrl(QLatin1String("https://api.dropbox.com/1/oauth/request_token")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    QUrl postData;

    postData.addQueryItem(QLatin1String("oauth_consumer_key"), mOauthconsumerKey);
    postData.addQueryItem(QLatin1String("oauth_nonce"), mNonce);
    postData.addQueryItem(QLatin1String("oauth_signature"), mOauthSignature);
    postData.addQueryItem(QLatin1String("oauth_signature_method"),mOauthSignatureMethod);
    postData.addQueryItem(QLatin1String("oauth_timestamp"), mTimestamp);
    postData.addQueryItem(QLatin1String("oauth_version"), mOauthVersion);
    qDebug()<<" https://api.dropbox.com/1/oauth/request_token"<<postData;

    QNetworkReply *reply = mNetworkAccessManager->post(request, postData.encodedQuery());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::getTokenAccess()
{
    mActionType = AccessToken;
    mError = false;
    QNetworkRequest request(QUrl(QLatin1String("https://api.dropbox.com/1/oauth/access_token")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    QUrl postData;

    postData.addQueryItem(QLatin1String("oauth_consumer_key"), mOauthconsumerKey);
    qDebug()<<" mAccessOauthSignature"<<mAccessOauthSignature;
    postData.addQueryItem(QLatin1String("oauth_nonce"), mNonce);
    postData.addQueryItem(QLatin1String("oauth_signature"), mAccessOauthSignature);
    postData.addQueryItem(QLatin1String("oauth_signature_method"),mOauthSignatureMethod);
    postData.addQueryItem(QLatin1String("oauth_timestamp"), mTimestamp);
    postData.addQueryItem(QLatin1String("oauth_version"), mOauthVersion);
    postData.addQueryItem(QLatin1String("oauth_token"), mOauthToken);
    qDebug()<<"getTokenAccess  postdata"<<postData;

    QNetworkReply *reply = mNetworkAccessManager->post(request, postData.encodedQuery());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::slotSendDataFinished(QNetworkReply *reply)
{
    const QString data = QString::fromUtf8(reply->readAll());
    reply->deleteLater();
    if (mError) {
        QJson::Parser parser;
        bool ok;

        QMap<QString, QVariant> error = parser.parse(data.toUtf8(), &ok).toMap();
        if (error.contains(QLatin1String("error"))) {
            const QString errorStr = error.value(QLatin1String("error")).toString();
            switch(mActionType) {
            case NoneAction:
                deleteLater();
                break;
            case RequestToken:
                Q_EMIT authorizationFailed(errorStr);
                deleteLater();
                break;
            case AccessToken:
                Q_EMIT authorizationFailed(errorStr);
                deleteLater();
                break;
            case UploadFiles:
                errorMessage(mActionType, errorStr);
                deleteLater();
                break;
            case CreateFolder:
                errorMessage(mActionType, errorStr);
                deleteLater();
                break;
            case AccountInfo:
                errorMessage(mActionType, errorStr);
                deleteLater();
                break;
            case ListFolder:
                errorMessage(mActionType, errorStr);
                deleteLater();
                break;
            case ShareLink:
                errorMessage(mActionType, errorStr);
                deleteLater();
                break;
            case CreateServiceFolder:
                deleteLater();
                break;
            default:
                qDebug()<<" Action Type unknown:"<<mActionType;
                deleteLater();
                break;
            }
        }
        return;
    }
    switch(mActionType) {
    case NoneAction:
        break;
    case RequestToken:
        parseRequestToken(data);
        break;
    case AccessToken:
        parseResponseAccessToken(data);
        break;
    case UploadFiles:
        parseUploadFile(data);
        break;
    case CreateFolder:
        parseCreateFolder(data);
        break;
    case AccountInfo:
        parseAccountInfo(data);
        break;
    case ListFolder:
        parseListFolder(data);
        break;
    case ShareLink:
        parseShareLink(data);
        break;
    case CreateServiceFolder:
        deleteLater();
        break;
    default:
        qDebug()<<" Action Type unknown:"<<mActionType;
    }
}

void DropBoxJob::parseAccountInfo(const QString &data)
{
    QJson::Parser parser;
    bool ok;

    QMap<QString, QVariant> info = parser.parse(data.toUtf8(), &ok).toMap();
    PimCommon::AccountInfo accountInfo;
    if (info.contains(QLatin1String("display_name")))
        accountInfo.displayName = info.value(QLatin1String("display_name")).toString();
    if (info.contains(QLatin1String("quota_info"))) {
        QMap<QString, QVariant> quotaInfo = info.value(QLatin1String("quota_info")).toMap();
        if (quotaInfo.contains(QLatin1String("quota"))) {
            accountInfo.quota = quotaInfo.value(QLatin1String("quota")).toLongLong();
        }
        if (quotaInfo.contains(QLatin1String("normal"))) {
            accountInfo.accountSize = quotaInfo.value(QLatin1String("normal")).toLongLong();
        }
        if (quotaInfo.contains(QLatin1String("shared"))) {
            accountInfo.shared = quotaInfo.value(QLatin1String("shared")).toLongLong();
        }
    }


    Q_EMIT accountInfoDone(accountInfo);
    deleteLater();
}

void DropBoxJob::parseResponseAccessToken(const QString &data)
{
    if(data.contains(QLatin1String("error"))) {
        qDebug()<<" return error !";
        Q_EMIT authorizationFailed(data);
    } else {
        QStringList split           = data.split(QLatin1Char('&'));
        QStringList tokenSecretList = split.at(0).split(QLatin1Char('='));
        mOauthTokenSecret          = tokenSecretList.at(1);
        QStringList tokenList       = split.at(1).split(QLatin1Char('='));
        mOauthToken = tokenList.at(1);
        mAccessOauthSignature = mOauthSignature + mOauthTokenSecret;

        Q_EMIT authorizationDone(mOauthToken, mOauthTokenSecret, mAccessOauthSignature);
    }
    deleteLater();
}

void DropBoxJob::parseRequestToken(const QString &result)
{
    const QStringList split = result.split(QLatin1Char('&'));
    if (split.count() == 2) {
        const QStringList tokenSecretList = split.at(0).split(QLatin1Char('='));
        mOauthTokenSecret = tokenSecretList.at(1);
        const QStringList tokenList = split.at(1).split(QLatin1Char('='));
        mOauthToken = tokenList.at(1);
        mAccessOauthSignature = mOauthSignature + mOauthTokenSecret;

        qDebug()<<" mOauthToken" <<mOauthToken<<"mAccessOauthSignature "<<mAccessOauthSignature<<" mOauthSignature"<<mOauthSignature;

    } else {
        qDebug()<<" data is not good: "<<result;
    }
    doAuthentification();
}

void DropBoxJob::doAuthentification()
{
    QUrl url(QLatin1String("https://api.dropbox.com/1/oauth/authorize"));
    url.addQueryItem(QLatin1String("oauth_token"), mOauthToken);
    QPointer<StorageAuthViewDialog> dlg = new StorageAuthViewDialog;
    dlg->setUrl(url);
    if (dlg->exec()) {
        getTokenAccess();
        delete dlg;
    } else {
        Q_EMIT authorizationFailed(i18n("Authentication Canceled."));
        delete dlg;
        deleteLater();
    }
}

void DropBoxJob::createFolder(const QString &folder)
{
    mActionType = CreateFolder;
    mError = false;
    QUrl url(QLatin1String("https://api.dropbox.com/1/fileops/create_folder"));
    url.addQueryItem(QLatin1String("root"), QLatin1String("dropbox"));
    url.addQueryItem(QLatin1String("path"), folder );
    url.addQueryItem(QLatin1String("oauth_consumer_key"),mOauthconsumerKey );
    url.addQueryItem(QLatin1String("oauth_nonce"), mNonce);
    url.addQueryItem(QLatin1String("oauth_signature"), mAccessOauthSignature.replace(QLatin1Char('&'),QLatin1String("%26")));
    url.addQueryItem(QLatin1String("oauth_signature_method"), mOauthSignatureMethod);
    url.addQueryItem(QLatin1String("oauth_timestamp"), mTimestamp);
    url.addQueryItem(QLatin1String("oauth_version"), mOauthVersion);
    url.addQueryItem(QLatin1String("oauth_token"), mOauthToken);
    qDebug()<<" postData "<<url;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    QNetworkReply *reply = mNetworkAccessManager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::uploadFile(const QString &filename)
{
    QFile *file = new QFile(filename);
    if (file->exists()) {
        mActionType = UploadFiles;
        mError = false;
        QFileInfo info(filename);
        const QString r = mAccessOauthSignature.replace(QLatin1Char('&'),QLatin1String("%26"));
        const QString str = QString::fromLatin1("https://api-content.dropbox.com/1/files_put/dropbox///test/%1?oauth_consumer_key=%2&oauth_nonce=%3&oauth_signature=%4&oauth_signature_method=PLAINTEXT&oauth_timestamp=%6&oauth_version=1.0&oauth_token=%5&overwrite=false").
                arg(info.fileName()).arg(mOauthconsumerKey).arg(mNonce).arg(r).arg(mOauthToken).arg(mTimestamp);
        KUrl url(str);
        QNetworkRequest request(url);
        if (!file->open(QIODevice::ReadOnly)) {
            delete file;
            return;
        } else {
            QNetworkReply *reply = mNetworkAccessManager->put(request, file);
            connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(slotUploadFileProgress(qint64, qint64)));
            file->setParent(reply);
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
        }
    }
}

void DropBoxJob::accountInfo()
{
    mActionType = AccountInfo;
    mError = false;
    QUrl url(QLatin1String("https://api.dropbox.com/1/account/info"));
    url.addQueryItem(QLatin1String("oauth_consumer_key"), mOauthconsumerKey);
    url.addQueryItem(QLatin1String("oauth_nonce"), mNonce);
    url.addQueryItem(QLatin1String("oauth_signature"), mAccessOauthSignature.replace(QLatin1Char('&'),QLatin1String("%26")));
    url.addQueryItem(QLatin1String("oauth_signature_method"),mOauthSignatureMethod);
    url.addQueryItem(QLatin1String("oauth_timestamp"), mTimestamp);
    url.addQueryItem(QLatin1String("oauth_version"), mOauthVersion);
    url.addQueryItem(QLatin1String("oauth_token"), mOauthToken);
    qDebug()<<" accountInfo "<<url;
    QNetworkRequest request(url);
    qDebug()<<" url "<<url;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    QNetworkReply *reply = mNetworkAccessManager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::listFolder(const QString &folder)
{
    qDebug()<<" void DropBoxJob::listFolders()";
    mActionType = ListFolder;
    mError = false;
    QUrl url(QLatin1String("https://api.dropbox.com/1/metadata/dropbox/"));
    url.addQueryItem(QLatin1String("oauth_consumer_key"),mOauthconsumerKey);
    url.addQueryItem(QLatin1String("oauth_nonce"), nonce);
    url.addQueryItem(QLatin1String("oauth_signature"), mAccessOauthSignature.replace(QLatin1Char('&'),QLatin1String("%26")));
    url.addQueryItem(QLatin1String("oauth_signature_method"),mOauthSignatureMethod);
    url.addQueryItem(QLatin1String("oauth_timestamp"), mTimestamp);
    url.addQueryItem(QLatin1String("oauth_version"),mOauthVersion);
    url.addQueryItem(QLatin1String("oauth_token"),mOauthToken);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    QNetworkReply *reply = mNetworkAccessManager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::slotUploadFileProgress(qint64 done, qint64 total)
{
    qDebug()<<" done "<<done<<" total :"<<total;
    Q_EMIT uploadFileProgress(done, total);
}

void DropBoxJob::parseUploadFile(const QString &data)
{
    QJson::Parser parser;
    bool ok;

    QMap<QString, QVariant> info = parser.parse(data.toUtf8(), &ok).toMap();
    //qDebug()<<" info "<<info;
    QString root;
    QString path;
    if (info.contains(QLatin1String("root"))) {
        root = info.value(QLatin1String("root")).toString();
        //qDebug()<<" root "<<root;
    }
    if (info.contains(QLatin1String("path"))) {
        path = info.value(QLatin1String("path")).toString();
        //qDebug()<<" path "<<path;
    }
    //TODO
    Q_EMIT uploadFileDone(path);
    shareLink(root, path);
}

void DropBoxJob::shareLink(const QString &root, const QString &path)
{
    mActionType = ShareLink;
    //QNetworkRequest request(QUrl(QLatin1String("https://api.dropbox.com/1/shares/") + root + QLatin1Char('/') + path));
    //request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    const QString r = mAccessOauthSignature.replace(QLatin1Char('&'),QLatin1String("%26"));
    const QString str = QString::fromLatin1("https://api.dropbox.com/1/shares///%1/%2?oauth_consumer_key=%3&oauth_nonce=%4&oauth_signature=%5&oauth_signature_method=PLAINTEXT&oauth_timestamp=%6&oauth_version=1.0&oauth_token=%6").
            arg(root).arg(path).arg(mOauthconsumerKey).arg(mNonce).arg(r).arg(mOauthToken).arg(mTimestamp);
    KUrl url(str);
    qDebug()<<" url"<<url;
    QNetworkRequest request(url);

    QNetworkReply *reply = mNetworkAccessManager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void DropBoxJob::parseShareLink(const QString &data)
{
    QJson::Parser parser;
    bool ok;
    QString url;
    QMap<QString, QVariant> info = parser.parse(data.toUtf8(), &ok).toMap();
    if (info.contains(QLatin1String("url"))) {
        url = info.value(QLatin1String("url")).toString();
    }
    qDebug()<<" info "<<info;

    Q_EMIT shareLinkDone(url);
    deleteLater();
}

void DropBoxJob::parseCreateFolder(const QString &data)
{
    qDebug()<<" data "<<data;
    QJson::Parser parser;
    bool ok;
    QMap<QString, QVariant> info = parser.parse(data.toUtf8(), &ok).toMap();
    QString foldername;
    if (info.contains(QLatin1String("path"))) {
        foldername = info.value(QLatin1String("path")).toString();
    }
    Q_EMIT createFolderDone(foldername);
    deleteLater();
}

void DropBoxJob::parseListFolder(const QString &data)
{
    //TODO
    Q_EMIT listFolderDone(QStringList());
    deleteLater();
}
