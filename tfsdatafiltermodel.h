#ifndef TFSDATAFILTERMODEL_H
#define TFSDATAFILTERMODEL_H

#include <QSortFilterProxyModel>

class TfsDataFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TfsDataFilterModel();

    void setFilterPattern(const QString &pattern);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QRegularExpression filterPattern;
    bool isFiltering;
};

#endif // TFSDATAFILTERMODEL_H
