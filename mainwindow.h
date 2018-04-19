#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <RInside.h>


class QTemporaryFile;
class QSvgWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(RInside & R, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_dsc_2_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionAdd_dsc_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QSvgWidget *m_svg;          // the SVG device
    RInside & m_R;              // reference to the R instance passed to constructor
    QString m_tempfile;         // name of file used by R for plots
    QString m_svgfile;          // another temp file, this time from Qt
    QString recentDirectory;
    QString recentSaveDirectory;
    QString curFile;
    QString cur_XTitle;
    QString cur_YTitle;
    int curDataNum;
    double cur_xmin;
    double cur_xmax;
    double cur_ymin;
    double cur_ymax;


    void filterFile(void);      // modify the richer SVG produced by R
    void writeSettings();
    void readSettings();
    QString strippedName(const QString &fullFileName);
    void setCurrentFile(const QString &fileName);
    void loadRlibraries();
    void plot();
    void filterFileEps(const QString &inputfile, const QString &outputfile);
    bool saveAs();
    bool openNewFile(const QString &dataNum);
    void addplotline(const QString &dataNameN);
    void plotSvg();
    void axisRescale();
    void axisRename();
};

#endif // MAINWINDOW_H
