#include "schemeexplorerdialog.h"
#include "ui_schemeexplorerdialog.h"

SchemeExplorerDialog::SchemeExplorerDialog(QWidget * p) :
    QDialog(p),
    ui(new Ui::SchemeExplorerDialog)
{
    ui->setupUi(this);
    connect(ui->schemeExplorer, SIGNAL(closeClicked()), this, SLOT(reject()));
}

SchemeExplorerDialog::~SchemeExplorerDialog()
{
    delete ui;
}

void SchemeExplorerDialog::
setSelectionMode(SchemeExplorer::DataType type)
{
    ui->schemeExplorer->selectionMode(type);

    if( SchemeExplorer::NONE != type ) {
        connect(ui->schemeExplorer, SIGNAL(idSelected(QString)), this, SLOT(accept()));
    } else {
        disconnect(ui->schemeExplorer, SIGNAL(idSelected(QString)), this, SLOT(accept()));
    }
}

QString SchemeExplorerDialog::
selectedId() const {
    return ui->schemeExplorer->selectedId();
}

QVariantList SchemeExplorerDialog::
selectedData() const {
    return ui->schemeExplorer->selectedData();
}
