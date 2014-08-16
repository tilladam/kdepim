/*
  Copyright (C) 2014 Christian Mollekopf <mollekopf@kolabsys.com>

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
#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <KDebug>
#include "reparentingmodel.h"

class DummyNode : public ReparentingModel::Node
{
public:
    DummyNode(ReparentingModel &personModel, const QString &name)
    : ReparentingModel::Node(personModel),
    mName(name)
    {}

    virtual ~DummyNode(){};

    virtual bool operator==(const Node &node) const {
        const DummyNode *dummyNode = dynamic_cast<const DummyNode*>(&node);
        if (dummyNode) {
            return (dummyNode->mName == mName);
        }
        return false;
    }

private:
    virtual QVariant data(int role) const {
        if (role == Qt::DisplayRole) {
            return mName;
        }
        return QVariant();
    }
    virtual bool setData(const QVariant& variant, int role){
        return false;
    }
    virtual bool isDuplicateOf(const QModelIndex& sourceIndex) {
        return (sourceIndex.data().toString() == mName);
    }

    virtual bool adopts(const QModelIndex& sourceIndex) {
        return sourceIndex.data().toString().contains(QLatin1String("orphan"));
    }

    QString mName;
};

class ModelSignalSpy : public QObject {
    Q_OBJECT
public:
    explicit ModelSignalSpy(QAbstractItemModel &model) {
        connect(&model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
        connect(&model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
        connect(&model, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)), this, SLOT(onRowsMoved(QModelIndex,int,int, QModelIndex, int)));
        connect(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
        connect(&model, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
        connect(&model, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    }

    QStringList mSignals;
    QModelIndex parent;
    int start;
    int end;

public Q_SLOTS:
    void onRowsInserted(QModelIndex p, int s, int e) {
        mSignals << QLatin1String("rowsInserted");
        parent = p;
        start = s;
        end = e;
    }
    void onRowsRemoved(QModelIndex p, int s, int e) {
        mSignals << QLatin1String("rowsRemoved");
        parent = p;
        start = s;
        end = e;
    }
    void onRowsMoved(QModelIndex,int,int,QModelIndex,int) {
        mSignals << QLatin1String("rowsMoved");
    }
    void onDataChanged(QModelIndex,QModelIndex) {
        mSignals << QLatin1String("dataChanged");
    }
    void onLayoutChanged() {
        mSignals << QLatin1String("layoutChanged");
    }
    void onModelReset() {
        mSignals << QLatin1String("modelReset");
    }
};

QModelIndex getIndex(char *string, const QAbstractItemModel &model)
{
    QModelIndexList list = model.match(model.index(0, 0), Qt::DisplayRole, QString::fromLatin1(string), 1, Qt::MatchRecursive);
    if (list.isEmpty()) {
        return QModelIndex();
    }
    return list.first();
}

QModelIndexList getIndexList(char *string, const QAbstractItemModel &model)
{
    return model.match(model.index(0, 0), Qt::DisplayRole, QString::fromLatin1(string), 1, Qt::MatchRecursive);
}

class ReparentingModelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testPopulation();
    void testAddRemoveSourceItem();
    void testInsertSourceRow();
    void testInsertSourceRowSubnode();
    void testAddRemoveProxyNode();
    void testDeduplicate();
    void testDeduplicateNested();
    void testDeduplicateProxyNodeFirst();
    void testNestedDeduplicateProxyNodeFirst();
    void testReparent();
    void testReparentResetWithoutCrash();
    void testAddReparentedSourceItem();
    void testRemoveReparentedSourceItem();
    void testNestedReparentedSourceItem();
    void testAddNestedReparentedSourceItem();
    void testSourceDataChanged();
    void testSourceLayoutChanged();
    void testInvalidLayoutChanged();
};

void ReparentingModelTest::testPopulation()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));
    sourceModel.appendRow(new QStandardItem(QLatin1String("row2")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("row2", reparentingModel).isValid());
}

void ReparentingModelTest::testAddRemoveSourceItem()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);
    ModelSignalSpy spy(reparentingModel);

    sourceModel.appendRow(new QStandardItem(QLatin1String("row2")));
    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("row2", reparentingModel).isValid());
    QCOMPARE(spy.parent, QModelIndex());
    QCOMPARE(spy.start, 1);
    QCOMPARE(spy.end, 1);

    sourceModel.removeRows(1, 1, QModelIndex());
    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(!getIndex("row2", reparentingModel).isValid());
    QCOMPARE(spy.parent, QModelIndex());
    QCOMPARE(spy.start, 1);
    QCOMPARE(spy.end, 1);

    QCOMPARE(spy.mSignals, QStringList() << QLatin1String("rowsInserted") << QLatin1String("rowsRemoved"));
}

//Ensure the model can deal with rows that are inserted out of order
void ReparentingModelTest::testInsertSourceRow()
{
    QStandardItemModel sourceModel;
    QStandardItem *row2 = new QStandardItem(QLatin1String("row2"));
    sourceModel.appendRow(row2);

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);
    ModelSignalSpy spy(reparentingModel);

    QStandardItem *row1 = new QStandardItem(QLatin1String("row1"));
    sourceModel.insertRow(0, row1);
    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("row2", reparentingModel).isValid());

    //The model does not try to reorder. First come, first serve.
    QCOMPARE(getIndex("row1", reparentingModel).row(), 1);
    QCOMPARE(getIndex("row2", reparentingModel).row(), 0);
    reparentingModel.setData(reparentingModel.index(1, 0, QModelIndex()), QLatin1String("row1foo"), Qt::DisplayRole);
    reparentingModel.setData(reparentingModel.index(0, 0, QModelIndex()), QLatin1String("row2foo"), Qt::DisplayRole);
    QCOMPARE(row1->data(Qt::DisplayRole).toString(), QLatin1String("row1foo"));
    QCOMPARE(row2->data(Qt::DisplayRole).toString(), QLatin1String("row2foo"));
}

//Ensure the model can deal with rows that are inserted out of order in a subnode
void ReparentingModelTest::testInsertSourceRowSubnode()
{
    QStandardItem *parent = new QStandardItem(QLatin1String("parent"));
    
    QStandardItemModel sourceModel;
    sourceModel.appendRow(parent);
    QStandardItem *row2 = new QStandardItem(QLatin1String("row2"));
    parent->appendRow(row2);

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);
    ModelSignalSpy spy(reparentingModel);

    QStandardItem *row1 = new QStandardItem(QLatin1String("row1"));
    parent->insertRow(0, row1);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("row2", reparentingModel).isValid());
    //The model does not try to reorder. First come, first serve.
    QCOMPARE(getIndex("row1", reparentingModel).row(), 1);
    QCOMPARE(getIndex("row2", reparentingModel).row(), 0);
    reparentingModel.setData(reparentingModel.index(1, 0, getIndex("parent", reparentingModel)), QLatin1String("row1foo"), Qt::DisplayRole);
    reparentingModel.setData(reparentingModel.index(0, 0, getIndex("parent", reparentingModel)), QLatin1String("row2foo"), Qt::DisplayRole);
    QCOMPARE(row1->data(Qt::DisplayRole).toString(), QLatin1String("row1foo"));
    QCOMPARE(row2->data(Qt::DisplayRole).toString(), QLatin1String("row2foo"));
}

void ReparentingModelTest::testAddRemoveProxyNode()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    ModelSignalSpy spy(reparentingModel);

    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("proxy1", reparentingModel).isValid());

    reparentingModel.removeNode(DummyNode(reparentingModel, QLatin1String("proxy1")));

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    QVERIFY(!getIndex("proxy1", reparentingModel).isValid());

    QCOMPARE(spy.mSignals, QStringList() << QLatin1String("modelReset") << QLatin1String("modelReset"));
}

void ReparentingModelTest::testDeduplicate()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("row1"))));

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QCOMPARE(getIndexList("row1", reparentingModel).size(), 1);
    //TODO ensure we actually have the source index and not the proxy index
}

/**
 * rebuildAll detects and handles nested duplicates
 */
void ReparentingModelTest::testDeduplicateNested()
{
    QStandardItemModel sourceModel;
    QStandardItem *item = new QStandardItem(QLatin1String("row1"));
    item->appendRow(new QStandardItem(QLatin1String("child1")));
    sourceModel.appendRow(item);

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("child1"))));

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QCOMPARE(getIndexList("child1", reparentingModel).size(), 1);
}

/**
 * onSourceRowsInserted detects and removes duplicates
 */
void ReparentingModelTest::testDeduplicateProxyNodeFirst()
{
    QStandardItemModel sourceModel;
    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("row1"))));

    QTest::qWait(0);

    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QCOMPARE(getIndexList("row1", reparentingModel).size(), 1);
    //TODO ensure we actually have the source index and not the proxy index
}

/**
 * onSourceRowsInserted detects and removes nested duplicates
 */
void ReparentingModelTest::testNestedDeduplicateProxyNodeFirst()
{
    QStandardItemModel sourceModel;
    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("child1"))));

    QTest::qWait(0);

    QStandardItem *item = new QStandardItem(QLatin1String("row1"));
    item->appendRow(new QStandardItem(QLatin1String("child1")));
    sourceModel.appendRow(item);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QCOMPARE(getIndexList("child1", reparentingModel).size(), 1);
    //TODO ensure we actually have the source index and not the proxy index
}

void ReparentingModelTest::testReparent()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("orphan")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("proxy1", reparentingModel).isValid());
    QCOMPARE(reparentingModel.rowCount(getIndex("proxy1", reparentingModel)), 1);
}

/*
 * This test ensures we properly deal with reparented source nodes if the model is reset.
 * This is important since source nodes are removed during the model reset while the proxy nodes (to which the source nodes have been reparented) remain.
 *
 * Note that this test is only useful with the model internal asserts.
 */
void ReparentingModelTest::testReparentResetWithoutCrash()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("orphan")));

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));
    QTest::qWait(0);

    reparentingModel.setSourceModel(&sourceModel);

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
}

void ReparentingModelTest::testAddReparentedSourceItem()
{
    QStandardItemModel sourceModel;

    ReparentingModel reparentingModel;
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));
    reparentingModel.setSourceModel(&sourceModel);

    QTest::qWait(0);

    ModelSignalSpy spy(reparentingModel);

    sourceModel.appendRow(new QStandardItem(QLatin1String("orphan")));

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("proxy1", reparentingModel).isValid());
    QCOMPARE(spy.mSignals, QStringList() << QLatin1String("rowsInserted"));
    QCOMPARE(spy.parent, getIndex("proxy1", reparentingModel));
    QCOMPARE(spy.start, 0);
    QCOMPARE(spy.end, 0);
}

void ReparentingModelTest::testRemoveReparentedSourceItem()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("orphan")));
    ReparentingModel reparentingModel;
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));
    reparentingModel.setSourceModel(&sourceModel);

    QTest::qWait(0);

    ModelSignalSpy spy(reparentingModel);

    sourceModel.removeRows(0, 1, QModelIndex());

    QTest::qWait(0);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 1);
    QVERIFY(getIndex("proxy1", reparentingModel).isValid());
    QVERIFY(!getIndex("orphan", reparentingModel).isValid());
    QCOMPARE(spy.mSignals, QStringList() << QLatin1String("rowsRemoved"));
    QCOMPARE(spy.parent, getIndex("proxy1", reparentingModel));
    QCOMPARE(spy.start, 0);
    QCOMPARE(spy.end, 0);
}

void ReparentingModelTest::testNestedReparentedSourceItem()
{
    QStandardItemModel sourceModel;
    QStandardItem *item = new QStandardItem(QLatin1String("parent"));
    item->appendRow(QList<QStandardItem*>() << new QStandardItem(QLatin1String("orphan")));
    sourceModel.appendRow(item);

    ReparentingModel reparentingModel;
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));
    reparentingModel.setSourceModel(&sourceModel);

    QTest::qWait(0);

    //toplevel should have both parent and proxy
    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("orphan", reparentingModel).isValid());
    QCOMPARE(getIndex("orphan", reparentingModel).parent(), getIndex("proxy1", reparentingModel));
}

void ReparentingModelTest::testAddNestedReparentedSourceItem()
{
    QStandardItemModel sourceModel;

    ReparentingModel reparentingModel;
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("proxy1"))));
    reparentingModel.setSourceModel(&sourceModel);

    QTest::qWait(0);

    ModelSignalSpy spy(reparentingModel);

    QStandardItem *item = new QStandardItem(QLatin1String("parent"));
    item->appendRow(QList<QStandardItem*>() << new QStandardItem(QLatin1String("orphan")));
    sourceModel.appendRow(item);

    QTest::qWait(0);

    //toplevel should have both parent and proxy
    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("orphan", reparentingModel).isValid());
    QCOMPARE(getIndex("orphan", reparentingModel).parent(), getIndex("proxy1", reparentingModel));
    QCOMPARE(spy.mSignals, QStringList() << QLatin1String("rowsInserted") << QLatin1String("rowsInserted"));
}

void ReparentingModelTest::testSourceDataChanged()
{
    QStandardItemModel sourceModel;
    QStandardItem *item = new QStandardItem(QLatin1String("row1"));
    sourceModel.appendRow(item);

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&sourceModel);

    item->setText(QLatin1String("rowX"));

    QVERIFY(!getIndex("row1", reparentingModel).isValid());
    QVERIFY(getIndex("rowX", reparentingModel).isValid());
}


void ReparentingModelTest::testSourceLayoutChanged()
{
    QStandardItemModel sourceModel;
    sourceModel.appendRow(new QStandardItem(QLatin1String("row2")));
    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));

    QSortFilterProxyModel filter;
    filter.setSourceModel(&sourceModel);

    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&filter);
    ModelSignalSpy spy(reparentingModel);

    QPersistentModelIndex index1 = reparentingModel.index(0, 0, QModelIndex());
    QPersistentModelIndex index2 = reparentingModel.index(1, 0, QModelIndex());

    //Emits layout changed and sorts the items the other way around
    filter.sort(0, Qt::AscendingOrder);

    QCOMPARE(reparentingModel.rowCount(QModelIndex()), 2);
    QVERIFY(getIndex("row1", reparentingModel).isValid());
    //Right now we don't even care about the order
    // QCOMPARE(spy.mSignals, QStringList() << QLatin1String("layoutChanged"));
    QCOMPARE(index1.data().toString(), QLatin1String("row2"));
    QCOMPARE(index2.data().toString(), QLatin1String("row1"));
}

/*
 * This is a very implementation specific test that tries to crash the model
 */
//Test for invalid implementation of layoutChanged
//*have proxy node in model
//*insert duplicate from source
//*issue layout changed so the model get's rebuilt
//*access node (which is not actually existing anymore)
// => crash
void ReparentingModelTest::testInvalidLayoutChanged()
{
    QStandardItemModel sourceModel;
    QSortFilterProxyModel filter;
    filter.setSourceModel(&sourceModel);
    ReparentingModel reparentingModel;
    reparentingModel.setSourceModel(&filter);
    reparentingModel.addNode(ReparentingModel::Node::Ptr(new DummyNode(reparentingModel, QLatin1String("row1"))));

    QTest::qWait(0);

    //Take reference to proxy node
    QPersistentModelIndex persistentIndex = getIndexList("row1", reparentingModel).first();
    QVERIFY(persistentIndex.isValid());

    sourceModel.appendRow(new QStandardItem(QLatin1String("row1")));
    sourceModel.appendRow(new QStandardItem(QLatin1String("row2")));

    //This rebuilds the model and invalidates the reference
    //Emits layout changed and sorts the items the other way around
    filter.sort(0, Qt::AscendingOrder);

    //This fails because the persistenIndex is no longer valid
    persistentIndex.data().toString();
    QVERIFY(!persistentIndex.isValid());
}


QTEST_MAIN(ReparentingModelTest)

#include "reparentingmodeltest.moc"