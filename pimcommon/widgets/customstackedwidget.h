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

#ifndef CUSTOMSTACKEDWIDGET_H
#define CUSTOMSTACKEDWIDGET_H

#include "pimcommon/pimcommon_export.h"

#include <QWidget>

namespace PimCommon {
class PIMCOMMON_EXPORT CustomStackedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomStackedWidget(QWidget *parent=0);
    ~CustomStackedWidget();
};
}
#endif // CUSTOMSTACKEDWIDGET_H