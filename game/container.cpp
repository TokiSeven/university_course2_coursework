#include "container.h"

Container::Container(QString pl_name, QHostAddress s_ip, quint16 s_port, quint16 p_port, QObject *parent) : QObject(parent)
{
    this->server_ip = s_ip;
    this->server_port = s_port;
    this->player_port = p_port;

    pl.setIp(QHostAddress::LocalHost);
    pl.setName(pl_name);
    pl.setOnline(true);
}

Container::~Container()
{
    emit signal_closed();
}

void Container::slot_game_close()
{
    emit signal_closed();
    //    if (this)
    //        delete this;
}

void Container::updatePlayers(QList<Player> players)
{
    players.clear();
    players.append(players);
    emit signal_update_all();
}

void Container::addPlayer(Player player)
{
    players.append(player);
    emit signal_update_current();
}

void Container::deletePlayer(int num)
{
    if (num >= 0 && num < players.size())
    {
        players.removeAt(num);
        emit signal_update_all();
    }
}

void Container::updatePlayer(Player pl)
{
    int num = players.indexOf(pl);
    if (num != -1)
    {
        players[num](pl);
        emit signal_update_current();
    }
    else
    {
        this->pl(pl);
        emit signal_update_all();
    }
}