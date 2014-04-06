/*
  Copyright (c) 2014 Montel Laurent <montel@kde.org>

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

#include "openattachmentfolderwidget.h"

#include <KLocalizedString>
#include <KAction>
#include <KRun>

#include <QTimer>

using namespace MessageViewer;

OpenAttachmentFolderWidget::OpenAttachmentFolderWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    mTimer->setInterval(5000); // 5 seconds
    connect(mTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Positive);
    setWordWrap(true);
    KAction *action = new KAction( i18n( "Open folder where attachment was saved" ), this );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotOpenAttachmentFolder()) );
    addAction( action );
}

OpenAttachmentFolderWidget::~OpenAttachmentFolderWidget()
{

}

void OpenAttachmentFolderWidget::setFolder(const KUrl &url)
{
    mUrl = url;
}

void OpenAttachmentFolderWidget::slotOpenAttachmentFolder()
{
    if (!mUrl.isEmpty()) {
        new KRun( mUrl, this );
        slotHideWarning();
    }
}

void OpenAttachmentFolderWidget::slotHideWarning()
{
    if (mTimer->isActive())
        mTimer->stop();
    animatedHide();
}


void OpenAttachmentFolderWidget::slotShowWarning()
{
    if (mTimer->isActive())
        mTimer->stop();
    mTimer->start();
    animatedShow();
}

void OpenAttachmentFolderWidget::slotTimeOut()
{
    animatedHide();
}
