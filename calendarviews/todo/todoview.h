/*
  This file is part of KOrganizer.

  Copyright (c) 2000,2001,2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2005 Rafal Rzepecki <divide@users.sourceforge.net>
  Copyright (c) 2008 Thomas Thrainer <tom_t@gmx.at>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/
#ifndef CALENDARVIEWS_TODOVIEW_H
#define CALENDARVIEWS_TODOVIEW_H

#include "eventview.h"
#include <Akonadi/Calendar/ETMCalendar>
#include <Akonadi/Calendar/IncidenceChanger>

#include <KConfig>
#include <QPointer>

class KJob;
class TodoCategoriesDelegate;
class TodoViewQuickAddLine;
class TodoViewQuickSearch;
class TodoViewSortFilterProxyModel;
class TodoViewView;

namespace KPIM {
  class KDatePickerPopup;
}

namespace Akonadi {
  class ETMViewStateSaver;
}

class QItemSelection;
class QMenu;
class QModelIndex;
class QToolButton;
class QTimer;

namespace EventViews {

class EVENTVIEWS_EXPORT TodoView : public EventViews::EventView
{
  Q_OBJECT
  friend class ModelStack;

  public:
    TodoView( const EventViews::PrefsPtr &preferences, bool sidebarView, QWidget *parent );
    ~TodoView();

    virtual void setCalendar( const Akonadi::ETMCalendar::Ptr & );

    virtual Akonadi::Item::List selectedIncidences() const;
    virtual KCalCore::DateList selectedIncidenceDates() const;
    virtual int currentDateCount() const { return 0; }

    void setDocumentId( const QString & ) {}

    void saveLayout( KConfig *config, const QString &group ) const;

    void restoreLayout( KConfig *config, const QString &group, bool minimalDefaults );

    /** documentation in baseview.h */
    void getHighlightMode( bool &highlightEvents,
                           bool &highlightTodos,
                           bool &highlightJournals );

    bool usesFullWindow();

    bool supportsDateRangeSelection() { return false; }

  public Q_SLOTS:
    virtual void setIncidenceChanger( Akonadi::IncidenceChanger *changer );
    virtual void showDates( const QDate &start, const QDate &end,
                            const QDate &preferredMonth = QDate() );
    virtual void showIncidences( const Akonadi::Item::List &incidenceList, const QDate &date );
    virtual void updateView();
    virtual void changeIncidenceDisplay( const Akonadi::Item &incidence, Akonadi::IncidenceChanger::ChangeType changeType );
    virtual void updateConfig();
    virtual void clearSelection();
    void expandIndex( const QModelIndex &index );
    void restoreViewState();
    void saveViewState();

    void createNote();
    void createEvent();

  protected Q_SLOTS:
    void resizeEvent( QResizeEvent * ) /*Q_DECL_OVERRIDE*/;
    void addQuickTodo( Qt::KeyboardModifiers modifier );

    void contextMenu( const QPoint &pos );

    void selectionChanged( const QItemSelection &selected,
                           const QItemSelection &deselected );

    // slots used by popup-menus
    void showTodo();
    void editTodo();
    void deleteTodo();
    void newTodo();
    void newSubTodo();
    void copyTodoToDate( const QDate &date );

  private Q_SLOTS:
    void scheduleResizeColumns();
    void resizeColumns();
    void itemDoubleClicked( const QModelIndex &index );
    void setNewDate( const QDate &date );
    void setNewPercentage( QAction *action );
    void setNewPriority( QAction *action );
    void changedCategories( QAction *action );
    void setFullView( bool fullView );

    void setFlatView( bool flatView, bool notifyOtherViews = true );

    void onRowsInserted( const QModelIndex &parent, int start, int end );
    void onTagsFetched(KJob*);

  Q_SIGNALS:
    void purgeCompletedSignal();
    void unSubTodoSignal();
    void unAllSubTodoSignal();
    void configChanged();
    void fullViewChanged( bool enabled );
    void printPreviewTodo();
    void printTodo();

    void createNote(const Akonadi::Item &item);
    void createEvent(const Akonadi::Item &item);

  private:
    QMenu *createCategoryPopupMenu();
    QString stateSaverGroup() const;

    /** Creates a new todo with the given text as summary under the given parent */
    void addTodo( const QString &summary,
                  const Akonadi::Item &parentItem,
                  const QStringList &categories = QStringList() );

    TodoViewView *mView;
    TodoViewSortFilterProxyModel *mProxyModel;
    TodoCategoriesDelegate *mCategoriesDelegate;

    TodoViewQuickSearch *mQuickSearch;
    TodoViewQuickAddLine *mQuickAdd;
    QToolButton *mFullViewButton;
    QToolButton *mFlatViewButton;

    QMenu *mItemPopupMenu;
    KPIM::KDatePickerPopup *mCopyPopupMenu;
    KPIM::KDatePickerPopup *mMovePopupMenu;
    QMenu *mPriorityPopupMenu;
    QMenu *mPercentageCompletedPopupMenu;
    QList<QAction*> mItemPopupMenuItemOnlyEntries;
    QList<QAction*> mItemPopupMenuReadWriteEntries;

    QAction *mMakeTodoIndependent;
    QAction *mMakeSubtodosIndependent;

    QPointer<Akonadi::ETMViewStateSaver> mTreeStateRestorer;

    QMap<QAction *,int> mPercentage;
    QMap<QAction *,int> mPriority;
    bool mSidebarView;
    bool mResizeColumnsScheduled;
    QTimer *mResizeColumnsTimer;
};


}

#endif
