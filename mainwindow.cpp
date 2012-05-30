#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

#include "schemeexplorerdialog.h"

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
    connect(ui->actionPeople_Info,SIGNAL(triggered()),this,SLOT(startDlgDemo()));
    connect(ui->actionDemo_Country_Info,SIGNAL(triggered()),this,SLOT(startDlgDemoCountry()));
    connect(ui->actionSchemeExplorer, SIGNAL(triggered()), this, SLOT(openSchemeExplorer()));

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

void MainWindow::startDlgDemo()
{
    m_pForm->startDlgDemo();
}

void MainWindow::startDlgDemoCountry()
{
    m_pForm->startDlgDemoCountry();
}

void MainWindow::openSchemeExplorer()
{
    SchemeExplorerDialog dialog(this);
    dialog.resize(600, 400);
    dialog.exec();
}
