/* pilotTodoEntry.h			KPilot
**
** Copyright (C) 1998-2001 by Dan Pilone
**
** See the .cc file for an explanation of what this file is for.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
** MA 02139, USA.
*/

/*
** Bug reports and questions can be sent to adridg@cs.kun.nl
*/
#ifndef __KPILOT_TODO_ENTRY_H
#define __KPILOT_TODO_ENTRY_H

#include <qbitarray.h>
#include <time.h>
#include <string.h>

#include <pi-macros.h>
#include <pi-todo.h>

#include "pilotAppCategory.h"
#include "pilotRecord.h"


class PilotTodoEntry : public PilotAppCategory
{
public:
  PilotTodoEntry(void) : PilotAppCategory() 
    { memset(&fTodoInfo, 0, sizeof(struct ToDo)); }
  PilotTodoEntry(PilotRecord* rec);
  ~PilotTodoEntry() { free_ToDo(&fTodoInfo); }
  
  PilotRecord* pack() { return PilotAppCategory::pack(); }
  
  struct tm getDueDate() const { return fTodoInfo.due; }
  void setDueDate(struct tm& d) { fTodoInfo.due = d; }
  
  int getIndefinite() const { return fTodoInfo.indefinite; }
  void setIndefinite(int i) { fTodoInfo.indefinite = i; }

  int getPriority() const { return fTodoInfo.priority; }
  void setPriority(int p) { fTodoInfo.priority = p; }

  int getComplete() const { return fTodoInfo.complete; }
  void setComplete(int c) { fTodoInfo.complete = c; }

  void  setDescription(const char* desc);
  char* getDescription() { return fTodoInfo.description; }

  void  setNote(const char* note);
  char* getNote() { return fTodoInfo.note; }
  
protected:
  void *pack(void *, int *);
  void unpack(const void *, int = 0) { }
  
private:
  struct ToDo fTodoInfo;
};



#endif


// $Log:$
