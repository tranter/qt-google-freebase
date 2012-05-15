#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(startLogin()));
    connect(ui->actionFreebaseLogin, SIGNAL(triggered()), this, SLOT(startFreebaseLogin()));
    connect(ui->actionBackward,SIGNAL(triggered()),this,SLOT(goBackward()));
    connect(ui->actionForward,SIGNAL(triggered()),this,SLOT(goForward()));
    connect(ui->actionRun,SIGNAL(triggered()),this,SLOT(onRun()));

    QLabel* permanentWidget = new QLabel(this);
    permanentWidget->setText("Based on Freebase API: http://www.freebase.com");
    statusBar()->addPermanentWidget(permanentWidget);
    m_pStatusLabel = new QLabel(this);
    statusBar()->addWidget(m_pStatusLabel);

    m_pForm = new Form(this);
    setCentralWidget(m_pForm);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startLogin()
{
    m_pForm->startLogin(true);
}

void MainWindow::startFreebaseLogin()
{
    m_pForm->startFreebaseLogin();
}

void MainWindow::startLoginDefault()
{
    m_pForm->startLogin(false);
}

void MainWindow::showStatusText(const QString& msg)
{
    m_pStatusLabel->setText(msg);
}

void MainWindow::goBackward()
{
    m_pForm->onBacwardAction();
}

void MainWindow::goForward()
{
    m_pForm->onForwardAction();
}

void MainWindow::onRun()
{
    m_pForm->runQuery();
}
