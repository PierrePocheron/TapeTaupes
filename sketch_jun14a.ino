#include <Gamebuino-Meta.h>

//  Partie informations 
int partieTime = 60;

const int size = 9;

class Personnage
{
    private:
        // Personnage - Taille
        int personnageSizeX = 2;
        int personnageSizeY = 6;

        // Personnage - Vitesse
        int personnageSpeedX = 2;
        int personnageSpeedY = 2;

        // Personnage - Vitesse
        int personnagePositionX = gb.display.width()/2;
        int personnagePositionY = gb.display.height()/2;

        // Personnage - Déplacement max
        int personnageMaxPositionXDroite = gb.display.width() - personnageSizeX;
        int personnageMaxPositionXGauche = 0;
        int personnageMaxPositionYHaut = 0;
        int personnageMaxPositionYBas = gb.display.height() - personnageSizeY;

        // Personnage - Score
        int personnageScore = 0;

    public:
        Personnage() {

        };

        void update() {
          // Utilisateur - onClic - Button Left
          if (gb.buttons.pressed(BUTTON_LEFT)) 
          {
            if(personnagePositionX > personnageMaxPositionXGauche)
            {
              personnagePositionX -= personnageSpeedX;
            }
          }

          // Utilisateur - onClic - Button Right
          if (gb.buttons.pressed(BUTTON_RIGHT)) 
          {
            if(personnagePositionX < personnageMaxPositionXDroite)
            {
              personnagePositionX += personnageSpeedX;
            }
          }

          // Utilisateur - onClic - Button Up
          if (gb.buttons.pressed(BUTTON_UP)) 
          {
            if(personnagePositionY > personnageMaxPositionYHaut)
            {
              personnagePositionY -= personnageSpeedY;
            }
          }

          // Utilisateur - onClic - Button Down
          if (gb.buttons.pressed(BUTTON_DOWN)) 
          {
            if(personnagePositionY < personnageMaxPositionYBas)
            {
              personnagePositionY += personnageSpeedY;
            }
          }

          // Affichage Graphique - Le Personnage
          gb.display.setColor(BLUE);
          gb.display.fillRect(personnagePositionX, personnagePositionY, personnageSizeX, personnageSizeY);

        }
};


class Taupe {
    private:
        bool isGold;
        bool isOutB;
        int taupeSizeX = 2;
        int taupeSizeY = 2;
        ulong spawnTime;
        int time;
    public:
        Taupe() {
            spawnTime = millis();
            //5% de chance d'être en or
            isGold = (random(100) + 1) >= 95;
            //Durée de vie de la taupe sur le plateau
            time = 2;
        };
        
        void update() {
            ulong now = millis();
            ulong deltaInSec = (now - spawnTime) / 1000;
            if(deltaInSec > time) {
                //Si le temps est dépassé alors on l'as fait despawn
            }
        }

        bool isOut() {
            return isOutB;
        }
};

class Controller {
    private:
        Taupe* taupeInGame[size];
        Personnage personnage;
        ulong tickTime;
        int spawningTime = 2;
    public:
        Controller() {
            tickTime = millis();
        };
        
        void update() {
            ulong now = millis();
            ulong deltaInSec = (now - tickTime) / 1000;

            if(deltaInSec > spawningTime) {
                tickTime = millis();
                spawnMole();
            }
            
            Taupe* taupeCurrent;
            //Ici on met a jour toutes les taupes, a savoir dans quel ordre on update perso -> taupe 
            for(uint i = 0; i < size; i++) {
                taupeCurrent = taupeInGame[i];
                if(taupeCurrent != nullptr) {
                    //Il y a bien une taupe présente dans le trou
                    taupeCurrent->update();
                    if(!taupeCurrent->isOut()) {
                        //La taupe est rentrée alors on l'a supprime
                        delete taupeCurrent;
                    }
                }
            } 
        }

        void spawnMole() {
            uint positionMole;
            bool isGood = false;

            do {
                positionMole = random(10);
                if(taupeInGame[positionMole] == nullptr) {
                    //aucune taupe a cet endroit
                    taupeInGame[positionMole] = new Taupe();
                    isGood = true;
                }
            } while(!isGood);
        }
};

Controller gameController;



//  Func Setup
void setup() {
  gb.begin();

}

//  Func Loop
void loop() {
  while (!gb.update());
  gb.display.clear();

  gameController.update();
}
