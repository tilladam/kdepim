/*
    knfoldermanager.h

    KNode, the KDE newsreader
    Copyright (c) 1999-2001 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US
*/

#ifndef KNFOLDERMANAGER_H
#define KNFOLDERMANAGER_H

#include <qlist.h>

class KNListView;
class KNCleanUp;
class KNFolder;
class KNArticleManager;
class KNCleanupProgress;


class KNFolderManager
{
  public:
    KNFolderManager(KNListView *v, KNArticleManager *a);
    ~KNFolderManager();

    //folder access
    void setCurrentFolder(KNFolder *f);
    KNFolder* currentFolder()             { return c_urrentFolder; }
    bool hasCurrentFolder()               { return (c_urrentFolder!=0); }
    KNFolder* folder(int id);

    //standard folders
    KNFolder* drafts()                    { return f_List.at(0); }
    KNFolder* outbox()                    { return f_List.at(1); }
    KNFolder* sent()                      { return f_List.at(2); }

    //folder handling
    void newFolder(KNFolder *p);
    void deleteFolder(KNFolder *f);
    void removeFolder(KNFolder *f);
    void emptyFolder(KNFolder *f);
    void showProperties(KNFolder *f);

    //unsent articles
    int unsentForAccount(int accId);

    //compacting
    void compactFolder(KNFolder *f);
    void compactAll(KNCleanUp *cup);

    //synchronization
    void syncFolders();   


  protected:
    int loadCustomFolders();
    void showListItems();
    void createListItem(KNFolder *f);
        
    KNFolder  *c_urrentFolder;
    QList<KNFolder> f_List;
    KNListView *v_iew;
    KNArticleManager *a_rtManager;

};

#endif
