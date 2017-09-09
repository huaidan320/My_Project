#include <QDebug>
#include "mainwindow.h"

static S_CARLIFE_DELEGATE MainWindow::carlifeDelegate = {
    MainWindow::completionCB,
    MainWindow::screenAcquireCB,
    MainWindow::screenReleaseCB
};

MainWindow::MainWindow()
{
    w_bottom = new QWidget(this);
    w_bottom->setGeometry(0, 0, 1600, 480);
    w_bottom->setStyleSheet("QWidget{background-color:rgba(70,130,180,255);}");

    connectButton = new QPushButton(w_bottom);
    connectButton->setGeometry(200, 200, 100, 80);
    connectButton->setStyleSheet("QPushButton:pressed{background-color:rgb(219,112,147);}"
                                 "QPushButton{font-family:'Microsoft YaHei';font-size:15px;}");
    connectButton->setText("Connect");

    connect(connectButton, SIGNAL(clicked(bool)), this, SLOT(connectButtonSlot()));

    //background-color:transparent;"QPushButton{border-radius:30px;border:2px groove transparent;border-style:outset;}"

    disconnectButton = new QPushButton(w_bottom);
    disconnectButton->setGeometry(1300, 200, 100, 80);
    disconnectButton->setStyleSheet("QPushButton:pressed{background-color:rgb(219,112,147);}"
                                    "QPushButton{font-family:'Microsoft YaHei';font-size:15px;}");
    //color:#666666;
    disconnectButton->setText("Disconnect");

    connect(disconnectButton, SIGNAL(clicked(bool)), this, SLOT(disconnectButtonSlot()));
}

MainWindow::~MainWindow()
{
    //disconnect
    //delete
}

//TODO: Carlife touch event(ctrlTouchActionDown, ctrlTouchActionUp,
//ctrlTouchActionMove, ctrlTouchSigleClick, ctrlTouchDoubleClick,
//ctrlTouchLongPress, ctrlTouchCarHardKeyCode)
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    qDebug("press is (%d, %d).", e->pos().x(), e->pos().y());

    //TODO: Return value processing
    CarlifeSDK::getInstance()->carlifeTouch(e->pos().x(), e->pos().y(), ACTION_DOWN);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug("release is (%d, %d).", e->pos().x(), e->pos().y());
}

void MainWindow::connectButtonSlot()
{
    qDebug() << "connectButtonSlot";

    //Init carlife parameters
    //TODO: Return value processing
    CarlifeSDK::getInstance()->carlifeInit(NULL, &carlifeDelegate);

    //Carlife connect start
    //TODO: Return value processing
    CarlifeSDK::getInstance()->carlifeStart();
}

void MainWindow::disconnectButtonSlot()
{
    qDebug() << "disconnectButtonSlot";

    //Stop carlife
    //TODO: Return value processing
    CarlifeSDK::getInstance()->carlifeStop();

    //Unkown
    CarlifeSDK::getInstance()->carlifeDstory();
}

void MainWindow::completionCB(RESULT_TYPE)
{
    //TODO: Connected success or failed
}

void MainWindow::screenAcquireCB()
{
    //TODO: Unkown
}

void MainWindow::screenReleaseCB()
{
    //TODO: Unkown
}
