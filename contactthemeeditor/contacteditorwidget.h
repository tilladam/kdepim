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

#ifndef CONTACTEDITORWIDGET_H
#define CONTACTEDITORWIDGET_H
#include "grantleethemeeditor/editorwidget.h"

class ContactEditorWidget : public GrantleeThemeEditor::EditorWidget
{
    Q_OBJECT
public:
    explicit ContactEditorWidget(QWidget *parent = 0);
    ~ContactEditorWidget();

public Q_SLOTS:
    void createCompleterList(const QStringList &extraCompletion = QStringList());
};

#endif // CONTACTEDITORWIDGET_H
