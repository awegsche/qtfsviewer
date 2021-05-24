#include "tfsdatafiltermodel.h"

TfsDataFilterModel::TfsDataFilterModel()
    :isFiltering(false)
{

}

void TfsDataFilterModel::setFilterPattern(const QString& pattern)
{
    if (pattern.isEmpty())
        isFiltering = false;
    else {
        filterPattern.setPattern(pattern);
        isFiltering = true;
    }
    invalidate();
}

bool TfsDataFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!isFiltering) return true;
    for ( int i = 0; i < sourceModel()->columnCount(source_parent); i++) {
        auto index = sourceModel()->index(source_row, i, source_parent);
        auto match = filterPattern.match(sourceModel()->data(index).toString());
        if (match.hasMatch())
            return true;
    }
    return false;
}
