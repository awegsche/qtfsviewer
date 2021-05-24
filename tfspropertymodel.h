#ifndef TFSPROPERTYMODEL_H
#define TFSPROPERTYMODEL_H

#include <qabstractitemmodel.h>
#include "tfs_dataframe.h"

class TfsPropertyModel : public QAbstractItemModel
{
   tfs::dataframe<double> *df;
public:
    TfsPropertyModel();
    TfsPropertyModel(tfs::dataframe<double>* df)
        : df(df) {}

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // TFSPROPERTYMODEL_H
