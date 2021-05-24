#ifndef QFILTERWORKER_H
#define QFILTERWORKER_H

#include <QObject>
#include <regex>
#include "tfs_dataframe.h"


// for multithreaded filtering and searhing
class QFilterWorker : public QObject {

    Q_OBJECT
public:
    QFilterWorker(tfs::dataframe<double>* df)
        : df(df), keep_running(true) {}

public slots:
    void filter(const QString& pattern, std::vector<size_t>* buf);

signals:
    void done_filtering(std::vector<size_t>* buf);

private:
    bool keep_running;
    tfs::dataframe<double> *df;
    //std::vector<size_t>* indices;
};

#endif // QFILTERWORKER_H
