#ifndef PROCESSING_H
#define PROCESSING_H

#include <QDialog>

namespace Ui {
class Processing;
}

class Processing : public QDialog
{
    Q_OBJECT

public:
    explicit Processing(QWidget *parent = 0);
    ~Processing();
    void setText(QString s);
    bool wasCancelled();
    void reject();
    void showEvent( QShowEvent * event );
    void setProgress(int percent);

private:
    Ui::Processing *ui;
    bool _wasCancelled;
    QWidget* Parent;
};

#endif // PROCESSING_H
