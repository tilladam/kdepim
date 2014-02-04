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

#include "storageservicenavigationbar.h"

#include <KLocalizedString>
#include <KStandardShortcut>
#include <KIcon>

#include <QAction>

StorageServiceNavigationBar::StorageServiceNavigationBar(QWidget *parent)
    : QToolBar(parent)
{
    addAction(KIcon(QLatin1String("go-home")),i18n("Home"), this, SIGNAL(goHome()));
    mGoBack = addAction(KIcon(QLatin1String("go-previous")),i18n("Back"));
    mGoBack->setShortcuts( KStandardShortcut::shortcut(KStandardShortcut::Back) );
    mGoForward = addAction(KIcon(QLatin1String("go-next")),i18n("Forward"));
    mGoForward->setShortcuts( KStandardShortcut::shortcut(KStandardShortcut::Forward) );
}

StorageServiceNavigationBar::~StorageServiceNavigationBar()
{

}
