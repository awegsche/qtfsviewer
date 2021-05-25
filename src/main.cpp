#include "darkstyle.h"
#include "viewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCommandLineParser parser;
    parser.addPositionalArgument("file", "tfs file to open");

    qInstallMessageHandler(Viewer::redirectMessageToLogWindow);
    qDebug() << "starting application";

    //QApplication::setStyle(new DarkStyle);
    QApplication a(argc, argv);
    parser.process(a);
    Viewer w;
    const auto args = parser.positionalArguments();
    if (args.count() > 0)
        w.open_tfs(args[0]);

    w.show();
    return a.exec();
}
