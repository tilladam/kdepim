/***************************************************************************
                          FilterOE4.cxx  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000 by Hans Dijkema
    email                : kmailcvt@hum.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "filter_oe4.hxx"
#include "oe4_2mbox.h"

#include <kfiledialog.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <klocale.h>

FilterOE4::FilterOE4() : Filter(i18n("Import Folders From Outlook Express 4"),
      "Stephan B. Nedregard/Hans Dijkema",
      i18n("<p>Select the Outlook Express directory on your system. "
      "This import filter will search for folders (the '.mbx' files).</p>"
      "<p><b>NOTE:</b> You will not be able to revert to your original folder "
      "structure, only the folders themselves are imported. But you will "
      "probably only do this one time.</p>"
      "<p><b>NOTE:</b> The folder names will be the same as the Outlook Express "
      "folder names, but they will be preceded with 'OE4-'. If this causes "
      "problems for you (you have KMail folders beginning with 'OE4-'), "
      "cancel this import function (the next dialog will allow you to do "
      "that) and rename the existing KMail folders.</p>"))
{
}

FilterOE4::~FilterOE4()
{
}

void FilterOE4::import(FilterInfo *info)
{
  QString  msg;
  QString  choosen;
  QWidget *parent=info->parent();

  if (!kmailStart(info)) { return; }

   choosen=KFileDialog::getExistingDirectory(QDir::homeDirPath(),parent, 
      i18n("Select Folder"));

   if (choosen.isEmpty()) { return; } // No directory choosen here!

   msg=i18n("Searching for Outlook Express 4 '.mbx' folders in directory %1").arg(choosen);
   info->log(msg);

   {
     DIR *d;
     struct dirent *entry;
     d=opendir(QFile::encodeName(choosen));
     if (d==NULL) 
     {
       QString msg;
       msg=i18n("Can't open directory %1").arg(choosen);
       info->alert(name(),msg);
     }
     else 
     {
       int   N=0,n=0;
       float perc;

       entry=readdir(d);
       while (entry!=NULL) 
       {
         char *file=entry->d_name;
         if (strlen(file)>4 && strcasecmp(&file[strlen(file)-4],".mbx")==0) 
         {
           N+=1;
         }
         entry=readdir(d);
       }
       if (N==0) 
       {
         info->alert(name(),i18n("No '.mbx' folders found!"));
       }
       rewinddir(d);

       info->overall();

       entry=readdir(d);
       while (entry!=NULL) {char *file=entry->d_name;

         n+=1;
         perc=(((float) n)/((float) N))*100.0;
         info->overall(perc);

         if (strlen(file)>4 && strcasecmp(&file[strlen(file)-4],".mbx")==0) {
           {
             char fldr[PATH_MAX],name[256];

             snprintf(fldr, sizeof(fldr), "%s/%s",QFile::encodeName(choosen).data(),file);
             snprintf(name, sizeof(name), "%s",file);name[strlen(name)-4]='\0';

             {
               QString s;
               s.sprintf("\t%s",fldr);
               s=i18n("Source:")+s;
               info->from(s);
               s.sprintf("\tOE4-%s",name);
               s=i18n("Destination:")+s;
               info->to(s);
             }

             {
               msg=i18n("  importing folder %1 to KMail %2").arg(file).arg(name);
               info->log(msg);
             }

             {
               OE42MBox m(fldr,name,this,info);
               m.convert();
             }
           }
         }
         entry=readdir(d);
       }
       closedir(d);

       if (N!=0) 
       {
         info->log(i18n("done."));
         info->current();
         info->current(100.0);
         info->overall();
         info->overall(100.0);

         info->alert(name() ,i18n("All '.mbx' folders are imported"));
       }
     }
   }

   kmailStop(info);
}

// vim: ts=2 sw=2 et
