#ifndef VIEWER_H
#define VIEWER_H

#include <QMainWindow>
#include <QStringListModel>
#include "tfs_dataframe.h"
#include "tfsmodel.h"
#include "tfsdatafiltermodel.h"
#include "tfspropertymodel.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Viewer; }
QT_END_NAMESPACE

class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    ~Viewer();

    void open_tfs(const QString& filename);

private slots:
    void on_actionOpen_triggered();

    void on_actionplotColumn_triggered();

    void on_closePlotButton_clicked();

    void on_actionscatter_plot_column_triggered();

    void on_actionSave_triggered();

    void on_actionSave_Compressed_triggered();


    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Viewer *ui;
    tfs::dataframe<double> *df;
    TFSModel *model;
    TfsPropertyModel *prop_model;

    QVector<QPen> plot_colors;

    void set_chart(const std::string& name,
                   const std::vector<double>& points);
    void set_chart(const std::string& name,
                   const std::vector<double>& x,
                   const QVector<QVector<double>>& y);
    /**
     * @brief Plots the given data lines.
     * Please don't destroy the origin while plotting is performed!
     * @param x
     * @param y
     */
    void set_chart(const tfs::data_vector<double>& x,
                   const QVector<const tfs::data_vector<double>*>& y);

public:
    static void redirectMessageToLogWindow(QtMsgType type, const QMessageLogContext &context, const QString &msg);

};
#endif // VIEWER_H
