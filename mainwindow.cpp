#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTemporaryFile>
#include <QSvgWidget>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QtWidgets>
#include <QDebug>

MainWindow::MainWindow(RInside &R, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_R(R)
{
    ui->setupUi(this);


    m_svg = new QSvgWidget();
    QHBoxLayout *svgLayout = new QHBoxLayout;
    svgLayout->addWidget(m_svg);

    ui->widgetSVG->setLayout(svgLayout);
    m_tempfile = QString::fromStdString(Rcpp::as<std::string>(m_R.parseEval("tfile <- tempfile(fileext=\".svg\")")));
    m_svgfile = QString::fromStdString(Rcpp::as<std::string>(m_R.parseEval("sfile <- tempfile(fileext=\".svg\")")));


    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//===========================================================================
//
//===========================================================================
void MainWindow::filterFile()
{
    // cairoDevice creates richer SVG than Qt can display
    // but per Michaele Lawrence, a simple trick is to s/symbol/g/ which we do here
    QFile infile(m_tempfile);
    infile.open(QFile::ReadOnly);
    QFile outfile(m_svgfile);
    outfile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream in(&infile);
    QTextStream out(&outfile);
    QRegExp rx1("<symbol");
    QRegExp rx2("</symbol");
    while (!in.atEnd()) {
        QString line = in.readLine();
        line.replace(rx1, "<g"); // so '<symbol' becomes '<g ...'
        line.replace(rx2, "</g");// and '</symbol becomes '</g'
        out << line << "\n";
    }
    infile.close();
    outfile.close();
}

//===========================================================================
// Почемуто записывается файл eps где вместо точек запятые и он не читается. меняем все запятые на точки
//===========================================================================
void MainWindow::filterFileEps(const QString &inputfile, const QString &outputfile)
{
    QFile infile(inputfile);
    infile.open(QFile::ReadOnly);
    QFile outfile(outputfile);
    outfile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream in(&infile);
    QTextStream out(&outfile);
    QRegExp rx1(",");
    //QRegExp rx2("</symbol");
    while (!in.atEnd()) {
        QString line = in.readLine();
        line.replace(rx1, "."); // so '<symbol' becomes '<g ...'
        //line.replace(rx2, "</g");// and '</symbol becomes '</g'
        out << line << "\n";
    }
    infile.close();
    outfile.close();
}

//===========================================================================
// Открыть новый файл для чтения
//===========================================================================
void MainWindow::on_actionOpen_dsc_2_triggered()
{
    cur_xmin = 1000000.0;
    cur_xmax = -1000000.0;
    cur_ymin = 1000000.0;
    cur_ymax = -1000000.0;

    cur_XTitle = "X";

    cur_YTitle = "Y";

    if(openNewFile("datas0"))
    {

        curDataNum = 0;
        plot();
        axisRescale();
        axisRename();
        plotSvg();
        ui->actionAdd_dsc->setEnabled(true);
    }
}

//===========================================================================
// Открыть файл для чтения
//===========================================================================
bool MainWindow::openNewFile(const QString &dataNum)
{
    QString fileName = QFileDialog::getOpenFileName(this,
                tr("Open DSC data file"), recentDirectory, ("DSM files(*.dsm);;All Files(*)"));

        if (!fileName.isEmpty())
        {
            recentDirectory = QFileInfo(fileName).absolutePath();

            if(QFileInfo(fileName).completeSuffix() == "dsm")
            {
                setCurrentFile(fileName);
                //m_R["fname"] = fileName;

                std::string cmd0 = fileName.toUtf8().constData();

                m_R["fname"] = cmd0;

                // For debug reassons ============================================
//              std::string txt_debug =  "cat('Running ls()\n'); print(ls()); cat('\fname=', fname, '\n'); print(class(fname));";
//              m_R.parseEvalQ(txt_debug);
                //================================================================

                //m_R.parseEvalQ(cmd);

                loadRlibraries();

                std::string datStr = dataNum.toStdString();

                cmd0 = datStr +" <- readandconvert(fname,20);";

                m_R.parseEvalQ(cmd0);

                double x_min = Rcpp::as< double >(m_R.parseEval(datStr+"$V1[1]"));
                double x_max = Rcpp::as< double >(m_R.parseEval("tail("+datStr+"$V1,n=1)"));

                double y_min = Rcpp::as< double >(m_R.parseEval("min("+datStr+"$V2)"));
                double y_max = Rcpp::as< double >(m_R.parseEval("max("+datStr+"$V2)"));

                if(x_min < cur_xmin)
                {
                    m_R["x_min"] = x_min;
                    cur_xmin = x_min;
                }
                if(x_max > cur_xmax)
                {
                    m_R["x_max"] = x_max;
                    cur_xmax = x_max;
                }
                if(y_min < cur_ymin)
                {
                    m_R["y_min"] = y_min;
                    cur_ymin = y_min;
                }
                if(y_max > cur_ymax)
                {
                    m_R["y_max"] = y_max;
                    cur_ymax = y_max;
                }
                double x_delta = (cur_xmax-cur_xmin)/5;
                double y_delta = (cur_ymax-cur_ymin)/5;

                m_R["x_delta"] = x_delta;
                m_R["y_delta"] = y_delta;

                //m_R.parseEvalQ("x_min = datas2$V1[1]; x_max = tail(datas2$V1,n=1)");
                //m_R.parseEvalQ("y_min = min(datas2$V2); y_max = max(datas2$V2)");

                //m_R.parseEvalQ("x_delta = (x_max - x_min)/5");
                //m_R.parseEvalQ("y_delta = (y_max - y_min)/5");

                //double x_min = Rcpp::as< double >(m_R.parseEval("x_min"));
                //double x_max = Rcpp::as< double >(m_R.parseEval("x_max"));
                //double y_min = Rcpp::as< double >(m_R.parseEval("y_min"));
                //double y_max = Rcpp::as< double >(m_R.parseEval("y_max"));

                //double x_delta = Rcpp::as< double >(m_R.parseEval("x_delta"));
                //double y_delta = Rcpp::as< double >(m_R.parseEval("y_delta"));

                ui->lineEditXmin->setText(QString::number(cur_xmin));
                ui->lineEditXmax->setText(QString::number(cur_xmax));
                ui->lineEditYmin->setText(QString::number(cur_ymin));
                ui->lineEditYmax->setText(QString::number(cur_ymax));

                ui->lineEditXdelta->setText(QString::number(x_delta));
                ui->lineEditYdelta->setText(QString::number(y_delta));

                ui->lineEditXTitle->setText(cur_XTitle);
                ui->lineEditYTitle->setText(cur_YTitle);



                return true;

//                // For debug reassons ============================================
//                txt_debug = "cat('Running ls()\n'); print(ls());  print(datas2); print(class(datas2));";
//
//                m_R.parseEvalQ(txt_debug);
                // For debug reassons ============================================
            }
        }
        return false;
}

//===========================================================================
// Реакция на закрытие программы
//===========================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

//===========================================================================
// Установка текущего открытого файла
//===========================================================================
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);
    QString shownName = tr("Untitled");
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("PlotDSM-1.0")));
}

//===========================================================================
// Взять только имя файла из строки с указанием пути и имени файла
//===========================================================================
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//===========================================================================
// Записать в файл основые установки программы
//===========================================================================
void MainWindow::writeSettings()
{
    QSettings settings("DSM Plot Application.", "PlotWithR");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentDirectory", recentDirectory);
    settings.setValue("recentSaveDirectory", recentSaveDirectory);
}

//===========================================================================
// Считать из файла основые установки программы
//===========================================================================
void MainWindow::readSettings()
{
    QSettings settings("DSM Plot Application.", "PlotWithR");

    restoreGeometry(settings.value("geometry").toByteArray());

    recentDirectory = settings.value("recentDirectory").toString();
    recentSaveDirectory = settings.value("recentSaveDirectory").toString();
}

//===========================================================================
// R load library
//===========================================================================
void MainWindow::loadRlibraries()
{
    std::string txt = "suppressMessages(library(ggplot2))";
    m_R.parseEvalQ(txt);

    txt = "suppressMessages(library(scales))";
    m_R.parseEvalQ(txt);

    txt = "suppressMessages(library(gridExtra))";
    m_R.parseEvalQ(txt);

    // std::string => QByteArray
    //QByteArray byteArray(stdString.c_str(), stdString.length());

    // QByteArray => std::string
    //std::string stdString(byteArray.constData(), byteArray.length());

    // Считать фунуцию из R файла и загрузить ее в R
    QString qrcdata;
    QString fileName(":/Rfile/dsmread.R");

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"filenot opened"<<endl;
    }
    else
    {
        //qDebug()<<"file opened"<<endl;
        qrcdata = file.readAll();
    }

    file.close();

    //qDebug()<<qrcdata<<endl;

    txt = qrcdata.toStdString();

    //txt = "source('/home/dumsky/R/dsmread.R')";
    m_R.parseEvalQ(txt);

}

//===========================================================================
// Rescale
//===========================================================================
void MainWindow::axisRescale()
{
    std::string cmd2 = "scale_x_continuous(expand = c(0, 0), limits=c(x_min,x_max), breaks=seq(from = x_min, to = x_max, by = x_delta)) +";
    std::string cmd3 = "scale_y_continuous(limits=c(y_min,y_max), breaks=seq(from = y_min, to = y_max, by = y_delta))";

    std::string cmd = "bp = bp + " + cmd2 + cmd3;

    //std::string cmd = "bp + " + cmd2 + cmd3;

    m_R.parseEvalQ(cmd);
}

//===========================================================================
// Rename Axis
//===========================================================================
void MainWindow::axisRename()
{
    std::string cmd2 = "labs(title = \"\", x = \"" + cur_XTitle.toStdString() + "\", y = \"" + cur_YTitle.toStdString() + "\", color = \"\n\", size=16) +";
    //std::string cmd2 = "scale_x_continuous(expand = c(0, 0), limits=c(x_min,x_max), breaks=seq(from = x_min, to = x_max, by = x_delta)) +";
    //std::string cmd3 = "scale_y_continuous(limits=c(y_min,y_max), breaks=seq(from = y_min, to = y_max, by = y_delta))";

    std::string cmd = "bp = bp + " + cmd2;

    m_R.parseEvalQ(cmd);
}


//===========================================================================
// Plot svg for qt widget display
//===========================================================================
void MainWindow::plotSvg()
{
    std::string cmd = "svg(width=10,height=5,pointsize=10,filename=tfile); print(bp); dev.off();";
    //std::string cmd10 = "print(bp); dev.off();";

    m_R.parseEvalQ(cmd);
    filterFile();           	// we need to simplify the svg file for display by Qt
    m_svg->load(m_svgfile);
}

//===========================================================================
// R plot
//===========================================================================
void MainWindow::plot(void) {

    m_R["line_with"] = 2;
   // m_R["x_min"] = -20;
   // m_R["x_max"] = 200;
   // m_R["x_delta"] = 50;
   // m_R["y_delta"] = 1000;

    //std::string cmd0 = "svg(width=10,height=5,pointsize=10,filename=tfile); ";
    std::string cmd1 = "bp = ggplot() + geom_line(data = datas0, aes(x = V1, y = V2), colour=\"black\", size=line_with)+";
//    std::string cmd2 = "scale_x_continuous(expand = c(0, 0), limits=c(x_min,x_max), breaks=seq(from = x_min, to = x_max, by = x_delta)) +";
//    std::string cmd3 = "scale_y_continuous(limits=c(y_min,y_max), breaks=seq(from = y_min, to = y_max, by = y_delta)) +";
//    std::string cmd4 = "labs(title = \"\", x = \"DP\", y = \"P\", color = \"\n\", size=16) +";
    std::string cmd5 = "theme_bw() +";
    std::string cmd6 = "theme(axis.text.x = element_text(colour = \"black\", size = 16), axis.title.x = element_text(size = 16,hjust = 0.5)) +";
    std::string cmd7 = "theme(axis.text.y = element_text(colour = \"black\", size = 16), axis.title.y = element_text(size = 16,hjust = 0.5)) +";
    std::string cmd8 = "theme(panel.border = element_blank(),  axis.line = element_line(size = 0.5, colour = \"black\")) +";
    std::string cmd9 = "theme(panel.background = element_rect(fill = \"white\"),plot.margin = margin(0.5, 2, 0.5, 0.2, \"cm\"),plot.background = element_rect(fill = \"white\", colour = \"white\", size=0));";
    //std::string cmd9 = " ggsave(file=tfile,plot=bp, width = 10, height = 5, dpi = 300);";
    //std::string cmd10 = "print(bp); dev.off();";
    std::string cmd = cmd1  + /*cmd4 +*/ cmd5 + cmd6 + cmd7 + cmd8 + cmd9; //
    m_R.parseEvalQ(cmd);
   // qDebug() << QString::fromStdString(cmd);


}

//bp <- ggplot() +
//#  geom_line(data = datas1, aes(x = V1, y = V2), colour="black", size=2)+
//  geom_line(data = datas2, aes(x = V1, y = V2), colour="black", size=2)+
// # geom_line(data = ptline, aes(x = V1, y = V2), colour="gray50", size=0.5)+
// # scale_y_continuous(limits=c(-7500,-3000), breaks=seq(from = -1000, to = -7500, by = -500)) +
//  scale_x_continuous(expand = c(0, 0), limits=c(-20,200), breaks=seq(from = -30, to = 200, by = 2)) +
//  labs(title = " ", x = "DP", y = "P", color = "\n", size=16) +
//  theme_bw() +
//  theme(axis.text.x = element_text(colour = 'black', size = 16), axis.title.x = element_text(size = 16,hjust = 0.5, vjust = 0.2)) +
//  theme(axis.text.y = element_text(colour = 'black', size = 16), axis.title.y = element_text(size = 16,hjust = 0.5, vjust = 0.2)) +
//  theme(panel.border = element_blank(),  axis.line = element_line(size = 0.5, colour = "black")) +
//  theme(panel.background = element_rect(fill = "white"),plot.margin = margin(0.5, 2, 0.5, 0.2, "cm"),plot.background = element_rect(fill = "white",colour = "white",size=0))

//===========================================================================
// Добавить еще один график к уже существующим
//===========================================================================
void MainWindow::addplotline(const QString &dataNameN)
{
    //std::string cmd0 = "svg(width=10,height=5,pointsize=10,filename=tfile); ";
    QString cmd1str = "bp = bp + geom_line(data = " + dataNameN + ", aes(x = V1, y = V2), colour=\"black\", size=line_with)";
    std::string cmd1 = cmd1str.toStdString();
    //std::string cmd2 = "print(bp); dev.off();";

    m_R.parseEvalQ(cmd1);

    //m_R.parseEvalQ("print("+dataNameN.toStdString()+")");

    //filterFile();           	// we need to simplify the svg file for display by Qt
    //m_svg->load(m_svgfile);
}

//===========================================================================
// Кнопка сохранить eps
//===========================================================================
void MainWindow::on_pushButton_clicked()
{
    saveAs();
}

//===========================================================================
// Диалог сохранения файла под именем ...
//===========================================================================
bool MainWindow::saveAs()
{
//    QString fileName = QFileDialog::getSaveFileName(this,
//                               tr("Save ..."), ".",
//                               tr("PLS files (*.sct)"));

    QString fileName = QFileDialog::getSaveFileName(this,
       tr("Save EPS for Latex"), recentSaveDirectory, ("EPS file(*.eps)"));

    if (fileName.isEmpty())
        return false;
    recentSaveDirectory = QFileInfo(fileName).absolutePath();
    m_R.parseEvalQ("ggsave(\"badcommaeps.eps\", width = 10, height = 5, dpi = 300);");
    filterFileEps("badcommaeps.eps",fileName);

    return true;
}

//===========================================================================
// перерисовать с новыми параметрами
//===========================================================================
void MainWindow::on_pushButton_2_clicked()
{
    m_R["x_min"] = ui->lineEditXmin->text().toDouble();
    m_R["x_max"] = ui->lineEditXmax->text().toDouble();

    m_R["y_min"] = ui->lineEditYmin->text().toDouble();
    m_R["y_max"] = ui->lineEditYmax->text().toDouble();

    m_R["x_delta"] = ui->lineEditXdelta->text().toDouble();
    m_R["y_delta"] = ui->lineEditYdelta->text().toDouble();

    cur_XTitle = ui->lineEditXTitle->text();
    cur_YTitle = ui->lineEditYTitle->text();

    axisRescale();
    axisRename();
    plotSvg();

    //plot();
}

//===========================================================================
// Добавить еще один график к уже существующим
//===========================================================================
void MainWindow::on_actionAdd_dsc_triggered()
{
    curDataNum++;
    QString dataNum = "datas"+QString::number(curDataNum);
    qDebug() << dataNum;
    if(openNewFile(dataNum))
    {
        addplotline(dataNum);
        axisRescale();
        plotSvg();
    }
    else
    {
        curDataNum--;
    }
}
