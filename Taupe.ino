#include <Gamebuino-Meta.h>

//  Partie informations 
int partieTime = 60;

const int size = 9;

const int sizeCircle = 6;
const int sizePlayerCircle = sizeCircle + 2;

int circleCenterX = (gb.display.width()/2) - (sizeCircle / 2);
int circleCenterY = (gb.display.height()/2) - (sizeCircle / 2);


struct Point {
    uint posX;
    uint posY;
};

//Dimensions 80 x 64
Point tabHole[size] = {{circleCenterX - 20, circleCenterY - 20}, {circleCenterX, circleCenterY - 20}, {circleCenterX + 20, circleCenterY - 20},
                       {circleCenterX - 20, circleCenterY}, {circleCenterX, circleCenterY}, {circleCenterX + 20, circleCenterY},
                       {circleCenterX - 20, circleCenterY + 20}, {circleCenterX, circleCenterY + 20}, {circleCenterX + 20, circleCenterY + 20}};


class Taupe {
    private:
        bool _isGold;
        bool isOutB;
        int taupeSizeX = 2;
        int taupeSizeY = 2;
        ulong spawnTime;
        int time;
        int positionMole;
    public:
        Taupe(int _positionMole) {
            spawnTime = millis();
            //5% de chance d'être en or
            _isGold = (random(100) + 1) >= 80;
            //Durée de vie de la taupe sur le plateau
            time = random(3) + 1;
            positionMole = _positionMole;
            isOutB = true;
        };
        
        void update() {
            ulong now = millis();
            ulong deltaInSec = (now - spawnTime) / 1000;
            if(deltaInSec > time) {
                //Si le temps est dépassé alors on l'as fait despawn
                isOutB = false;
            }
        }

        void render() {
            //Partie graphique
            if(_isGold) {
                gb.display.setColor(YELLOW);
            } else {
                gb.display.setColor(BLUE);
            }
            Point holeCurrent = tabHole[positionMole];
            gb.display.fillCircle(holeCurrent.posX, holeCurrent.posY, 4);
        }

        bool isOut() {
            return isOutB;
        }

        int getScoreMole() {
            return _isGold ? 3 : 1;
        }
};

class Personnage
{
    private:
        // Personnage - Position
        int personnagePosition;

        // Personnage - Score
        int personnageScore = 0;

    public:
        Personnage() {
            personnagePosition = 5;

            personnageScore = 0;
        };

        void update() {
        // Affichage Graphique - Le Personnage
        //   gb.display.setColor(BLUE);
        //   gb.display.fillRect(personnagePositionX, personnagePositionY, personnageSizeX, personnageSizeY);

        }

        void render() {
            gb.display.setColor(RED);
            Point holeCurrent = tabHole[getPosition()];

            gb.display.println(personnageScore);

            gb.display.drawCircle((holeCurrent.posX + (sizeCircle / 2)) - (sizePlayerCircle / 2) + 1, (holeCurrent.posY + (sizeCircle / 2)) - (sizePlayerCircle / 2) + 1, sizePlayerCircle);
            
        }

        void move(Button action) {
            if(action == BUTTON_UP) {
                if(personnagePosition > 3)
                {
                    personnagePosition -= 3;
                }
            }
            if(action == BUTTON_DOWN) {
                if(personnagePosition < 7)
                {
                    personnagePosition += 3;
                }
            }
            if(action == BUTTON_LEFT) {
                if(personnagePosition > 1 && personnagePosition != 4 && personnagePosition != 7)
                {
                    personnagePosition -= 1;
                }
            }
            if(action == BUTTON_RIGHT) {
                if(personnagePosition < 9 && personnagePosition != 3 && personnagePosition != 6)
                {
                    personnagePosition += 1;
                }
            }
        }

        void hitMole(Taupe* taupeHit) {
            personnageScore += taupeHit->getScoreMole();
        }

        int getPosition() {
            return personnagePosition - 1;
        }

        int getScore() {
            return personnageScore;
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
                spawningTime = random(3) + 1;
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
                        taupeInGame[i] = nullptr;
                    }
                }
            }

            if (gb.buttons.pressed(BUTTON_UP)){ personnage.move(BUTTON_UP); }  
            if (gb.buttons.pressed(BUTTON_DOWN)) { personnage.move(BUTTON_DOWN); } 
            if (gb.buttons.pressed(BUTTON_LEFT)) { personnage.move(BUTTON_LEFT); } 
            if (gb.buttons.pressed(BUTTON_RIGHT)) { personnage.move(BUTTON_RIGHT); }
            if (gb.buttons.pressed(BUTTON_A)) {
                taupeCurrent = taupeInGame[personnage.getPosition()];
                if(taupeCurrent != nullptr) {
                    //Le joueur a bien tapé sur une taupe on l'a supprime
                    personnage.hitMole(taupeCurrent);
                    delete taupeCurrent;
                    taupeInGame[personnage.getPosition()] = nullptr;
                }
            }
        }

        //Faire le render après l'update
        void render() {
            for(Point hole : tabHole) {
                gb.display.setColor(GRAY);
                gb.display.drawCircle(hole.posX, hole.posY, sizeCircle);
            }

            Taupe* taupeCurrent;
            for(uint i = 0; i < size; i++) {
                taupeCurrent = taupeInGame[i];
                if(taupeCurrent != nullptr) {
                    //Il y a bien une taupe présente dans le trou
                    taupeCurrent->render();
                }
            }

            gb.display.setColor(YELLOW);
            
            personnage.render();
        }

        void spawnMole() {
            uint positionMole;
            bool isGood = false;

            do {
                positionMole = random(10);
                if(taupeInGame[positionMole] == nullptr) {
                    //aucune taupe a cet endroit
                    taupeInGame[positionMole] = new Taupe(positionMole);
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
  gameController.render();
}