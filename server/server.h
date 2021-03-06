#ifndef SERVER_H
#define SERVER_H

#include <QTimer>
#include "../game/data.h"
#include "network_main.h"

class Server : public network_main
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0, quint16 port_s = 1100, quint16 port_l = 2200);
    ~Server();

    //inline selectors
    inline const QList<Data> getPlayers()const{return this->players;}//return all players

signals:
    void signal_newPlayer(QString name);
    void signal_deletePlayer(QString name);

private slots:
    void checkWhoIsHere();//send on timer to all players and check their status
    void slot_sendPlayersToAll(){this->sendPlayersToAll();}

protected:
    void check_data(QDataStream&, QHostAddress);

private:
    //need for resend to players
    void sendPlayer(Data&);//do it, if player doing something
    void sendPlayerWhoIsHere();//check all who offline
    void sendAuth(QHostAddress, bool);//send to ip that he auth
    void sendPlayersToAll();//send all players to all
    void sendKey(QString, QString);

    bool checkAuth(const QString);//check authorization
    int searchPlayer(QHostAddress, QString);//search player by ip and name and return his number in list

    QList<Data> players;//all players

    QTimer timer_to_ask, timer_to_send_all;//timer for sending to players message "who are here?"
};

#endif // SERVER_H
