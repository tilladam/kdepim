/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include "kdganttconstraint.h"
#include "kdganttconstraint_p.h"

#include <QDateTime>

using namespace KDGantt;

/*!\class KDGantt::Constraint
 *\ingroup KDGantt
 * \brief A class used to represent a dependency.
 *
 * Instances of this class represent a dependency between the
 * data items pointed to by a start-QModelIndex and an
 * end-QModelIndex.
 */

/*!\enum KDGantt::Constraint::Type
 * This enum is unused for now.
 */

/*!\enum KDGantt::Constraint::ConstraintDataRole
 * Data roles used when specifying the pen to draw constraints with.
 * \sa setData
 */

Constraint::Private::Private()
    : type( TypeSoft )
{
}

Constraint::Private::Private( const Private& other )
    : QSharedData( other )
{
    start=other.start;
    end=other.end;
    type=other.type;
}

/*! Constructor. Creates a dependency for \a idx2 on \a idx1.
 * \param type controls if the constraint is a soft one that
 * is allowed to be broken (ie, go backwards in time) or a hard
 * constraint that will not allow the user to move an item so
 * that the constraint would have to go backwards. The default is
 * TypeSoft.
 *
 * Actually enforcing hard constraints is the responsibility of
 * the AbstractGrid subclass used in the view.
 */
Constraint::Constraint( const QModelIndex& idx1,  const QModelIndex& idx2, Type type )
    : d( new Private )
{
    d->start=idx1;
    d->end=idx2;
    d->type=type;
    Q_ASSERT_X( idx1 != idx2 || !idx1.isValid(), "Constraint::Constraint", "cannot create a constraint with idx1 == idx2" );
}

/*! Copy-Constructor. */
Constraint::Constraint( const Constraint& other )
    : d( other.d )
{
}

/*! Destructor */
Constraint::~Constraint()
{
}

/*! Assignment operator. */
Constraint& Constraint::operator=( const Constraint& other )
{
    d = other.d;
    return *this;
}

/*! This is unused for now. */
Constraint::Type Constraint::type() const
{
    return d->type;
}

/*! \returns The dependency index */
QModelIndex Constraint::startIndex() const
{
    return d->start;
}

/*! \returns The constrained index */
QModelIndex Constraint::endIndex() const
{
    return d->end;
}

/*! \returns The data associated with this index for the specified role.
 * \param role The role to fetch the data for.
 * \sa ConstraintDataRole
 */
QVariant Constraint::data( int role ) const
{
    return d->data.value( role );
}

/*! Set data on this index for the specified role.
 * \param role The role to set the data for.
 * \param value The data to set on the index.
 * \sa ConstraintDataRole
 */
void Constraint::setData( int role, const QVariant& value )
{
    d->data.insert( role, value );
}

/*! Compare two Constraint objects. Two Constraints are equal
 * if the have the same start and end indexes
 */
bool Constraint::operator==( const Constraint& other ) const
{
    if ( d == other.d ) return true;
    return ( *d ).equals( *( other.d ) );
}

/*!\internal*/
uint Constraint::hash() const
{
    return ::qHash( d->start ) ^ ::qHash( d->end ) ^ ::qHash( static_cast<uint>( d->type ) );
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug dbg, const Constraint& c )
{
    return c.debug( dbg );
}

QDebug Constraint::debug( QDebug dbg ) const
{
    dbg << "KDGantt::Constraint[ start="<<d->start<<" end="<<d->end<<"]";
    return dbg;
}

#endif /* QT_NO_DEBUG_STREAM */

#ifndef KDAB_NO_UNIT_TESTS

#include <QStandardItemModel>

#include "unittest/test.h"

KDAB_SCOPED_UNITTEST_SIMPLE( KDGantt, Constraint, "test" )
{
    QStandardItemModel dummyModel( 100, 100 );
    QModelIndex idx1 = dummyModel.index( 7, 17, QModelIndex() );
    QModelIndex idx2 = dummyModel.index( 42, 17, QModelIndex() );

    Constraint c1 = Constraint( QModelIndex(), QModelIndex(), Constraint::TypeSoft );
    Constraint c2 = Constraint( QModelIndex(), QModelIndex(), Constraint::TypeSoft );
    Constraint c3 = c2;
    Constraint c4( idx1, idx2 );
    Constraint c5( idx2, idx1 );

    assertTrue( c1==c2 );
    assertEqual( qHash( c1 ), qHash( c2 ) );
    assertTrue( c1==c3 );
    assertEqual( qHash( c1 ), qHash( c3 ) );
    assertTrue( c2==c3 );
    assertEqual( qHash( c2 ), qHash( c3 ) );

    assertFalse( c2==c4 );
    assertNotEqual( qHash( c2 ), qHash( c4 ) );

    assertFalse( c4==c5 );

    assertEqual( c3.type(), Constraint::TypeSoft );

    dummyModel.removeRow( 8 );
    assertFalse( c4==c5 );
}

#endif /* KDAB_NO_UNIT_TESTS */
