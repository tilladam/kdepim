/* -*- mode: c++; c-basic-offset:4 -*-
    conf/dirservconfigpage.h

    This file is part of Kleopatra, the KDE keymanager
    Copyright (c) 2004,2008 Klarälvdalens Datakonsult AB

    Kleopatra is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Kleopatra is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#ifndef DIRSERVCONFIGPAGE_H
#define DIRSERVCONFIGPAGE_H

#include <kcmodule.h>

#include "libkleo/kleo/cryptoconfig.h"

class QCheckBox;
class QLabel;
class QTimeEdit;
class KIntNumInput;
namespace Kleo {
  class CryptoConfig;
  class CryptoConfigEntry;
  class DirectoryServicesWidget;
}

/**
 * "Directory Services" configuration page for kleopatra's configuration dialog
 * The user can configure LDAP servers in this page, to be used for listing/fetching
 * remote certificates in kleopatra.
 */
class DirectoryServicesConfigurationPage : public KCModule {
  Q_OBJECT
public:
  explicit DirectoryServicesConfigurationPage( const KComponentData &instance, QWidget *parent=0, const QVariantList &args=QVariantList() );

  /* reimp */ void load();
  /* reimp */ void save();
  /* reimp */ void defaults();

private:
  Kleo::CryptoConfigEntry* configEntry( const char* componentName,
                                        const char* groupName,
                                        const char* entryName,
                                        Kleo::CryptoConfigEntry::ArgType argType,
                                        bool isList,
                                        bool showError=true );

  Kleo::DirectoryServicesWidget* mWidget;
  QTimeEdit* mTimeout;
  KIntNumInput* mMaxItems;
  QLabel* mMaxItemsLabel;
  QCheckBox* mAddNewServersCB;

  Kleo::CryptoConfigEntry* mX509ServicesEntry;
  Kleo::CryptoConfigEntry* mOpenPGPServiceEntry;
  Kleo::CryptoConfigEntry* mTimeoutConfigEntry;
  Kleo::CryptoConfigEntry* mMaxItemsConfigEntry;
  Kleo::CryptoConfigEntry* mAddNewServersConfigEntry;

  Kleo::CryptoConfig* mConfig;
};

#endif
