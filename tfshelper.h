#ifndef TFSHELPER_H
#define TFSHELPER_H

#include <QVariant>
#include "tfs_dataframe.h"

template<typename real>
QVariant data_value_to_qvariant(const tfs::data_value<real>& value) {
    switch (value.type) {
    case tfs::DataType::S:
        return QString::fromStdString(value.get_string());
    case tfs::DataType::LE:
        return value.get_double();
    case tfs::DataType::D:
        return value.get_int();
    default:
        return QVariant();
    }
}

#endif // TFSHELPER_H
