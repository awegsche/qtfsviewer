#include "qfilterworker.h"
#include <QDebug>

void QFilterWorker::filter(const QString& pattern, std::vector<size_t> *buf)
{
    try {
        std::smatch filtermatch;
        std::regex regex(pattern.toStdString());
        std::vector<size_t> string_columns;
        for (size_t c = 0; c < df->column_count(); c++)
        {
        const auto& col = df->get_column(c);
        if (col.get_type() == tfs::DataType::S)
            string_columns.push_back(c);
        }


        for (size_t i = 0; i< df->size(); i++){
        for (const auto& col: string_columns) {
            const auto& data = df->get_column(col).as_string_vector()[i];

            if (std::regex_search(data, filtermatch, regex)) {
            buf->push_back(i);
            break;
            }
        }
        }

        emit done_filtering(buf);
    }
    catch (std::regex_error e) {
        qWarning() << "failed compiling regex: " << QString::fromStdString(e.what());
        // delete buf because it will be thrown away
        delete buf;
        // return nullptr to signal that the filtering failed
        emit done_filtering(nullptr);

    }


}
