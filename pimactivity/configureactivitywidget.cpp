/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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

#include "configureactivitywidget.h"

#include <KTabWidget>
#include <KLocale>

#include <QHBoxLayout>
#include <QCheckBox>

namespace PimActivity {

class ConfigureActivityWidgetPrivate {
public:
    ConfigureActivityWidgetPrivate(ConfigureActivityWidget * qq)
        : q(qq), tabWidget( 0 )
    {
        QHBoxLayout * lay = new QHBoxLayout;
        activateActivity = new QCheckBox(i18n("Enable activity"));
        lay->addWidget(activateActivity);
        tabWidget = new KTabWidget;
        lay->addWidget(tabWidget);
        q->setLayout(lay);
    }
    ConfigureActivityWidget *q;
    QCheckBox *activateActivity;
    KTabWidget *tabWidget;
};

ConfigureActivityWidget::ConfigureActivityWidget(QWidget *parent)
    : QWidget(parent), d(new ConfigureActivityWidgetPrivate(this))
{
}

ConfigureActivityWidget::~ConfigureActivityWidget()
{
    delete d;
}

}


#include "configureactivitywidget.moc"
