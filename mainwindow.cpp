#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "poissonmatting.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = new MattingCanvas(this);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(canvas);
    ui->gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    filename = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    pmat.open(filename);
    canvas->setImage(pmat.getImage());
}
