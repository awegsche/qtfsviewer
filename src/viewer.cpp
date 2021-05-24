#include "viewer.h"
#include "ui_viewer.h"
#include <qdebug.h>

#include <qfiledialog.h>
#include "tfsmodel.h"
#include "darkstyle.h"

QStringListModel message_model;




Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Viewer)
    , df(nullptr)
{
    qDebug() << "about to start main window";
    ui->setupUi(this);

    //ui->formLayout->addWidget(new QChart());
    //ui->graphicsView->setScene(&scene);

    auto* vHead = ui->propertyTable->verticalHeader();

    vHead->setDefaultSectionSize(18);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor::fromRgbF(0.12156, 0.46667, 0.70588));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(1.0, 0.4980392156862745, 0.054901960784313725));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.17254901960784313, 0.6274509803921569, 0.17254901960784313));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.8392156862745098, 0.15294117647058825, 0.1568627450980392));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.5803921568627451, 0.403921568627451, 0.7411764705882353));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.5490196078431373, 0.33725490196078434, 0.29411764705882354));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.8901960784313725, 0.4666666666666667, 0.7607843137254902));
    plot_colors.append(pen);
    pen.setColor(QColor::fromRgbF(0.4980392156862745, 0.4980392156862745, 0.4980392156862745));
    plot_colors.append(pen);

    //message_model.setStringList(QStringList());
    ui->logView->setModel(&message_model);

    ui->chartArea->setVisible(false);
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setOpenGl(true, 2);
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->legend->setFont(QFont("Helvetica", 12));

    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(12);

    connect(&message_model, SIGNAL(rowsInserted(QModelIndex, int,int)), ui->logView, SLOT(scrollToBottom()));

    QAction *search = new QAction(this);
    search->setShortcut(Qt::CTRL | Qt::Key_F);

    connect(search, &QAction::triggered, this, &Viewer::jump_to_search);
    this->addAction(search);

}

Viewer::~Viewer()
{
    delete ui;
    delete this->df;
    delete this->model;
}


void Viewer::on_actionOpen_triggered()
{
    auto filename = QFileDialog::getOpenFileName(this, tr("Open TFS file"),
                                                 "K:/CERN/tfs", tr("TFS files (*.tfs *.dat);;Binary TFS files (*.btfs);;all files (*.*)"));
    open_tfs(filename);

}

void Viewer::set_chart(const std::string &name, const std::vector<double> &points)
{
    QVector<double> x(points.size());
    QVector<double> y(points.size());

    double min_x, max_x, min_y = 1.0e12, max_y = 1.0e-12;

    for (size_t i = 0; i < points.size(); i++) {
        x[i] = i;
        y[i] = points[i];

        min_y = y[i] < min_y ? y[i] : min_y;
        max_y = y[i] > max_y ? y[i] : max_y;
    }

    min_x = 0;
    max_x = points.size();

    ui->customPlot->clearGraphs();

    ui->customPlot->legend->setVisible(true);
    //ui->customPlot->legend->setFont(QFont("Helvetica", 9));
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x,y);
    ui->customPlot->graph(0)->setName(QString::fromStdString(name));
    ui->customPlot->graph(0)->addToLegend();

    ui->customPlot->xAxis->setLabel(QString("index"));
    ui->customPlot->xAxis->setRange(min_x, max_x);
    ui->customPlot->yAxis->setRange(min_y, max_y);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->replot();
    ui->chartArea->setVisible(true);
}

void Viewer::set_chart(const std::string &name,
                       const std::vector<double> &points_x,
                       const QVector<QVector<double>>& cols_y)
{
    ui->customPlot->clearGraphs();

    QVector<double> x(points_x.size());
    double min_x=1.0e12, max_x=1.0e-12, min_y = 1.0e12, max_y = 1.0e-12;

    for (auto& coly : cols_y) {
        QVector<double> y(points_x.size());
        for (size_t i = 0; i < points_x.size(); i++) {
            x[i] = points_x[i];
            y[i] = coly[i];

            min_y = y[i] < min_y ? y[i] : min_y;
            max_y = y[i] > max_y ? y[i] : max_y;
            min_x = x[i] < min_x ? x[i] : min_x;
            max_x = x[i] > max_x ? x[i] : max_x;
        }
        auto graph = ui->customPlot->addGraph();
        graph->setData(x,y);
    }

    ui->customPlot->xAxis->setRange(min_x, max_x);
    ui->customPlot->yAxis->setRange(min_y, max_y);
    ui->customPlot->xAxis->setLabel(QString::fromStdString(name));
    ui->customPlot->replot();
    ui->chartArea->setVisible(true);

}

void Viewer::set_chart(const tfs::data_vector<double> &x, const QVector<const tfs::data_vector<double> *> &y)
{
    ui->actionplotColumn->setChecked(true);
    ui->actionscatter_plot_column->setChecked(false);
    if (x.get_type() != tfs::DataType::LE) return;
    ui->customPlot->clearGraphs();
    QVector<double> x_(x.size());
    for (double d : x.as_double_vector())
        x_.push_back(d);
    int clr_index = 0;
    for (auto y_ : y) {
        if (y_->get_type() != tfs::DataType::LE) continue;

        QVector<double> y_values(y_->size());
        for (double d : y_->as_double_vector())
            y_values.push_back(d);

        auto graph = ui->customPlot->addGraph();
        graph->setData(x_, y_values);
        graph->setName(QString::fromStdString(y_->get_name()));
        graph->setPen(plot_colors[clr_index]);
        clr_index = clr_index == (plot_colors.size() - 1) ? 0 : clr_index + 1;
    }

    ui->customPlot->xAxis->rescale();
    ui->customPlot->yAxis->rescale();
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->xAxis->setLabel(QString::fromStdString(x.get_name()));
    ui->customPlot->replot();
    ui->chartArea->setVisible(true);

}

void Viewer::redirectMessageToLogWindow(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(message_model.insertRow(message_model.rowCount())) {
        QModelIndex index = message_model.index(message_model.rowCount() - 1, 0);
        message_model.setData(index, msg);
    }

    //mainView->message_model.dataChanged(
    //            mainView->message_model.index(0),
    //            mainView->message_model.index(2)
    //            );
}

void Viewer::on_actionplotColumn_triggered()
{
    qDebug() << "try to plot";
    auto select = ui->tableView->selectionModel();

    if (!select || !select->hasSelection()) {
        qDebug() << "no selection";
        return;
    }
    if (select->selectedColumns().size() == 1) {
        auto& col = df->get_column(select->selectedColumns()[0].column());
        if (col.get_type() != tfs::DataType::LE) {
            qDebug() << "plot works only with %le columns";
            return;
        }
       set_chart("some column", col.as_double_vector());
       qDebug() << "plotting successful";
    }
    else if (select->selectedColumns().size() >= 2) {
        auto& colx = df->get_column(select->selectedColumns()[0].column());
        /*
        QVector<QVector<double>> coly(select->selectedColumns().size()-1);
        for (int i = 1; i< select->selectedColumns().size(); i++)
        {
            auto& c = df->get_column(select->selectedColumns()[i].column());
            if (c.type != tfs::DataType::LE)
            {
                qDebug() << "wrong column format";
                return;
            }
            coly.push_back(QVector<double>(c.size()));
            for (double d : c.payload.double_vector) {
                coly[i-1].push_back(d);
            }

        }
       set_chart(colx.name,
                 colx.payload.double_vector,
                 coly);
                 */
        QVector<const tfs::data_vector<double>*> y_columns;
        y_columns.reserve(select->selectedColumns().size());
        for (int i = 1; i< select->selectedColumns().size(); i++) {
            y_columns.push_back(&df->get_column(select->selectedColumns()[i].column()));
        }

        set_chart(colx, y_columns);

       qDebug() << "plotting successful";
    }
    else {
        qDebug() << "wrong number of columns selected: " << select->selectedColumns().size();
    }
}

void Viewer::on_closePlotButton_clicked()
{
    ui->chartArea->setVisible(false);
}

void Viewer::on_actionscatter_plot_column_triggered()
{
    on_actionplotColumn_triggered();

    for (int i = 0; i < ui->customPlot->graphCount(); i++)
    {
        ui->customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsNone);
        QCPScatterStyle myScatter;
        myScatter.setShape(QCPScatterStyle::ssCross);
        myScatter.setPen(plot_colors[i]);
        myScatter.setBrush(Qt::white);
        myScatter.setSize(4);
        ui->customPlot->graph(i)->setScatterStyle(myScatter);
    }
    ui->customPlot->replot();
}

void Viewer::on_actionSave_triggered()
{
    qWarning() << "save action not implemented yet";
}

void Viewer::on_actionSave_Compressed_triggered()
{
    if (!df) {
        qWarning() << "no TFS dataframe open";
        return;
    }

    auto filename = QFileDialog::getSaveFileName(this, tr("Save compressed TFS file"),
                                                 "", tr("TFS files (*.btfs)"));
    if (filename.isEmpty()) {
        qWarning() << "no filename selected. abort saving.";
        return;
    }

    df->to_binary_file(filename.toStdString());
}

void Viewer::open_tfs(const QString &filename)
{
    qDebug() << "try to open file " << filename;

    if (QFile::exists(filename)) {
        qDebug() << "file exists";

        if (df)
            delete df;

        if (filename.endsWith(".btfs"))
        {
            df = new tfs::dataframe<double>;
            df->load_from_binary_file(filename.toStdString());
        }
        else
            df = new tfs::dataframe<double>(filename.toStdString());

        qDebug() << "tfs file loaded";
        setWindowTitle(QString("TFS Viewer - %1").arg(QFileInfo(filename).fileName()));

        if (model)
            delete model;
        model = new TFSModel(df);
        ui->tableView->setModel(model);
        prop_model = new TfsPropertyModel(this->df);
        ui->propertyTable->setModel(prop_model);
    }
}


void Viewer::jump_to_search()
{
    ui->filterDataEdit->setFocus();
}

void Viewer::on_filterDataEdit_textChanged(const QString &arg1)
{
    if (!model) return;
    model->filter(arg1);

}
