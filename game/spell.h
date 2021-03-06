#ifndef BULLET_H
#define BULLET_H

#include "entity.h"


class Spell
        : public Entity // класс специальной способности
{
public:

    SoundBuffer SPbuf, EXbuf;
    Sound SP, EX;

    Spell(AnimationManager &a, Level &lev, int x, int y, bool DIR, std::string name, float DX, float dy, int health, std::string first_anim, float W, float H)
        : Entity(a,x + ((!DIR) ? 70 : -70),y,name,DX,dy,health,first_anim,DIR,W,H)
    {
        this->type = "Spell";
        if (dir)
        {
            dx=-DX;
            anim.flip();
        }
        obj = lev.GetObjects("solid"); // массив объектов solid
        SPbuf.loadFromFile("files/spell.wav");
        SP.setBuffer(SPbuf);
        SP.play();
        EXbuf.loadFromFile("files/explode.wav");
        EX.setBuffer(EXbuf);
    }

    void update(float time) // функция обновления
    {
        x+=dx*time; // вычисление координаты x

        for (int i=0;i<obj.size();i++)
            if (getRect().intersects(obj[i].rect)) // если наш спрайт пересекается с любым solid
                Health=0; // то теряет все жизни

        if (Health<=0) // если теряет все жизни
        {
            anim.set("explode"); // то взрывается
            dx=0;
            if (anim.isPlaying()==false) // по окончании анимации
            {
                EX.play();
                life=false; // удаляется из массива объектов
            }
        }
        anim.tick(time); // функция анимации
    }

};

#endif // BULLET_H
