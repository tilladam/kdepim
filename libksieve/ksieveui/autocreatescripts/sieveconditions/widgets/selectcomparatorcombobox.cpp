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

#include "selectcomparatorcombobox.h"
#include "autocreatescripts/autocreatescriptdialog.h"

SelectComparatorComboBox::SelectComparatorComboBox(QWidget *parent)
    : KComboBox(parent)
{
    initialize();
}

SelectComparatorComboBox::~SelectComparatorComboBox()
{
}

void SelectComparatorComboBox::initialize()
{
    Q_FOREACH (const QString &capability, KSieveUi::AutoCreateScriptDialog::sieveCapabilities()) {
        if (capability.startsWith(QLatin1String("comparator-"))) {
            QString str(capability);
            str.remove(QLatin1String("comparator-"));
            addItem(str, capability);
        }
    }
}

QString SelectComparatorComboBox::require() const
{
    return itemData(currentIndex()).toString();
}

QString SelectComparatorComboBox::code() const
{
    return QString::fromLatin1(":comparator \"%1\"").arg(itemText(currentIndex()));
}

#include "selectcomparatorcombobox.moc"
