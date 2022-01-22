#include "logdialog.h"

#include <QShowEvent>
#include <QTextEdit>
#include <QPushButton>
#include <QLayout>

LogDialog::LogDialog(QWidget *parrent) : QDialog(parrent)
{
    setWindowTitle(tr("Log viewer"));
    setWindowFlags(Qt::Dialog);

    debugTextEdit = new QTextEdit();
    debugTextEdit->setReadOnly(true);
    QPalette palette = debugTextEdit->palette();
    palette.setBrush(QPalette::Active, QPalette::Text, Qt::darkGreen);
    palette.setBrush(QPalette::Active, QPalette::Base, Qt::black);
    palette.setBrush(QPalette::Inactive, QPalette::Text, Qt::darkGreen);
    palette.setBrush(QPalette::Inactive, QPalette::Base, Qt::black);
    debugTextEdit->setPalette(palette);
    debugTextEdit->append(tr("Tpaint log viewer"));
    closeButton = new QPushButton(tr("Close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->addStretch();
    bottomLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(debugTextEdit);
    mainLayout->addLayout(bottomLayout);
}

void LogDialog::debugMessageReceived(const QString& str) {
    debugTextEdit->append(str);
}

void LogDialog::showEvent(QShowEvent *event) {
    if (!dialogGeometryRect.isNull()) this->setGeometry(dialogGeometryRect);
    event->accept();
}

void LogDialog::closeEvent(QCloseEvent *event) {
    dialogGeometryRect = this->geometry();
    event->accept();
}
