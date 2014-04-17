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
#include "mergecontactinfowidget.h"

#include "kaddressbookgrantlee/widget/grantleecontactviewer.h"

#include <KLocalizedString>

#include <Akonadi/Item>

#include <QHBoxLayout>
#include <QStackedWidget>

using namespace KABMergeContacts;

MergeContactInfoWidget::MergeContactInfoWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout;
    mStackWidget = new QStackedWidget;

    mContactViewer = new KAddressBookGrantlee::GrantleeContactViewer;
    mStackWidget->addWidget(mContactViewer);
    lay->addWidget(mStackWidget);
    setLayout(lay);
}


MergeContactInfoWidget::~MergeContactInfoWidget()
{

}

void MergeContactInfoWidget::setContact(const Akonadi::Item &item)
{
    mContactViewer->setContact(item);
}
