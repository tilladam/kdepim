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

#ifndef SELECTMIMECOMBOBOX_H
#define SELECTMIMECOMBOBOX_H
#include <KComboBox>

namespace KSieveUi {
class SelectMimeComboBox : public KComboBox
{
    Q_OBJECT
public:
    explicit SelectMimeComboBox(QWidget *parent = 0);
    ~SelectMimeComboBox();

    QString code() const;
    void setCode(const QString &code, const QString &name, QString &error);

    QString require() const;

Q_SIGNALS:
    void valueChanged();

private:
    void initialize();
};
}

#endif // SELECTMIMECOMBOBOX_H
