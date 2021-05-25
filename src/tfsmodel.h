#ifndef TFSMODEL_H
#define TFSMODEL_H

#include "qabstractitemmodel.h"

#include <regex>
#include <QThread>

#include "tfs_dataframe.h"
#include "qfilterworker.h"

QVariant df_loc(const tfs::dataframe<double>& df, int row, int column);


class TFSModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //TFSModel();

    TFSModel(tfs::dataframe<double> *dataframe);
    ~TFSModel();

    void filter(const QString& pattern);

signals:
    void request_filter(const QString& pattern, std::vector<size_t>* buf);

public slots:
    void receive_indices(std::vector<size_t>* buf);

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    tfs::dataframe<double> *df;

    bool is_filtering;
    QFilterWorker *filterworker;
    std::vector<size_t> *accepted_rows;
    //std::vector<size_t> *index_buffer;
    QThread workerthread;

};

#endif // TFSMODEL_H
