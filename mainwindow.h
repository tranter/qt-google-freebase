#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void startLoginDefault();

private slots:
    void startLogin();
    void startFreebaseLogin();
    void openSchemeExplorer();
    void showStatusText(const QString& msg);

private:
    Ui::MainWindow * ui;
    class QLabel * m_pStatusLabel;
    class Form * m_pForm;
};

#endif // MAINWINDOW_H
