#ifndef WLOG_H
#define WLOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class wLog;
}

class wLog : public QDialog
{
    Q_OBJECT

public:
    explicit wLog(QWidget *parent = 0);
    ~wLog();
    QTableWidget* table();
    void setFocusOnDefaultBtn();
    void setNextLogRowsColor();
    QColor logRowsColor();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::wLog *ui;
    bool NextLogRowsColor;
    void showEvent(QShowEvent * event);
};

#endif // WLOG_H
