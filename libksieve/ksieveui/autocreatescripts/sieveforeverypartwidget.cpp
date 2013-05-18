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

#include "sieveforeverypartwidget.h"

#include <KLocale>
#include <KLineEdit>

#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>

namespace KSieveUi {
SieveForEveryPartWidget::SieveForEveryPartWidget(QWidget *parent)
    : SieveWidgetPageAbstract(parent)
{
    QHBoxLayout *topLayout = new QHBoxLayout;

    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    w->setLayout(lay);
    mForLoop = new QCheckBox(i18n("Add ForEveryPart loop"));
    lay->addWidget(mForLoop);

    QLabel *lab = new QLabel(i18n("Name (optional):"));
    lay->addWidget(lab);

    mName = new KLineEdit;
    lay->addWidget(mName);

    topLayout->addWidget(w,0, Qt::AlignTop);

    setLayout(topLayout);
}

SieveForEveryPartWidget::~SieveForEveryPartWidget()
{

}

void SieveForEveryPartWidget::generatedScript(QString &script, QStringList &requires)
{
    if (mForLoop->isChecked()) {
        requires << QLatin1String("foreverypart");
        const QString loopName = mName->text();
        if (loopName.isEmpty()) {
            script += QLatin1String("foreverypart {");
        } else {
            const QString nameStr = QString::fromLatin1(":name \"%1\"").arg(loopName);
            script += QString::fromLatin1("foreverypart %1 {").arg(nameStr);
        }
    }
}

}

#include "sieveforeverypartwidget.moc"