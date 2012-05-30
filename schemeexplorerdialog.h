#ifndef SCHEMEEXPLORERDIALOG_H
#define SCHEMEEXPLORERDIALOG_H

#include "schemeexplorer.h"

#include <QDialog>

namespace Ui {
class SchemeExplorerDialog;
}

class SchemeExplorerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SchemeExplorerDialog(QWidget *parent = 0);
    ~SchemeExplorerDialog();

    void setSelectionMode(SchemeExplorer::DataType type);
    QString selectedId() const;
    
private:
    Ui::SchemeExplorerDialog *ui;
};

#endif // SCHEMEEXPLORERDIALOG_H
