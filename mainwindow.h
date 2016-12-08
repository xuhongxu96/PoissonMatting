#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include "poissonmatting.h"
#include "mattingcanvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionDraw_Unknown_triggered(bool checked);

    void on_actionMark_Foreground_triggered();

    void on_actionClear_triggered();

private:
    Ui::MainWindow *ui;
    QString filename;
    PoissonMatting pmat;
    MattingCanvas *canvas;
    QScrollArea *scrollArea;

};

#endif // MAINWINDOW_H
