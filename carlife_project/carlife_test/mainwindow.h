#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMouseEvent>
#include <QPushButton>
#include "CarlifeSdk.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

public slots:
    void connectButtonSlot();
    void disconnectButtonSlot();

private:
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QWidget *w_bottom;

    //Carlife callback
    static void completionCB(RESULT_TYPE);
    static void screenAcquireCB(void);
    static void screenReleaseCB(void);

    static S_CARLIFE_DELEGATE carlifeDelegate;
};

#endif // MAINWINDOW_H
