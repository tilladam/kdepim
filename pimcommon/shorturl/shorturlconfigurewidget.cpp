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

#include "shorturlconfigurewidget.h"
#include "shorturlutils.h"

#include <KLocalizedString>

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

using namespace PimCommon;
ShortUrlConfigureWidget::ShortUrlConfigureWidget(QWidget *parent)
    : QWidget(parent),
      mChanged(false)
{
    QHBoxLayout *lay = new QHBoxLayout;
    lay->setMargin(0);

    QLabel *lab = new QLabel(i18n("Select Short URL server:"));
    lay->addWidget(lab);

    mShortUrlServer = new QComboBox;
    connect(mShortUrlServer, SIGNAL(activated(int)), this, SLOT(slotChanged()));
    lay->addWidget(mShortUrlServer);
    setLayout(lay);
    init();
    loadConfig();
}

ShortUrlConfigureWidget::~ShortUrlConfigureWidget()
{

}

void ShortUrlConfigureWidget::slotChanged()
{
    mChanged = true;
}

void ShortUrlConfigureWidget::init()
{
    for (int i=0; i <PimCommon::ShortUrlUtils::EndListEngine; ++i) {
        mShortUrlServer->addItem(PimCommon::ShortUrlUtils::stringFromEngineType(static_cast<PimCommon::ShortUrlUtils::EngineType>(i)), i);
    }
}

void ShortUrlConfigureWidget::loadConfig()
{
    const int engineType = PimCommon::ShortUrlUtils::readEngineSettings();
    mShortUrlServer->setCurrentIndex(mShortUrlServer->findData(engineType));
    mChanged = false;
}

void ShortUrlConfigureWidget::writeConfig()
{
    if (mChanged) {
        PimCommon::ShortUrlUtils::writeEngineSettings(mShortUrlServer->itemData(mShortUrlServer->currentIndex()).toInt());
        Q_EMIT settingsChanged();
    }
    mChanged = false;
}

void ShortUrlConfigureWidget::resetToDefault()
{
    mShortUrlServer->setCurrentIndex(0);
    mChanged = false;
}

