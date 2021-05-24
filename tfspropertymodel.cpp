#include "tfspropertymodel.h"
#include "tfshelper.h"

TfsPropertyModel::TfsPropertyModel()
{

}

QModelIndex TfsPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex TfsPropertyModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int TfsPropertyModel::rowCount(const QModelIndex &parent) const
{
    return df->property_count();
}

int TfsPropertyModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TfsPropertyModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        auto& p = df->get_property(index.row());
        switch (index.column()) {
        case 0:
            return QString::fromStdString(p.name);
        case 1:
            return QString::fromStdString(tfs::string_fromDT(p.value.type));
        case 2:
            return data_value_to_qvariant(p.value);
        }
    }

    return QVariant();
}

QVariant TfsPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    //switch(orientation) {
    if (orientation == Qt::Orientation::Horizontal) {
        switch(section) {
        case 0: return QString("Name");
        case 1: return QString("type");
        case 2: return QString("Value");
        }
    }
    return QVariant();
}
