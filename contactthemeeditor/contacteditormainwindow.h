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

#ifndef CONTACTEDITORMAINWINDOW_H
#define CONTACTEDITORMAINWINDOW_H

#include <KXmlGuiWindow>

class ContactEditorPage;
class KAction;
class KRecentFilesAction;
class KUrl;

class ContactEditorMainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit ContactEditorMainWindow();
    ~ContactEditorMainWindow();

protected:
    void closeEvent(QCloseEvent *);

private Q_SLOTS:
    void slotNewTheme();
    void slotCloseTheme();
    void slotAddExtraPage();
    void slotOpenTheme();
    void slotUploadTheme();
    void slotQuitApp();
    bool slotSaveTheme();
    void slotInstallTheme();
    void slotInsertFile();
    void slotManageTheme();
    void slotUpdateView();
    void slotConfigure();
    void slotCanInsertFile(bool b);
    void slotThemeSelected(const KUrl &);
    void slotSaveAsTheme();

private:
    enum ActionSaveTheme {
        SaveOnly = 0,
        SaveAndCloseTheme,
        SaveAndCreateNewTheme
    };

    bool loadTheme(const QString &directory);
    void readConfig();
    void updateActions();
    bool saveCurrentProject(ActionSaveTheme act);
    void setupActions();
    void closeThemeEditor();
    ContactEditorPage *mContactEditor;
    KAction *mNewThemeAction;
    KAction *mCloseThemeAction;
    KAction *mAddExtraPage;
    KAction *mCloseAction;
    KAction *mOpenAction;
    KAction *mUploadTheme;
    KAction *mSaveAction;
    KAction *mInstallTheme;
    KAction *mInsertFile;
    KAction *mManageTheme;
    KAction *mUpdateView;
    KAction *mSaveAsAction;
    KRecentFilesAction *mRecentFileAction;
};

#endif // CONTACTEDITORMAINWINDOW_H
