/***************************************************************************
                          OE42MBox.cpp  -  description
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

#include <klocale.h>
#include <ktempfile.h>

#include <qfile.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "filters.hxx"
#include "oe4_2mbox.h"
#include "liboe.h"

#define CAP cap

QString       OE42MBox::cap = i18n("Import Outlook Express 4");
Filter *      OE42MBox::F = 0;
const char *  OE42MBox::FOLDER = 0;
FilterInfo *  OE42MBox::INFO = 0;
int           OE42MBox::numOfMessages = 0;
unsigned long OE42MBox::added = 0;
unsigned long OE42MBox::mails = 0;

OE42MBox::OE42MBox(const char *in,const char *out,Filter *F,FilterInfo *I)
{
  info=I;
  f=F;
  folderIn=in;
  folderTo=out;
}

OE42MBox::~OE42MBox()
{
}

int OE42MBox::convert(void)
{
oe_data *result;
char s[1024];
  F=f;

  snprintf(s,sizeof(s),"OE4-%s",folderTo);
  FOLDER=s;
  INFO=info;

  added=mails=0;

  numOfMessages=0;
  result=oe_readbox((char *) folderIn,addMessage);

  if (!result->success) {
    switch (result->errcode) {
      case OE_CANNOTREAD :
        {QString msg;
           msg=i18n("Cannot read mailbox %1").arg(folderIn);
           info->alert(CAP,msg);
        }
      break;
      case OE_NOTOEBOX :
        {QString msg;
           msg=i18n("%1 is not an Outlook Express 4 mailbox").arg(folderIn);
           info->alert(CAP,msg);
        }
      break;
      default:
        {QString msg;
           msg=i18n("Unrecoverable error while reading %1").arg(folderIn);
           info->alert(CAP,msg);
        }
      break;
    }
  }
  else {QString msg;
    msg=i18n("%n mail read, %1 were new KMail folders", "%n mails read, %1 were new KMail folders", mails).arg(added);
    info->log(msg);
  }

return result->success;
}

void OE42MBox::addMessage(const char *string,int code)
{
static KTempFile *tmp=0;
static int status=-1;
static float perc=0.0;

  switch(code) {
    case 1: // begin new message
    {
      if (status!=0 && status!=-1) { addMessage("",0); }
      status=1;
      tmp = new KTempFile;
      tmp->setAutoDelete(true);
      if (tmp->status() != 0 || !tmp->fstream()) 
      {   
          QString msg;
          msg=i18n("FATAL: Cannot open temporary file.");
          INFO->alert(CAP,msg);
          status=-1;
          delete tmp;
          tmp = 0;
      }
      if (status!=-1) {
        fprintf(tmp->fstream(),"%s",string);
      }
    }
    break;
    case 2: // continue message
    {
      if (status!=1 && status!=2 && status!=-1) {QString msg;
        msg=i18n("FATAL: Cannot add to ended message,\n"
                         "skipping until message start");
        INFO->alert(CAP,msg);
        status=-1;
      }
      else if (status!=-1) {
        status=2;
        fprintf(tmp->fstream(),"%s",string);
      }
    }
    break;
    case 0: //end of message
    {
      if (status!=1 && status!=2 && status!=-1) {QString msg;
        msg=i18n("FATAL: Cannot end an ended message,\n"
                 "skipping until message start");
        INFO->alert(CAP,msg);
        status=-1;
      }
      else if (status!=-1) {
        status=0;

        perc+=2.0;
        if (perc>100.0) {
          perc=0.0;
          INFO->current();
        }
        INFO->current(perc);

        fprintf(tmp->fstream(),"%s",string);
        if (!tmp->close())
        { 
          QString msg;
          msg=i18n("FATAL: Cannot write temporary file.");
          INFO->alert(CAP,msg);
          status=-1;
        }
        else {
          F->kmailMessage(INFO, (char *) FOLDER, QFile::encodeName(tmp->name()).data());
          added++;
          mails += 1;
          status=-1; // skip to next message.
        }
        delete tmp;
        tmp = 0;
      }
    }
    break;
  }
}

#undef CAP

