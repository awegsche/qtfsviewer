#include "qfilterworker.h"

void QFilterWorker::filter(const QString& pattern, std::vector<size_t> *buf)
{
    std::smatch filtermatch;
    std::regex regex(pattern.toStdString());

    for (size_t i = 0; i< df->size(); i++){
        for (size_t c = 0; c < df->column_count(); c++)
        {
            const auto& col = df->get_column(c);
            if (col.get_type() == tfs::DataType::S) {
                const auto& data = col.as_string_vector()[i];

                if (std::regex_search(data, filtermatch, regex)) {
                    buf->push_back(i);
                    break;
                }
            }
        }
    }

    emit done_filtering(buf);
}
