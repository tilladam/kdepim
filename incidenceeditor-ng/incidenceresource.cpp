/*
  Copyright (C) 2014 Sandro Knauß <knauss@kolabsys.com>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "incidenceresource.h"
#include "resourcemanagement.h"
#include "resourcemodel.h"
#include "attendeecomboboxdelegate.h"
#include "incidencedatetime.h"

#ifdef KDEPIM_MOBILE_UI
#include "ui_dialogmoremobile.h"
#else
#include "ui_dialogdesktop.h"
#endif

#include <KDebug>
#include <KDescendantsProxyModel>
#include <KMessageBox>

#include <KPIMUtils/Email>

#include <QCompleter>
using namespace IncidenceEditorNG;

class SwitchRoleProxy:public QSortFilterProxyModel {

public:
    SwitchRoleProxy(QObject *parent = 0)
        : QSortFilterProxyModel(parent) {

    }

    virtual QVariant data(const QModelIndex& index, int role) const {
        QVariant d;
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            d = QSortFilterProxyModel::data(index, ResourceModel::FullName);
            return d;
        }
        d = QSortFilterProxyModel::data(index, role);
        return d;
    }
};

#ifdef KDEPIM_MOBILE_UI
IncidenceResource::IncidenceResource(IncidenceAttendee* ieAttendee, IncidenceDateTime *dateTime, Ui::EventOrTodoMore *ui)
#else
IncidenceResource::IncidenceResource(IncidenceAttendee* ieAttendee, IncidenceDateTime *dateTime, Ui::EventOrTodoDesktop *ui)
#endif
    : IncidenceEditor(0)
    , mUi(ui)
    , dataModel(ieAttendee->dataModel())
    , mDateTime(dateTime)
    , resourceDialog(new ResourceManagement())
{
    setObjectName("IncidenceResource");
    connect(resourceDialog, SIGNAL(okClicked()),
            SLOT(dialogOkPressed()));

    connect( mDateTime, SIGNAL(startDateChanged(QDate)),
             SLOT(slotDateChanged()) );
    connect( mDateTime, SIGNAL(endDateChanged(QDate)),
             SLOT(slotDateChanged()) );


#ifndef KDEPIM_MOBILE_UI
    QStringList attrs;
    attrs << QLatin1String("cn") <<  QLatin1String("mail");

    completer = new QCompleter(this);
    ResourceModel *model = new ResourceModel(attrs, this);

    KDescendantsProxyModel *proxyModel = new KDescendantsProxyModel( this );
    proxyModel->setSourceModel( model );
    SwitchRoleProxy *proxyModel2 = new SwitchRoleProxy(this);
    proxyModel2->setSourceModel(proxyModel);

    completer->setModel(proxyModel2);
    completer->setCompletionRole(ResourceModel::FullName);
    completer->setWrapAround(false);
    mUi->mNewResource->setCompleter(completer);

    AttendeeLineEditDelegate *attendeeDelegate = new AttendeeLineEditDelegate(this);
    attendeeDelegate->setCompletionMode( KGlobalSettings::self()->completionMode() );

    ResourceFilterProxyModel *filterProxyModel = new ResourceFilterProxyModel(this);
    filterProxyModel->setDynamicSortFilter(true);
    filterProxyModel->setSourceModel(dataModel);


    mUi->mResourcesTable->setModel(filterProxyModel);
    mUi->mResourcesTable->setItemDelegateForColumn(AttendeeTableModel::Role, ieAttendee->roleDelegate());
    mUi->mResourcesTable->setItemDelegateForColumn(AttendeeTableModel::FullName, attendeeDelegate);
    mUi->mResourcesTable->setItemDelegateForColumn(AttendeeTableModel::Status, ieAttendee->stateDelegate());
    mUi->mResourcesTable->setItemDelegateForColumn(AttendeeTableModel::Response, ieAttendee->responseDelegate());

    connect(mUi->mFindResourcesButton, SIGNAL(clicked()), SLOT(findResources()));
    connect(mUi->mBookResourceButton, SIGNAL(clicked()), SLOT(bookResource()));
    connect(filterProxyModel, SIGNAL(layoutChanged()), SLOT(layoutChanged()));
    connect(filterProxyModel, SIGNAL(layoutChanged()), SLOT(updateCount()));
    connect(filterProxyModel, SIGNAL(rowsInserted(const QModelIndex&, int , int)), SLOT(updateCount()));
    connect(filterProxyModel, SIGNAL(rowsRemoved(const QModelIndex&, int , int)), SLOT(updateCount()));
    // only update when FullName is changed
    connect(filterProxyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SLOT(updateCount()));
#endif
}

void IncidenceResource::load(const KCalCore::Incidence::Ptr &incidence)
{
    slotDateChanged();
}

void IncidenceResource::slotDateChanged()
{
    resourceDialog->slotDateChanged(mDateTime->startDate(), mDateTime->endDate());
}


void IncidenceResource::save(const KCalCore::Incidence::Ptr &incidence)
{
  //all logic inside IncidenceAtendee (using same model)
}

bool IncidenceResource::isDirty() const
{
  //all logic inside IncidenceAtendee (using same model)
  return false;
}

void IncidenceResource::bookResource()
{
#ifndef KDEPIM_MOBILE_UI
    QString name, email;

    KPIMUtils::extractEmailAddressAndName(mUi->mNewResource->text(), email, name);
    KCalCore::Attendee::Ptr attendee(new KCalCore::Attendee(name, email));
    attendee->setCuType(KCalCore::Attendee::Resource);
    dataModel->insertAttendee(dataModel->rowCount(), attendee);
#endif
}

void IncidenceResource::findResources()
{
    resourceDialog->show();
}

void IncidenceResource::dialogOkPressed()
{
    ResourceItem::Ptr item = resourceDialog->selectedItem();
    QString name = item->ldapObject().value(QLatin1String("cn"));
    QString email = item->ldapObject().value(QLatin1String("mail"));
#ifndef KDEPIM_MOBILE_UI
    KCalCore::Attendee::Ptr attendee(new KCalCore::Attendee(name, email));
    attendee->setCuType(KCalCore::Attendee::Resource);
    dataModel->insertAttendee(dataModel->rowCount(), attendee);
#endif
}


void IncidenceResource::layoutChanged()
{
#ifndef KDEPIM_MOBILE_UI
    QHeaderView* headerView = mUi->mResourcesTable->horizontalHeader();
    headerView->setSectionHidden(AttendeeTableModel::CuType, true);
    headerView->setSectionHidden(AttendeeTableModel::Name, true);
    headerView->setSectionHidden(AttendeeTableModel::Email, true);
    headerView->setResizeMode(AttendeeTableModel::Role,  QHeaderView::ResizeToContents);
    headerView->setResizeMode(AttendeeTableModel::FullName, QHeaderView::Stretch);
    headerView->setResizeMode(AttendeeTableModel::Available,  QHeaderView::ResizeToContents);
    headerView->setResizeMode(AttendeeTableModel::Status,  QHeaderView::ResizeToContents);
    headerView->setResizeMode(AttendeeTableModel::Response,  QHeaderView::ResizeToContents);
#endif
}


void IncidenceResource::updateCount()
{
    emit resourceCountChanged(resourceCount());
}

int IncidenceResource::resourceCount() const
{
#ifndef KDEPIM_MOBILE_UI
    int c=0;
    QModelIndex index;
    QAbstractItemModel *model = mUi->mResourcesTable->model();
    if (!model ) {
        return 0;
      }
    for(int i=0;i< model->rowCount(QModelIndex());i++) {
        index = model->index(i,AttendeeTableModel::FullName);
        if (!model->data(index).toString().isEmpty()) {
            c++;
          }
      }
    return c;

#endif
    return 0;
}

#include "incidenceresource.moc"
