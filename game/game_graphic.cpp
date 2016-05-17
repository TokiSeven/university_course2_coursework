#include "game_graphic.h"
#include <QDebug>
Game_graphic::Game_graphic(Container *cont, QObject *parent)//constructor, set size of window and it's name
    : QObject(parent)
{
    qDebug() << QString("Game_graphic-->> ") + "Created.";
    this->cont = cont;

    this->windowHeight = 280;
    this->windowWidth = 450;
    this->windowName = "Anime Fighting v 5.0";

    //connect(cont, SIGNAL(signal_update_all()), this, SLOT(slot_update()));
}

Game_graphic::~Game_graphic()//destructor
{
    //emit signal_closed();
    //    if (this)
    //        delete this;
}

void Game_graphic::initialization()
{
    view = new View(FloatRect(0, 0, 450, 280)); // камера

    lvl.LoadFromFile("files/map.tmx");

    bg.loadFromFile("files/images/bg.png");
    ichigo_t.loadFromFile("files/images/ichigo1.png");

    anim.loadFromXML("files/anim_Ichigo.xml",ichigo_t);

    anim.animList["jump"].loop = 0;
    anim.animList["cattack"].loop = 0;
    anim.animList["vattack"].loop = 0;
    anim.animList["cforwardattack"].loop = 0;
    anim.animList["vforwardattack"].loop = 0;
    anim.animList["cjumpattack"].loop = 0;
    anim.animList["vjumpattack"].loop = 0;
    anim.animList["spell"].loop = 0;

    anim2.loadFromXML("files/anim_getsuga.xml",ichigo_t);
    anim2.animList["explode"].loop = 0;

    background.setTexture(bg);
    background.setOrigin(bg.getSize().x/2,bg.getSize().y/2);

    Ichigo = new PLAYER(anim, lvl, 300, 100,"Player",0.0,0.0,100,"stay");

    music.openFromFile("files/bg.wav");
    music.play();
    music.setVolume(20);

    o = false;
    space = false;
}

void Game_graphic::updatePlayersAll()
{
    pl_all.clear();

    int size = cont->getPlayer_all().size();
    float width = cont->getPlayer_current().getWidth();
    float height = cont->getPlayer_current().getHeight();

    //for current player
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(width, height));
    rect.setFillColor(sf::Color(100, 100, 100));
    rect.setPosition(cont->getPlayer_current().getX(), cont->getPlayer_current().getY());
    pl_current = rect;
    //connect(cont->getPlayer_current(), SIGNAL(changed_position(Player*)), this, SLOT(slot_position(Player*)));

    //for all players
    for(int i = 0; i < size; i++)
    {
        rect.setPosition(cont->getPlayer_all()[i].getX(), cont->getPlayer_all()[i].getY());
        pl_all.append(rect);
    }
}

void Game_graphic::main_cycle()//started every cycle of game
{
    float time = clock.getElapsedTime().asMicroseconds(); // время выполнения за 1 такт процессора
    clock.restart();

    time = time / 500;  // здесь регулируем скорость игры

    if (time > 40) time = 40;

    // нажатие клавиш
    if (Keyboard::isKeyPressed(Keyboard::Left)) Ichigo->key["L"]=true;
    if (Keyboard::isKeyPressed(Keyboard::Right)) Ichigo->key["R"]=true;
    if (Keyboard::isKeyPressed(Keyboard::Up)) Ichigo->key["Up"]=true;
    if (Keyboard::isKeyPressed(Keyboard::Down)) Ichigo->key["Down"]=true;
    if (Keyboard::isKeyPressed(Keyboard::Space))
    {
        Ichigo->key["Space"]=true;
        if(Ichigo->anim.animList["spell"].currentFrame >= 6 && Ichigo->anim.animList["spell"].currentFrame < 6.1)
            if(!space) // анимация способности на 6 кадре 1 раз за 1 нажатие на кнопку
            {
                space = true;
                entities.push_back(new Spell(anim2,lvl,Ichigo->x+25,Ichigo->y-60,Ichigo->dir,"Spell", 0.3, 0.0, 10, "move")); // добавление способности в массив объектов
            }
    }
    if (!Keyboard::isKeyPressed(Keyboard::Space))
        space = false;
    if (Keyboard::isKeyPressed(Keyboard::C)) Ichigo->key["C"]=true;
    if (Keyboard::isKeyPressed(Keyboard::V)) Ichigo->key["V"]=true;
    if (Keyboard::isKeyPressed(Keyboard::B)) Ichigo->key["B"]=true;

    if(Keyboard::isKeyPressed(Keyboard::O))
        if(!o)
        {
            entities.push_back(new PLAYER(anim, lvl, 700, 500,"Player",0.0,0.0,100,"stay"));
            o=true;
        }

    for(it=entities.begin();it!=entities.end();) // есои объект мертв, то удаляем из массива
    {
        Entity *b = *it;
        b->update(time);
        if (b->life==false)
        {
            it  = entities.erase(it);
            delete b;
        }
        else it++;
    }


    Ichigo->update(time); // обновление игрока
    healthBar.update(Ichigo->Health); // обновление полоски здоровья

    for(it=entities.begin();it!=entities.end();it++) // взаимодействие объектов
    {
        // враги
        if ((*it)->name=="Player")
        {
            Entity *pl = *it;
            PLAYER *player = (PLAYER*)pl;

            if (player->Health<=0) continue;

            if  (Ichigo->getRect().intersects(player->getRect())) // если объекты пересекаются
            { // и у персонажа проигрывается анимация атаки
                if (Ichigo->anim.isPlaying() && Ichigo->anim.currentAnim != "stay" && Ichigo->anim.currentAnim != "jump" && Ichigo->anim.currentAnim != "spell" && Ichigo->anim.currentAnim != "walk" && Ichigo->anim.currentAnim != "hit")
                { // то раним врага
                    player->hit = true;
                    player->Health-=5;
                    if (player->dir)
                        player->x+=10;
                    else
                        player->x-=10;
                }
                else if (!Ichigo->hit)
                { // иначе раним игрока
                    Ichigo->Health-=5;
                    Ichigo->hit=true;
                    if (Ichigo->dir)
                        Ichigo->x+=10;
                    else
                        Ichigo->x-=10;
                }
            }

            // способности
            for (std::list<Entity*>::iterator it2=entities.begin(); it2!=entities.end(); it2++)
            {
                Entity *spell = *it2;
                if (spell->name=="Spell")
                    if (spell->Health>0)
                    {
                        if(spell->getRect().intersects(player->getRect())) // если способность взаимодействует с врагом
                        { // то пропадает и отнимает жизни
                            spell->Health=0;
                            player->Health-=5;
                        }
                        for (std::list<Entity*>::iterator it3=entities.begin(); it3!=entities.end(); it3++)
                        {
                            Entity *spell2 = *it3;
                            if(spell2->name=="Spell")
                                if(spell2->Health>0)
                                    if(spell->getRect() != spell2->getRect()) // если способность взаимодействует со способностью
                                        if(spell->getRect().intersects(spell2->getRect()))
                                        { // то они взаимоуничтожаются
                                            spell->Health=0;
                                            spell2->Health=0;
                                        }
                        }
                    }
            }
        }
    }


    // ///////////////////отображаем на экран/////////////////////
    view->setCenter(Ichigo->x,Ichigo->y-50); // установка камеры
    window->setView(*view);
    // отрисовка фона и карты
    background.setPosition(view->getCenter());
}

void Game_graphic::draw()//draw on the window all that we have
{
    window->draw(background);

    lvl.Draw(*window);

    // отрисовка объектов
    for(it=entities.begin();it!=entities.end();it++)
        (*it)->draw(*window);

    // отрисовка игрока и полоски здоровья
    Ichigo->draw(*window);
    healthBar.draw(*window);
}

void Game_graphic::events(sf::Event &event)//when something was done (for example, pressed key or mouse)
{
    if (event.type == sf::Event::Closed)
    {
        emit signal_game_closed();
        window->close();
    }
    //    if (event.type == sf::Event::KeyPressed)
    //    {
    //        Player_old pl(cont->getPlayer_current());
    //        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    //            pl.setX(pl.getX() - 1);
    //        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    //            pl.setX(pl.getX() + 1);
    //        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    //            pl.setY(pl.getY() - 1);
    //        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    //            pl.setY(pl.getY() + 1);
    //        cont->updatePlayer(pl);
    //    }
}

void Game_graphic::game_start()//main function of game
{
    //sf::RenderWindow window(sf::VideoMode(this->windowWidth, this->windowHeight), this->windowName.toStdString().c_str());
    this->window = new sf::RenderWindow(sf::VideoMode(this->windowWidth, this->windowHeight), this->windowName.toStdString().c_str());

    this->initialization();

    while (window->isOpen())
    {
        this->updatePlayersAll();

        sf::Event event;
        while (window->pollEvent(event))
        {
            events(event);
        }

        this->main_cycle();

        window->clear();
        this->draw();
        window->display();
    }
    emit signal_game_closed();
}

//================================================================================================
//==========================================SLOTS(BEGIN)==========================================
//================================================================================================
void Game_graphic::slot_position(Player_old player)
{
    int num = cont->getPlayer_all().indexOf(player);
    if (num == -1)//if didn't been finded in all players (it will be yourself)
        this->pl_current.setPosition(cont->getPlayer_current().getX(), cont->getPlayer_current().getY());
    else if (num < cont->getPlayer_all().size())
        this->pl_all[num].setPosition(cont->getPlayer_all()[num].getX(), cont->getPlayer_all()[num].getY());
}

void Game_graphic::slot_update()
{
    initialization();
}

void Game_graphic::slot_game_start()
{
    this->game_start();
}

void Game_graphic::slot_close()
{
    emit signal_game_closed();
    this->window->close();
}

//================================================================================================
//==========================================SLOTS(FINISH)==========================================
//================================================================================================
