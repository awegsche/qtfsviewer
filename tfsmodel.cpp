#include "tfsmodel.h"

//TFSModel::TFSModel()
//    :df(nullptr),
//      is_filtering(false),
//      workerthread(),
//{
//
//}

TFSModel::TFSModel(tfs::dataframe<double> *dataframe)
    : df(dataframe)
    , is_filtering(false)
    , filterworker(new QFilterWorker(dataframe))
    , accepted_rows(nullptr)
    , workerthread()
{

    connect(this, &TFSModel::request_filter, filterworker, &QFilterWorker::filter);
    connect(filterworker, &QFilterWorker::done_filtering, this, &TFSModel::receive_indices);

    filterworker->moveToThread(&workerthread);
    workerthread.start();
}

TFSModel::~TFSModel()
{
    delete filterworker;
    workerthread.quit();
    workerthread.wait();
}

void TFSModel::filter(const QString &pattern)
{
    if (pattern.isEmpty())
    {
        is_filtering = false;
        endResetModel();
        return;
    }

    is_filtering = true;
    auto index_buffer = new std::vector<size_t>;
    index_buffer->reserve(df->size());
    emit request_filter(pattern, index_buffer);
}

void TFSModel::receive_indices(std::vector<size_t>* buf)
{
    // if buf is nullptr, filtering failed and we don't update the model
    if (!buf) return;
    // accepted_rows will be replace, so trash the old one
    if (accepted_rows)
        delete accepted_rows;
    // replace old accepted_row with new one
    accepted_rows = buf;
    // and finally, request redraw
    endResetModel();
}

QModelIndex TFSModel::index(int row, int column, const QModelIndex &parent) const
{
    if (
        row > static_cast<int>(df->size())
        || column > df->column_count()
        || !df
        ) return QModelIndex();
    return createIndex(row, column);
}

QModelIndex TFSModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int TFSModel::rowCount(const QModelIndex &parent) const
{
    if (!df) return 0;
    if (is_filtering) return static_cast<int>(accepted_rows->size());
    return static_cast<int>(df->size());
}

int TFSModel::columnCount(const QModelIndex &parent) const
{
    if (!df) return 0;
    return static_cast<int>(df->column_count());
}

QVariant TFSModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
        {

        return is_filtering ?
                df_loc(*df, (*accepted_rows)[index.row()], index.column()) :
                df_loc(*df, index.row(), index.column());
        }
    default:
        return QVariant();
    }
}

QVariant TFSModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Orientation::Horizontal) {
            if (section >= df->column_count())
                return QVariant();
            auto retval = QString::fromStdString(this->df->get_column(section).get_name());
            return retval;
        }
        else {
            return section;
        }
    }
    }
    return QVariant();
}

QVariant df_loc(const tfs::dataframe<double> &df, int row, int column)
{
    auto& _column = const_cast<tfs::data_vector<double>&>(df.get_column(static_cast<size_t>(column)));
    auto _row = static_cast<size_t>(row);
    switch (_column.get_type()) {
    case tfs::DataType::S:
        return QString::fromStdString(_column.as_string_vector()[_row]);
    case tfs::DataType::LE:
        return _column.as_double_vector()[_row];
    default:
        return QVariant();
    }
}
