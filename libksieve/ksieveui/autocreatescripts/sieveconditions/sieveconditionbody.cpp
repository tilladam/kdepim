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

#include "sieveconditionbody.h"
#include "widgets/selectbodytypewidget.h"
#include "widgets/selectmatchtypecombobox.h"

#include <KLocale>
#include <KLineEdit>

#include <QWidget>
#include <QHBoxLayout>
#include <QDebug>

using namespace KSieveUi;
SieveConditionBody::SieveConditionBody(QObject *parent)
    : SieveCondition(QLatin1String("body"), i18n("Body"), parent)
{
}

SieveCondition *SieveConditionBody::newAction()
{
    return new SieveConditionBody;
}

QWidget *SieveConditionBody::createParamWidget( QWidget *parent ) const
{
    QWidget *w = new QWidget(parent);
    QHBoxLayout *lay = new QHBoxLayout;
    lay->setMargin(0);
    w->setLayout(lay);

    SelectBodyTypeWidget *bodyType = new SelectBodyTypeWidget;
    bodyType->setObjectName(QLatin1String("bodytype"));
    lay->addWidget(bodyType);

    SelectMatchTypeComboBox *matchType = new SelectMatchTypeComboBox;
    lay->addWidget(matchType);
    matchType->setObjectName(QLatin1String("matchtype"));

    KLineEdit *edit = new KLineEdit;
    edit->setClearButtonShown(true);
    lay->addWidget(edit);
    edit->setObjectName(QLatin1String("edit"));

    return w;
}

QString SieveConditionBody::code(QWidget *w) const
{
    SelectBodyTypeWidget *bodyType =  w->findChild<SelectBodyTypeWidget*>( QLatin1String("bodytype") );
    const QString bodyValue = bodyType->code();
    SelectMatchTypeComboBox *matchType = w->findChild<SelectMatchTypeComboBox*>( QLatin1String("matchtype"));
    bool isNegative = false;
    const QString matchValue = matchType->code(isNegative);


    KLineEdit *edit = w->findChild<KLineEdit*>( QLatin1String("edit"));
    const QString editValue = edit->text();
    return (isNegative ? QLatin1String("not ") : QString()) + QString::fromLatin1("body %1 %2 \"%3\"").arg(bodyValue).arg(matchValue).arg(editValue);
}

QStringList SieveConditionBody::needRequires() const
{
    return QStringList() << QLatin1String("body");
}

bool SieveConditionBody::needCheckIfServerHasCapability() const
{
    return true;
}

QString SieveConditionBody::serverNeedsCapability() const
{
    return QLatin1String("body");
}

#include "sieveconditionbody.moc"