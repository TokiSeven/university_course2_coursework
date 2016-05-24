#include "game_net.h"

Game_net::Game_net(Container *cont, QObject *parent)
    : network_main(cont->getServerPort(), cont->getPlayerPort(), parent)
{
    ////qDebug() << QString("Game_net-->> ") + "Created.";
    this->cont = cont;

    this->socketListen();

    timer_sendPlayer.setInterval(111);
    timer_server_answer.setInterval(3157);
    timer_answer.setInterval(2643);

    connect(&timer_server_answer, SIGNAL(timeout()), this, SLOT(slot_game_close()));
    connect(&timer_answer, SIGNAL(timeout()), this, SLOT(timeOut()));
    connect(&timer_sendPlayer, SIGNAL(timeout()), this, SLOT(slot_update()));
    connect(this, SIGNAL(connected()), &timer_sendPlayer, SLOT(start()));
    //connect(cont, SIGNAL(signal_update_all()), this, SLOT(slot_update()));
}

Game_net::~Game_net()
{
    ////qDebug() << QString("Game_net-->> ") + "~Game_net()";
    //this->cont->slot_game_close();
    //emit signal_closed();
    //    if (this)
    //        delete this;
}

//                          =====================
//                          <<__SENDING PLAYER
//                          =====================
void Game_net::sendPlayer()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out << cont->getPlayer_current().getName();
    out << Data::_CMD(_update);
    out << cont->getPlayer_current();

    this->sendMessage(data, cont->getServerIp());
}

//                          ==================
//                          <<__CHECK DATAGRAM
//                          ==================
void Game_net::check_data(QDataStream &in, QHostAddress IP)
{
    QString pl_name, cmd_qs;
    in >> pl_name;
    in >> cmd_qs;
    COMMAND cmd = Data::_CMD(cmd_qs);

    //qDebug() << pl_name + "::" + Data::_CMD(cmd);

    if (cmd == _update)
    {
        Data pl;
        in >> pl;
        cont->updatePlayer(pl);
    }
    else if (cmd == _players)
    {
        timer_server_answer.start();
        QList<Data> players;
        in >> players;
        cont->updatePlayers(players);
    }
    else if (cmd == _login && pl_name == QString::fromStdString("SERVER"))
    {
        QString ans;
        in >> ans;
        if (ans == QString::fromStdString("YES"))
        {
            this->cont->setServerIp(IP);

            timer_server_answer.start();
            timer_answer.stop();

            emit connected();
        }
        else if (ans == QString::fromStdString("NO"))
        {
            emit nick_incorrect();
        }
    }
}

void Game_net::slot_game_close()
{
    ////qDebug() << QString("Game_net-->> ") + "slot_game_close()";
    //emit signal_closed();
}

void Game_net::slot_update()
{
    sendPlayer();
}

void Game_net::slot_connect(QString nick, QString server)
{
    this->connectToServer(server, nick);
}

void Game_net::connectToServer(const QString serv_ip, QString pl_name)
{
    this->cont->getPlayer_current_pointer()->name = pl_name.toStdString();

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out << pl_name;
    out << Data::_CMD(_login);

    this->sendMessage(data, QHostAddress(serv_ip));

    timer_answer.start();
}

void Game_net::timeOut()
{
    ////qDebug() << QString("Game_net-->> ") + "timeOut()";
    timer_answer.stop();
    emit disconnected();
}
