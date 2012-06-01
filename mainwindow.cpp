#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

#include "schemeexplorerdialog.h"

MainWindow::MainWindow(QWidget * p) :
    QMainWindow(p),
    ui(new Ui::MainWindow),
    m_pStatusLabel(new QLabel(this)),
    m_pForm(new Form(this))
{
    ui->setupUi(this);
    setCentralWidget(m_pForm);

    connect(ui->actionExit,             SIGNAL(triggered()), this,      SLOT(close()));
    connect(ui->actionLogin,            SIGNAL(triggered()), this,      SLOT(startLogin()));
    connect(ui->actionFreebaseLogin,    SIGNAL(triggered()), this,      SLOT(startFreebaseLogin()));
    connect(ui->actionSchemeExplorer,   SIGNAL(triggered()), this,      SLOT(openSchemeExplorer()));
    connect(ui->actionPeople_Info,      SIGNAL(triggered()), m_pForm,   SLOT(startDlgDemo()));
    connect(ui->actionDemo_Country_Info,SIGNAL(triggered()), m_pForm,   SLOT(startDlgDemoCountry()));
    connect(ui->actionFreebaseExplorer, SIGNAL(triggered()), m_pForm,   SLOT(openFreebaseExplorer()));

    connect(m_pForm, SIGNAL(showStatusText(QString)), this, SLOT(showStatusText(QString)));

    QLabel* permanentWidget = new QLabel(this);
    permanentWidget->setText("Based on Freebase API: http://www.freebase.com");

    statusBar()->addPermanentWidget(permanentWidget);
    statusBar()->addWidget(m_pStatusLabel);
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

void MainWindow::openSchemeExplorer()
{
    SchemeExplorerDialog dialog(this);
    dialog.resize(600, 400);
    dialog.exec();
}
