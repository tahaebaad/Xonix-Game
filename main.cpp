// PF PROJECT- XONIX GAME
// SECTION:CS-2B
// 1. Syed M. Mesum Ali Naqvi (24i-0759)
// 2. Taha Ebaad (24i-0516)

// main code

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <time.h>
using namespace sf;
using namespace std;

const int M = 25;
const int N = 40;

int grid[M][N] = {0};
int ts = 18; //tile size

const int resolutionX = N*ts;
const int resolutionY = M*ts;

struct Enemy {
    int x, y, dx, dy;
    float speedcontrol;
    int movementType; // 0: linear, 1: zigzag, 2: circular
    float angle;      // For circular movement
    float patternTimer; // For zigzag movement
    float baseY;      
    float collisionCooldown; 

    Enemy(float speed = 1.0f) {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;

        if (dx == 0) dx = 1;
        if (dy == 0) dy = 1;
        
        speedcontrol = speed;
        movementType = 0; 
        angle = 0.0f;
        patternTimer = 0.0f;
        baseY = static_cast<float>(y); 
        collisionCooldown = 0.0f; 
    }

    // linear movement with collision detection
    void moveLinear() {
        if (collisionCooldown > 0) {
            collisionCooldown -= 0.016f; 
            return; // skip movement during cooldown
        }

        int newX = x + static_cast<int>(dx * speedcontrol);
        int newY = y + static_cast<int>(dy * speedcontrol);

        // update position
        x = newX;
        y = newY;

        // check collision , reverse direction
        int gridX = x / ts;
        int gridY = y / ts;
        if (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
            if (grid[gridY][gridX] == 1) {
                dx = -dx;
                dy = -dy;
                // avoid sticking
                while (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N && grid[gridY][gridX] == 1) {
                    x += static_cast<int>(dx * speedcontrol);
                    y += static_cast<int>(dy * speedcontrol);
                    gridX = x / ts;
                    gridY = y / ts;
                }
                collisionCooldown = 0.1f;
            }
        }
    }

    // Circular movement
    void moveCircular() {
        if (collisionCooldown > 0) {
            collisionCooldown -= 0.016f;
            return; // Skip movement during cooldown
        }

        angle += 0.05f * speedcontrol;
        float newX = x + cos(angle) * 3.0f * speedcontrol;
        float newY = y + sin(angle) * 3.0f * speedcontrol;
        
        if (newX > ts && newX < (N-1) * ts) x = static_cast<int>(newX);
        if (newY > ts && newY < (M-1) * ts) y = static_cast<int>(newY);
        
        int gridX = x / ts;
        int gridY = y / ts;
        if (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
            if (grid[gridY][gridX] == 1) {
                dx = -dx;
                dy = -dy;
                angle -= 0.05f * speedcontrol;
                // avoid sticking
                while (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N && grid[gridY][gridX] == 1) {
                    x += static_cast<int>(cos(angle) * 3.0f * speedcontrol);
                    y += static_cast<int>(sin(angle) * 3.0f * speedcontrol);
                    gridX = x / ts;
                    gridY = y / ts;
                }
                collisionCooldown = 0.1f;
            }
        }

        // bounce off screen edges
        if (x <= ts || x >= (N-1) * ts) dx = -dx;
        if (y <= ts || y >= (M-1) * ts) dy = -dy;
    }

    // Zigzag movement
    void moveZigzag() {
        if (collisionCooldown > 0) {
            collisionCooldown -= 0.016f; 
            return; 
        }
    
        // calculate new position
        patternTimer += 0.10f * speedcontrol;
        float newY = baseY + sin(patternTimer) * 30.0f;
        int newX = x + static_cast<int>(dx * speedcontrol);
    
        int gridX = newX / ts;
        int gridY = static_cast<int>(newY) / ts;
    
        if (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
            if (grid[gridY][gridX] == 1) {
                dx = -dx;
                patternTimer -= 0.10f * speedcontrol; 
                // avoid sticking
                newX = x + static_cast<int>(dx * speedcontrol);
                newY = baseY + sin(patternTimer) * 30.0f;
                gridX = newX / ts;
                gridY = static_cast<int>(newY) / ts;
                while (gridY >= 0 && gridY < M && gridX >= 0 && gridX < N && grid[gridY][gridX] == 1) {
                    newX += static_cast<int>(dx * speedcontrol);
                    newY = baseY + sin(patternTimer) * 30.0f;
                    gridX = newX / ts;
                    gridY = static_cast<int>(newY) / ts;
                }
                collisionCooldown = 0.1f;
            }
        }
    
        x = newX;
        y = static_cast<int>(newY);
    
        // Bounce off side walls
        if (x <= ts) {
            dx = -dx;     
            x = ts + 1;   
        } else if (x >= (N-1) * ts) {
            dx = -dx;     
            x = (N-1) * ts - 2; 
        }
    
        if (y < ts) y = ts;
        if (y >= (M-1) * ts) y = (M-1) * ts - 1;
    }

    // Main move function to delegate based on movement type
    void move(){
        if (movementType == 0) {
            moveLinear();
        } else if (movementType == 1) {
            moveZigzag();
        } else if (movementType == 2) {
            moveCircular();
        }

        if (x<ts) x=ts;
        if (x >= (N-1)*ts) x=(N-1) * ts - 1;
        if (y < ts) y=ts;
        if (y >= (M-1) * ts) y=(M-1) * ts - 1;
    }
};


//PROTOTYPES

bool menu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score);
bool scoreboard(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score);
 
bool selectlevel(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score);
bool playermodemenu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score);

bool GameoverMenu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText,int& levelType, int& p1score, int& p2score);
bool GameOver(RenderWindow& window);

void showTilesCount(RenderWindow& window, Font& font, int trailCount, int moveCount1, int ts, int resolutionX, int p1_score, int p1_powerups, bool p1_powerupActive);
void showBothTilesCount(RenderWindow& window, Font& font, int trailCount1, int trailCount2, int p1_alive, int p2_alive, int moveCount1, int moveCount2, int ts, int resolutionX, int p1_score, int p1_powerups, bool p1_powerupActive, int p2_score, int p2_powerups, bool p2_powerupActive);

void adjustBackground(RenderWindow& window, Sprite& backgroundSprite, Texture& backgroundTexture);

void positionMenuText(RenderWindow& window, Text& startText, Text& levelText, Text& scoreText, Text& exitText);
void positionlevelMenuText(RenderWindow& window, Text& title, Text& easy, Text& medium, Text& hard, Text& continuous,
Text& mainmenu);
void positionModeMenuText(RenderWindow& window, Text& title, Text& oneplayermode, Text& twoplayermode, Text& mainmenu);
void positionScoreboardText(RenderWindow& window, Text& title, Text& player1, Text& player2, Text& mainmenu);

void positionGameoverText(RenderWindow& window, Text& restart, Text& mainmenu, Text& exit);
void Score(int& p1_score, int& p2_score);
void drop(int y,int x);



//MAIN FUNCTION

int main(){

repeat:

    srand(time(0));

    RenderWindow window(VideoMode(resolutionX, resolutionY), "XONIX GAME!", Style::Close | Style::Resize);
    window.setFramerateLimit(60);
    	
    /////// LOAD IMAGES ///////
    Texture t1,t2,t3;
    t1.loadFromFile("tiles.png");
    t2.loadFromFile("gameover.png");
    t3.loadFromFile("enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100,100);
    sEnemy.setOrigin(20,20);

    float enemySpeed = 1.0f;
    int enemyCount = 2;
    Enemy a[20];

    bool Game=true;
    int x=0, y=0, dx=0, dy=0;       // player 1
    int x1=N-1, y1=M-1, dx1=0, dy1=0;   // player 2
    bool want_p2=false, p1_alive=true, p2_alive=false, p1_building=false, p2_building=false;
    float timer=0, delay=0.07; 
    Clock clock;
    
    int levelType = 0;
    int timecount = 0;
    int geocount = 0;
    int secs = 0;

    int trailCount = 0, trailCount1 = 0, trailCount2 = 0;
    int moveCount1=0, moveCount2=0;
    int p1_score=0, p1_powerups=0, p1_occurances=0;
    bool  p1_powerupActive=false, p1_1tile=false, p1_10tiles=false, p1_5tiles=false;
    int p2_score=0, p2_powerups=0, p2_occurances=0;  
    bool  p2_powerupActive=false, p2_1tile=false, p2_10tiles=false, p2_5tiles=false;

    bool p1_power50given=false, p2_power50given=false;
    int p1_power30plus=0, p2_power30plus=0;

    Clock stopClockp1, stopClockp2;
    Time stopDuration=milliseconds(3000);

    /////// INITIALIZE CORNER TILES BY 1 ///////
    for(int i=0; i<M; i++)
        for(int j=0; j<N; j++)
            grid[i][j] = (i==0 || j==0 || i==M-1 || j==N-1)?1:0;
        
        Font font;
    if(!font.loadFromFile("WinkyRough-MediumItalic.ttf")){
        cout<<"Error loading font!"<<endl;
        return false;
    }
    
            Clock gameClock;
            Text timeText;
            timeText.setFont(font);
            timeText.setCharacterSize(24);
            timeText.setFillColor(Color::White);
            timeText.setPosition(resolutionX/2-ts*3, 15);


            ////// BONUS: SOUND EFFECTS ///////
             SoundBuffer deathBuffer;
            Sound deathSound;
            if(!deathBuffer.loadFromFile("hit.wav")){
                cout<<"Failed to load death sound!"<<endl;
            }
            deathSound.setBuffer(deathBuffer);
            
            SoundBuffer tileBuffer;
            Sound tileSound;
            if(!tileBuffer.loadFromFile("click_x.wav")){
                cout<<"Failed to load tile sound!"<<endl;
            }
            tileSound.setBuffer(tileBuffer);

            SoundBuffer powerBuffer;
            Sound powerSound;
            if(!powerBuffer.loadFromFile("coin_flip.wav")){
                cout<<"Failed to load power sound!"<<endl;
            }
            powerSound.setBuffer(powerBuffer);

        /////// MENU ///////
        if(! menu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1_score, p2_score)){
            // If menu returns false, exit the game
            return 0;
        }

        gameClock.restart();

        if(want_p2){
            p2_alive=true;
        } else {
            p2_alive=false;
        }

        while (window.isOpen()){	

        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer+=time;

        secs = gameClock.getElapsedTime().asSeconds();
        timeText.setString("Time: " + to_string(secs));


       /////// ENEMY COUNT AND SPEED ///////
    if (secs > timecount + 20) {
        if (levelType == 4) {
            int oldCount = enemyCount;
            enemyCount += 2;
            if (enemyCount > 20) enemyCount = 20;
            for (int i = oldCount; i < enemyCount; i++){
                a[i] = Enemy(enemySpeed);
                if (secs > 62) {
                    a[i].movementType = 2; // Circular for new enemies after 62 seconds
                } else if (secs > 30) {
                    a[i].movementType = 1; // Zigzag for new enemies between 30-62 seconds
                }
            }
        }
        timecount += 20;
        enemySpeed++;
        // Update speed for all existing enemies without resetting movementType
        for (int i = 0; i < enemyCount; i++) {
            a[i].speedcontrol = enemySpeed;
        }
    }



if(secs > 30 && secs <= 62){ // kept second at 62 so that enemy spawns at 60th second and switches at 62nd second
    for(int i=0; i<enemyCount/2; i++){
        a[i].movementType=1; // Switch first half to zigzag
    }
}else if(secs > 62){
    for(int i=enemyCount/2; i<enemyCount; i++){
        a[i].movementType=2; // Switch second half to circular
    }
}


if(!p1_powerupActive && !p2_powerupActive){
    for (int i=0;i<enemyCount;i++) a[i].move();
}

    /////// RESET GAME IF RESTART ///////
        Event e;
        while (window.pollEvent(e)){
            if(e.type == Event::Closed)
                window.close();
               
            if(e.type == Event::KeyPressed){
             if(e.key.code==Keyboard::Escape){
                for (int i=1;i<M-1;i++)
                 for (int j=1;j<N-1;j++)
                   grid[i][j]=0;
                   
                if(levelType==0 || levelType==1 || levelType==4){
                    enemyCount=2;
                }else if(levelType==2){
                    enemyCount=4;
                }
                else if(levelType==3){
                    enemyCount=6;
                }
                x=0;y=0;
                x1=N-1; y1=M-1;

                dx=dy=dx1=dy1=0;

                trailCount=trailCount1=trailCount2=0;
                moveCount1=moveCount2=0;

                p1_alive=p2_alive=true;
                p1_building=p2_building=false;

                for (int i = 0; i < enemyCount; i++) {
                    a[i].x = 300;
                    a[i].y = 300;

                    a[i].dx = 4 - rand() % 8;
                    if (a[i].dx == 0) a[i].dx = 1;

                    a[i].dy = 4 - rand() % 8;
                    if (a[i].dy == 0) a[i].dy = 1;

                    a[i].speedcontrol = 1.0f;  
                    a[i].movementType = 0;
                    a[i].angle = 0.0f;
                    a[i].patternTimer = 0.0f;
                }

                gameClock.restart();
                timecount=geocount=0;
                enemySpeed=0;
                p1_score = p1_occurances =0;
                p1_powerupActive = p1_1tile = p1_10tiles = p1_5tiles =false;
                p2_score = p2_occurances =0;
                p2_powerupActive = p2_1tile = p2_10tiles = p2_5tiles =false;

                p1_power50given = p2_power50given = p1_power30plus = p2_power30plus =false;

                Game=true;
               }
            }
        }

        if(!p2_powerupActive && p1_alive){
            if (Keyboard::isKeyPressed(Keyboard::Left)) {dx=-1;dy=0; p1_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::Right)) {dx=1;dy=0; p1_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::Up)) {dx=0;dy=-1; p1_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::Down)) {dx=0;dy=1; p1_building=true;};
        }

        if(!p1_powerupActive && p2_alive){
            if (Keyboard::isKeyPressed(Keyboard::A)) {dx1=-1;dy1=0; p2_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::D)) {dx1=1;dy1=0; p2_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::W)) {dx1=0;dy1=-1; p2_building=true;};
            if (Keyboard::isKeyPressed(Keyboard::S)) {dx1=0;dy1=1; p2_building=true;}; 
        }

    if (!Game) continue;

    /////// MAKE TRAIL OF PLAYER ///////

    if (timer>delay){   
        if(!want_p2){
            // player 1
            x+=dx;
            y+=dy;

            if (x<0) x=0; if (x>N-1) x=N-1;
            if (y<0) y=0; if (y>M-1) y=M-1;

            if (grid[y][x]==2) Game=false;
            if (grid[y][x]==0){
                grid[y][x]=2;
            }

            timer=0;
        }
        if(want_p2){
            // player 1&2
            if(p1_alive){
                x+=dx;
                y+=dy;
                if (x<0) x=0; if (x>N-1) x=N-1;
                if (y<0) y=0; if (y>M-1) y=M-1;
            }
            if(p2_alive){
                x1+=dx1;
                y1+=dy1;
                if (x1<0) x1=0; if (x1>N-1) x1=N-1;
                if (y1<0) y1=0; if (y1>M-1) y1=M-1;
            }
                /////// COLLISION LOGIC ///////

                if(grid[y][x]==2 && grid[y1][x1]==3){
                    p1_alive=p2_alive=Game=false; 
                } else if(grid[y][x]==2 || grid[y][x]==3){
                    p1_alive=false;

                    for(int i=1; i<M-1; i++)
                        for(int j=1; j<N-1; j++)
                        if(grid[i][j] == 2) grid[i][j] = 0; 

                } else if(grid[y1][x1]==2 || grid[y1][x1]==3){
                    p2_alive=false;

                    for(int i=1; i<M-1; i++)
                        for(int j=1; j<N-1; j++)
                        if(grid[i][j] == 3) grid[i][j] = 0;  
                }


                if(x==x1 && y==y1) {
                    if( p1_building && p2_building ){
                        p1_alive=p2_alive=Game=false; 
                    } else if( p1_building && !p2_building ){
                        p1_alive=false;

                    for(int i=1; i<M-1; i++)
                        for(int j=1; j<N-1; j++)
                            if(grid[i][j] == 2) grid[i][j]=0; 
                    
                    } else if( !p1_building && p2_building ){
                        p2_alive=false;

                    for(int i=1; i<M-1; i++)
                        for(int j=1; j<N-1; j++)
                            if(grid[i][j] == 3) grid[i][j]=0; 
                    
                    }
                }

            if(p1_alive)
            if(grid[y][x]==0 && p1_building) grid[y][x]=2;

            if(p2_alive)
            if(grid[y1][x1]==0 && p2_building) grid[y1][x1]=3;

            timer=0;
        }

    }

    /////// COUNT TILES AND MOVES ///////

    if(!want_p2){
        if(grid[y][x] == 1){
            dx=0;
            dy=0;
        
                for(int i=0; i<enemyCount; i++)
                    drop(a[i].y/ts, a[i].x/ts);
        
                int newTiles = 0; // Track only newly filled tiles this step
                int newMoves = 0;

                for(int i=1; i<M-1; i++){
                    for(int j=1; j<N-1; j++){
                        if(grid[i][j] == -1)
                            grid[i][j] = 0; // not captured
                        else if(grid[i][j] == 0 || grid[i][j] == 2){
                            grid[i][j] = 1; 
                            newTiles++;    
                            newMoves++; 
                        }
                    }
                }
        
                trailCount += newTiles ; // accumulate total filled tiles
                if(newMoves>0){
                moveCount1++;
                tileSound.play();
                }

                if(newTiles>0) p1_1tile=true;
                if(newTiles>5) p1_5tiles=true;
                if(newTiles>10) p1_10tiles=true;

                        /////// SCORING //////
                        
            // player 1
            if(p1_5tiles || p1_10tiles){

                if(p1_occurances>=5 && p1_5tiles){
                    p1_score += newTiles*4;
                    p1_occurances++;
                }else if(p1_occurances>=3 && p1_5tiles){
                    p1_score += newTiles*2;
                    p1_occurances++;
                }else if(p1_occurances<3 && p1_10tiles){
                    p1_score += newTiles*2;
                    p1_occurances++;
                }else{
                    p1_score += newTiles;
                }
                p1_10tiles=p1_5tiles=false;
            }else if (p1_1tile){
            p1_score += newTiles;
            p1_1tile=false;
            }
        }
    } else if(want_p2){

        // for players 1
        if(grid[y][x] == 1 && p1_alive && p1_building){
            dx=0;
            dy=0;
            p1_building=false;
        
                for(int i=0; i<enemyCount; i++)
                    drop(a[i].y/ts, a[i].x/ts);
        
                int newTiles = 0; // Track only newly filled tiles this step
                int newMoves = 0;

                for(int i=1; i<M-1; i++){
                    for(int j=1; j<N-1; j++){
                        if(grid[i][j] == -1)
                            grid[i][j] = 0; // not captured
                        else if(grid[i][j] == 0 || grid[i][j] == 2){
                            grid[i][j] = 1; 
                            newTiles++;   
                            newMoves++;  
                        }
                    }
                }
        
                trailCount1 += newTiles ; // accumulate total filled tiles
                if(newMoves>0){
                moveCount1++;
                tileSound.play();
                }

                if(newTiles>0) p1_1tile=true;
                if(newTiles>5) p1_5tiles=true;
                if(newTiles>10) p1_10tiles=true;

                        /////// SCORING //////
                        
            // player 1
            if(p1_5tiles || p1_10tiles){

                if(p1_occurances>=5 && p1_5tiles){
                    p1_score += newTiles*4;
                    p1_occurances++;
                }else if(p1_occurances>=3 && p1_5tiles){
                    p1_score += newTiles*2;
                    p1_occurances++;
                }else if(p1_occurances<3 && p1_10tiles){
                    p1_score += newTiles*2;
                    p1_occurances++;
                }else{
                    p1_score += newTiles;
                }
                p1_10tiles=p1_5tiles=false;
            }else if(p1_1tile){
            p1_score += newTiles;
            p1_1tile=false;
            }

        }
                    

            // for player 2
            if(grid[y1][x1] == 1 && p2_alive && p2_building){
                dx1=0;
                dy1=0;
                p2_building=false;

                    for(int i=0; i<enemyCount; i++)
                        drop(a[i].y/ts, a[i].x/ts);
            
                    int newTiles = 0; // Track only newly filled tiles this step
                    int newMoves = 0;

                    for(int i=1; i<M-1; i++){
                        for(int j=1; j<N-1; j++){
                            if(grid[i][j] == -1)
                                grid[i][j] = 0; // not captured
                            else if(grid[i][j] == 0 || grid[i][j] == 3) {
                                grid[i][j] = 1; 
                                newTiles++;
                                newMoves++;      
                            }
                        }
                    }
            
                    trailCount2 += newTiles ; // accumulate total filled tiles
                    if(newMoves>0){
                    moveCount2++;
                    tileSound.play();
                    }

                    if(newTiles>0) p2_1tile=true;
                    if(newTiles>5) p2_5tiles=true;
                    if(newTiles>10) p2_10tiles=true;
    
                            /////// SCORING //////
                            
                // player 2
                if(p2_5tiles || p2_10tiles){
    
                    if(p2_occurances>=5 && p2_5tiles){
                        p2_score += newTiles*4;
                        p2_occurances++;
                    }else if(p2_occurances>=3 && p2_5tiles){
                        p2_score += newTiles*2;
                        p2_occurances++;
                    }else if(p2_occurances<3 && p2_10tiles){
                        p2_score += newTiles*2;
                        p2_occurances++;
                    }else{
                        p2_score += newTiles;
                    }
                    p2_10tiles=p2_5tiles=false;
                }else if (p2_1tile){
                p2_score += newTiles;
                p2_1tile=false;
                }

            }
        }

        for(int i=0;i<enemyCount;i++){
           if(grid[a[i].y/ts][a[i].x/ts]==2){
            p1_alive=false;
            Game=false;
            deathSound.play();
           }
           if(grid[a[i].y/ts][a[i].x/ts]==3){
            p2_alive=false;
            Game=false;
            deathSound.play();
            
           }
        }

      /////////  DRAW  //////////
    if(p1_powerupActive || p2_powerupActive){
        window.clear(Color::Magenta);
        powerSound.play();
    } else {
        window.clear(Color::Black);
    }

    // for drawing made or being made player1 or player2 tiles
    for (int i=0;i<M;i++)
        for (int j=0;j<N;j++)
         {
            if (grid[i][j]==0) continue;
            if (grid[i][j]==1) sTile.setTextureRect(IntRect( 0,0,ts,ts));
            if (grid[i][j]==2) sTile.setTextureRect(IntRect(36,0,ts,ts));
            if (grid[i][j]==3) sTile.setTextureRect(IntRect(72,0,ts,ts));
            sTile.setPosition(j*ts,i*ts);
            window.draw(sTile);
         }
        

      // draw player 1
      if(p1_alive){
        sTile.setTextureRect(IntRect(36,0,ts,ts));
        sTile.setPosition(x*ts,y*ts);
        window.draw(sTile);
      }

      // draw player 2
      if(p2_alive){
        sTile.setTextureRect(IntRect(72,0,ts,ts));
        sTile.setPosition(x1*ts,y1*ts);
        window.draw(sTile);
      }

      if(!p1_alive && (!p2_alive || !want_p2) ){
        Game=false;
        deathSound.play();
      }

//////// POWERUP ALLOCATION ////////
    if(p1_alive){
        if(p1_score>=50 && !p1_power50given){
            p1_powerups += 1;
            p1_power50given=true;
        }
        while(p1_score>= 70+p1_power30plus){
            p1_powerups++;
            p1_power30plus +=30;
        }
    }
    if(p2_alive){
        if(p2_score>=50 && !p2_power50given){
            p2_powerups += 1;
            p2_power50given=true;
        }
        while(p2_score>= 70+p2_power30plus){
            p2_powerups++;
            p2_power30plus += 30;
        }
    }

/////// POWERUP IN ACTION AND BONUS: BACKGROUND CHANGE DURING POWERUP ////////

    // for player 1
    if(!p1_powerupActive && Keyboard::isKeyPressed(Keyboard::M) && p1_powerups>0){
        p1_powerups--;
        p1_powerupActive=true;
        stopClockp1.restart();
        window.clear(Color::Magenta);
    }
    if(p1_powerupActive && stopClockp1.getElapsedTime()>=stopDuration){
        p1_powerupActive=false;
        window.clear(Color::Black);
    }

    // for player 2
    if(!p2_powerupActive && Keyboard::isKeyPressed(Keyboard::Q) && p2_powerups>0){
        p2_powerups--;
        p2_powerupActive=true;
        stopClockp2.restart();
        window.clear(Color::Magenta);
    }
    if(p2_powerupActive && stopClockp2.getElapsedTime()>=stopDuration){
        p2_powerupActive=false;
        window.clear(Color::Black);
    }

      /////// ROTATE AND DRAW ENEMY ///////
      sEnemy.rotate(10);
      for(int i=0;i<enemyCount;i++){
        sEnemy.setPosition(a[i].x,a[i].y);
        window.draw(sEnemy);
       }

       /////// SHOW TIMER ///////
       window.draw(timeText);

       /////// SHOW TILES AND MOVE COUNTS ///////
       if(!want_p2) showTilesCount(window, font, trailCount, moveCount1, ts, resolutionX, p1_score,p1_powerups, p1_powerupActive);
       if(want_p2) showBothTilesCount(window, font, trailCount1, trailCount2, p1_alive, p2_alive, moveCount1, moveCount2, ts, resolutionX, p1_score, p1_powerups, p1_powerupActive, p2_score, p2_powerups, p2_powerupActive);

// Gameover code

if(!Game){
Score(p1_score, p2_score);
     sleep(seconds(2));
     GameOver(window);
     sleep(seconds(3));
     if(!GameoverMenu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1_score, p2_score)){
            return 0;
     }else goto repeat;
     }     
     window.display();
      
     }
     
    return 0;
}

// main menu display
bool menu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score){

    Font font;
    if(!font.loadFromFile("WinkyRough-MediumItalic.ttf")){
    cout<<"Error loading font!"<<endl;
    return false;
    }

    // Sprite & Texture declaration
    Sprite backgroundSprite;
    Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile("background.jpeg")){
    cout<<"Error loading background texture!"<<endl;
    return false;
    }
    
    // Main Menu options
    Text startText("> START GAME", font, 40);
    Text levelText("> SELECT LEVEL", font, 40);
    Text scoreText("> SCOREBOARD", font, 40);
    Text exitText("> EXIT GAME", font, 40);
    
    backgroundSprite.setTexture(backgroundTexture);
    
    adjustBackground(window, backgroundSprite, backgroundTexture);
    positionMenuText(window, startText, levelText, scoreText, exitText);


while(window.isOpen()){

Event event;
    while(window.pollEvent(event)){

        if(event.type == Event::Closed){
            window.close();
            return false;
        }
            
        //to resize the screen
        if(event.type == Event::Resized){
            window.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
            
            adjustBackground(window, backgroundSprite, backgroundTexture);
            positionMenuText(window, startText, levelText, scoreText, exitText);
        
        }


        if(event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left){
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            if(startText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                return true; // START GAME clicked
                
            }else if(levelText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                bool level = selectlevel( window, resolutionX, want_p2, enemyCount, gameClock, timeText,
                levelType, p1score, p2score);
                if(level != false){
                return true;
                }
                
            }else if(scoreText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                bool score = scoreboard(window, resolutionX, want_p2, enemyCount, gameClock, timeText,
                levelType, p1score, p2score);
                if(score != false){
                return true;
                }
                
            }else if(exitText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                window.close();
            }
        }
    }

// Highlight hover
Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

startText.setFillColor(startText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

levelText.setFillColor(levelText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

scoreText.setFillColor(scoreText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

exitText.setFillColor(exitText.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

//Draw Elements on Screen
window.clear();
window.draw(backgroundSprite);
window.draw(startText);
window.draw(levelText);
window.draw(scoreText);
window.draw(exitText);
window.display();
}

return false;
}	

// easy, medium e.t.c. level selection display
bool selectlevel(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score) {
    Font font;
    if (!font.loadFromFile("WinkyRough-MediumItalic.ttf")) {
        cout << "Error loading font!" << endl;
        return false;
    }

    // Load background
    Sprite backgroundSprite;
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("levelbackground.jpeg")) {
        cout << "Error loading background texture!" << endl;
        return false;
    }
    
    backgroundSprite.setTexture(backgroundTexture);
    
    // Menu options
    Text title("CHOOSE ANY LEVEL:", font, 60);
    Text easy("< Easy >", font, 40);
    Text medium("< MEDIUM >", font, 40);
    Text hard("< HARD >", font, 40);
    Text continuous("< CONTINUOUS >", font, 40);
    Text mainmenu("< MAIN MENU >", font, 40);
    
    title.setFillColor(Color::Black);
   
    adjustBackground(window, backgroundSprite, backgroundTexture);
    positionlevelMenuText(window, title, easy, medium, hard, continuous, mainmenu);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return false;
            }
            
            // Handle resizing
            if (event.type == Event::Resized) {
                window.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
                adjustBackground(window, backgroundSprite, backgroundTexture);
                positionlevelMenuText(window, title, easy, medium, hard, continuous, mainmenu);
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                
                if (mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    menu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                    // Note: Returning false here keeps us in the menu system
                    return false;
                }
                else if (easy.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    levelType = 1;
                    enemyCount = 2;
                    bool mode = playermodemenu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                    if (mode) {
                        return true;
                    }
                }
                else if (medium.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    levelType = 2;
                    enemyCount = 4;
                    bool mode = playermodemenu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                    if (mode) {
                        return true;
                    }
                }
                else if (hard.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    levelType = 3;
                    enemyCount = 6;
                    bool mode = playermodemenu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                    if (mode) {
                        return true;
                    }
                }
                else if (continuous.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    levelType = 4;
                    enemyCount = 2;
                    bool mode = playermodemenu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                    if (mode) {
                        return true;
                    }
                }
            }
        }

        // Highlight hover
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        easy.setFillColor(easy.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? Color::Red : Color::Yellow);
        medium.setFillColor(medium.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? Color::Red : Color::Yellow);
        hard.setFillColor(hard.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? Color::Red : Color::Yellow);
        continuous.setFillColor(continuous.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? Color::Red : Color::Yellow);
        mainmenu.setFillColor(mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? Color::Red : Color::Yellow);

        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(easy);
        window.draw(medium);
        window.draw(hard);
        window.draw(continuous);
        window.draw(mainmenu);
        window.display();
    }
    return false;
}

// one or two player mode menu display
bool playermodemenu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1score, int& p2score){

    Font font;
    if(!font.loadFromFile("WinkyRough-MediumItalic.ttf")){
    cout<<"Error loading font!"<<endl;
    return false;
    }

    // Load background
    Sprite backgroundSprite;
    Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile("levelbackground.jpeg")){
    cout<<"Error loading background texture!"<<endl;
    return false;
    }
    
    // Mode Menu options
    Text title("CHOOSE PLAYER MODE:", font, 60);
    Text oneplayermode("> ONE PLAYER MODE", font, 40);
    Text twoplayermode("> TWO PLAYER MODE", font, 40);
    Text mainmenu("> MAIN MENU", font, 40);
    
    title.setFillColor(Color::Black);
    backgroundSprite.setTexture(backgroundTexture);

    adjustBackground(window, backgroundSprite, backgroundTexture);
    positionModeMenuText(window, title, oneplayermode, twoplayermode, mainmenu);

while(window.isOpen()){
    
Event event;
while(window.pollEvent(event)){

    if(event.type == Event::Closed){
    window.close();
    return false;
    }
    
    
            // If someone resize the screen (either full or short the screen)
    if (event.type == Event::Resized){
        window.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
        
            adjustBackground(window, backgroundSprite, backgroundTexture);
            positionModeMenuText(window, title, oneplayermode, twoplayermode, mainmenu);
            
    }

    if(event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left){
       Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        if(oneplayermode.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
            return true; // START GAME clicked
            
        }else if(twoplayermode.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
            want_p2=true;
            return true;
            
        }else if(mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
            menu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);            return true;
        }
    }
}

// Highlight hover
Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

oneplayermode.setFillColor(oneplayermode.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

twoplayermode.setFillColor(twoplayermode.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);

mainmenu.setFillColor(mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
Color::Red : Color::Yellow);


window.clear();
window.draw(backgroundSprite);
window.draw(title);
window.draw(oneplayermode);
window.draw(twoplayermode);
window.draw(mainmenu);
window.display();
}

return false;
}



bool scoreboard(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock, Text& timeText, int& levelType, int& p1_score, int& p2_score) {
    const int MAX = 100;
    int p1_scores[MAX], p2_scores[MAX];
    int count = 0;

    Font font;
    if (!font.loadFromFile("WinkyRough-MediumItalic.ttf")) {
        cout << "Error loading font!" << endl;
        return false;
    }

    Sprite backgroundSprite;
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("scoreboard.jpeg")) {
        cout << "Error loading background texture!" << endl;
        return false;
    }

    Text title("SCOREBOARD:", font, 60);
    title.setFillColor(Color::Blue);
    Text p1("P1 HIGH SCORES", font, 30);
    p1.setFillColor(Color::Red);
    Text p2("P2 HIGH SCORES", font, 30);
    p2.setFillColor(Color::Yellow);

    
    Text p1ScoreText[5], p2ScoreText[5];
	for (int i = 0; i < 5; i++) {
	    p1ScoreText[i].setFont(font);
	    p1ScoreText[i].setCharacterSize(30);
	    p1ScoreText[i].setFillColor(Color::White);
	    p1ScoreText[i].setPosition(240, 180 + i * 30);  // Left column

	    p2ScoreText[i].setFont(font);
	    p2ScoreText[i].setCharacterSize(30);
	    p2ScoreText[i].setFillColor(Color::White);
	    p2ScoreText[i].setPosition(450, 180 + i * 30);  // Right column (adjust X as needed)
	}

    Text mainmenu("> MAIN MENU", font, 40);
    mainmenu.setFillColor(Color::Yellow);

    backgroundSprite.setTexture(backgroundTexture);
    adjustBackground(window, backgroundSprite, backgroundTexture);
    positionScoreboardText(window, title, mainmenu, p1, p2);

    // Read scores from file
    ifstream file("score.txt");
    if (file.is_open()) {
        while (file >> p1_scores[count] >> p2_scores[count]) {
            count++;
            if (count >= MAX) break;
        }
        file.close();
    } else {
        cout << "Unable to open file." << endl;
        return false;
    }

    // Sort P1 scores
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (p1_scores[j] < p1_scores[j + 1]) {
                swap(p1_scores[j], p1_scores[j + 1]);
            }
        }
    }

    // Sort P2 scores
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (p2_scores[j] < p2_scores[j + 1]) {
                swap(p2_scores[j], p2_scores[j + 1]);
            }
        }
    }

	    for (int i = 0; i < 5 && i < count; i++) {
	    p1ScoreText[i].setString(to_string(p1_scores[i]));
	    p2ScoreText[i].setString(to_string(p2_scores[i]));
	    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                if (mainmenu.getGlobalBounds().contains(mousePos)) {
                    menu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1_score, p2_score);
                    return true;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        mainmenu.setFillColor(mainmenu.getGlobalBounds().contains(mousePos) ? Color::Red : Color::Yellow);

        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(p1);
        window.draw(p2);
        for (int i = 0; i < 5 && i < count; i++) {
	    window.draw(p1ScoreText[i]);
	    window.draw(p2ScoreText[i]);
	}
        window.draw(mainmenu);
        window.display();
    }

    return false;
}

//to mark empty spaces around enemy as unreachable
void drop(int y,int x){

    if(y<0 || y>=M || x<0 || x>=N) return;

    if (grid[y][x]==0) grid[y][x]=-1;
    if (grid[y-1][x]==0) drop(y-1,x);
    if (grid[y+1][x]==0) drop(y+1,x);
    if (grid[y][x-1]==0) drop(y,x-1);
    if (grid[y][x+1]==0) drop(y,x+1);
}		


// to show tiles count
void showTilesCount(RenderWindow& window, Font& font, int trailCount, int moveCount1, int ts, int resolutionX, int p1_score, int p1_powerups, bool p1_powerupActive){
    
    Text tileCountText;
    tileCountText.setFont(font);
    tileCountText.setCharacterSize(20);
    tileCountText.setFillColor(Color::White);
    tileCountText.setString("Player 1\nCount: "+to_string(trailCount)+ "\nMoves: "+to_string(moveCount1) +"\nPoints: "+to_string(p1_score) + "\nPowerUp Active: "+to_string(p1_powerupActive) + "\nPowerUp Left: "+to_string(p1_powerups) );

    int offset =2*ts; // distance from the top-right edge
    tileCountText.setPosition(offset,offset);

    window.draw(tileCountText);
}


// show both tiles count
void showBothTilesCount(RenderWindow& window, Font& font, int trailCount1, int trailCount2, int p1_alive, int p2_alive, int moveCount1, int moveCount2, int ts, int resolutionX, int p1_score, int p1_powerups, bool p1_powerupActive, int p2_score, int p2_powerups, bool p2_powerupActive){
    
    int offset =2*ts; // distance from the top-right edge

    // for player 1
    Text tileCountText1;
    tileCountText1.setFont(font);
    tileCountText1.setCharacterSize(20);
    tileCountText1.setFillColor(Color::Red);

    if(p1_alive) tileCountText1.setString("Player 1\nCount: " + to_string(trailCount1)+ "\nMoves: " + to_string(moveCount1) + "\nPoints: " + to_string(p1_score) + "\nPowerUp Active: " + to_string(p1_powerupActive) + "\nPowerUp Left: " + to_string(p1_powerups) );
    else  tileCountText1.setString("!Player 1\nCount: " + to_string(trailCount1)+ "\nMoves: " + to_string(moveCount1)  + "\nPoints: " + to_string(p1_score) + "\nPowerUp Active: " + to_string(p1_powerupActive) + "\nPowerUp Left: " + to_string(p1_powerups) );

    tileCountText1.setPosition(offset,offset);
    window.draw(tileCountText1);
   

    // for player 2
    Text tileCountText2;
    tileCountText2.setFont(font);
    tileCountText2.setCharacterSize(20);
    tileCountText2.setFillColor(Color::Yellow);

    if(p2_alive) tileCountText2.setString("Player 2\nCount: " + to_string(trailCount2)+ "\nMoves: " + to_string(moveCount2)  + "\nPoints: " + to_string(p2_score) + "\nPowerUp Active: " + to_string(p2_powerupActive) + "\nPowerUp Left: " + to_string(p2_powerups) );
    else tileCountText2.setString("!Player 2\nCount: " + to_string(trailCount2)+ "\nMoves: " + to_string(moveCount2)  + "\nPoints: " + to_string(p2_score) + "\nPowerUp Active: " + to_string(p2_powerupActive) + "\nPowerUp Left: " + to_string(p2_powerups) );

    tileCountText2.setPosition(resolutionX - tileCountText2.getGlobalBounds().width - offset, offset);
    window.draw(tileCountText2);
}

// Adjust the Background image position before & after resizing the screen
void adjustBackground(RenderWindow& window, Sprite& backgroundSprite, Texture& backgroundTexture){
    float winW = window.getSize().x;
    float winH = window.getSize().y;
    float texW = backgroundTexture.getSize().x;
    float texH = backgroundTexture.getSize().y;

    float scaleX = winW / texW;
    float scaleY = winH / texH;
    float scale = std::max(scaleX, scaleY); // Fill screen

    backgroundSprite.setScale(scale, scale);
    backgroundSprite.setPosition((winW - texW * scale) / 2, (winH - texH * scale) / 2);
}


// Adjust the Main Menu text position before & after resizing the screen
void positionMenuText(RenderWindow& window, Text& startText, Text& levelText, Text& scoreText, Text& exitText){

    Vector2u winSize = window.getSize();

    startText.setPosition(winSize.x * 0.5f - startText.getGlobalBounds().width / 2, winSize.y * 0.35f);
    levelText.setPosition(winSize.x * 0.5f - levelText.getGlobalBounds().width / 2, winSize.y * 0.45f);
    scoreText.setPosition(winSize.x * 0.5f - scoreText.getGlobalBounds().width / 2, winSize.y * 0.55f);
    exitText.setPosition(winSize.x * 0.5f - exitText.getGlobalBounds().width / 2, winSize.y * 0.65f);
}

// Adjust the Level Menu text position before & after resizing the screen
void positionlevelMenuText(RenderWindow& window, Text& title, Text& easy, Text& medium, Text& hard,
Text& continuous, Text& mainmenu){

    Vector2u winSize = window.getSize();

    title.setPosition(winSize.x * 0.5f - title.getGlobalBounds().width / 2, winSize.y * 0.15f);
    easy.setPosition(winSize.x * 0.5f - easy.getGlobalBounds().width / 2, winSize.y * 0.35f);
    medium.setPosition(winSize.x * 0.5f - medium.getGlobalBounds().width / 2, winSize.y * 0.45f);
    hard.setPosition(winSize.x * 0.5f - hard.getGlobalBounds().width / 2, winSize.y * 0.55f);
    continuous.setPosition(winSize.x * 0.5f - continuous.getGlobalBounds().width / 2, winSize.y * 0.65f);
    mainmenu.setPosition(winSize.x * 0.5f - mainmenu.getGlobalBounds().width / 2, winSize.y * 0.75f);
}


// Adjust the Player Mode Menu text position before & after resizing the screen
void positionModeMenuText(RenderWindow& window, Text& title, Text& oneplayermode, Text& twoplayermode,
Text& mainmenu){

    Vector2u winSize = window.getSize();

    title.setPosition(winSize.x * 0.5f - title.getGlobalBounds().width / 2, winSize.y * 0.15f);
    oneplayermode.setPosition(winSize.x * 0.5f - oneplayermode.getGlobalBounds().width / 2, winSize.y * 0.35f);
    twoplayermode.setPosition(winSize.x * 0.5f - twoplayermode.getGlobalBounds().width / 2, winSize.y * 0.45f);
    mainmenu.setPosition(winSize.x * 0.5f - mainmenu.getGlobalBounds().width / 2, winSize.y * 0.55f);
    
}


// Adjust the Scoreboard Menu text position before & after resizing the screen
void positionScoreboardText(RenderWindow& window, Text& title, Text& mainmenu, Text& p1, Text& p2){

    Vector2u winSize = window.getSize();

    title.setPosition(winSize.x * 0.5f - title.getGlobalBounds().width / 3, winSize.y * 0.15f);
    p1.setPosition(winSize.x * 0.4f - title.getGlobalBounds().width / 2, winSize.y * 0.35f);
    p2.setPosition(winSize.x * 0.7f - title.getGlobalBounds().width / 4, winSize.y * 0.35f);
    mainmenu.setPosition(winSize.x * 0.5f - mainmenu.getGlobalBounds().width / 2, winSize.y * 0.75f);
}

//GameOver screen
bool GameOver(RenderWindow& window){


    Texture tGameOver;
    tGameOver.loadFromFile("gameover.png");
    Sprite sGameOver(tGameOver);
    sGameOver.setPosition(110,120);
    
    
    window.clear();
    window.draw(sGameOver);
    window.display();
    
    return true;
    
    }
    
void Score(int& p1_score, int& p2_score){
ofstream file("score.txt", ios :: app); //append mode
if(file.is_open()){
file << p1_score << "\t\t" << p2_score <<endl;
file.close();
}else{
cout<<"Unable to open the score file."<<endl;
}

}
    
    
    
    //Gameover menu
    
    bool GameoverMenu(RenderWindow& window, const int& resolutionX, bool& want_p2, int& enemyCount, Clock& gameClock,
    Text& timeText,int& levelType, int& p1score, int& p2score){
    
        Font font;
        if(!font.loadFromFile("WinkyRough-MediumItalic.ttf")){
        cout<<"Error loading font!"<<endl;
        return false;
        }
    
        // Sprite & Texture declaration
        Sprite backgroundSprite;
        Texture backgroundTexture;
        if(!backgroundTexture.loadFromFile("gameovermenu.jpeg")){
        cout<<"Error loading background texture!"<<endl;
        return false;
        }
        
        Text player1;
        Text player2;
        player1.setFont(font);
        player1.setCharacterSize(40);
        player1.setFillColor(Color::White);
        player1.setPosition(150,150);
        player2.setFont(font);
        player2.setCharacterSize(40);
        player2.setFillColor(Color::White);
        player2.setPosition(200,200);
        Text restart("> RESTART", font, 40);
        Text mainmenu("> MAIN MENU", font, 40);
        Text exit("> EXIT GAME", font, 40);
        
        backgroundSprite.setTexture(backgroundTexture);
        
        adjustBackground(window, backgroundSprite, backgroundTexture);
        positionGameoverText(window, restart, mainmenu, exit);
       
    while(window.isOpen()){
    
    Event event;
        while(window.pollEvent(event)){
    
            if(event.type == Event::Closed){
                window.close();
                return false;
            }
                
            //to resize the screen
            if(event.type == Event::Resized){
                window.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
                
                    ifstream file("score.txt"); //append mode
            if(file.is_open()){
            file >> p1score >> p2score;
            file.close();
            }else{
            cout<<"Unable to open the score file."<<endl;
            
            }
                
                adjustBackground(window, backgroundSprite, backgroundTexture);
                positionGameoverText(window, restart, mainmenu, exit);
            
            }
    
    
            if(event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left){
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                
                    ifstream file("score.txt"); //append mode
            if(file.is_open()){
            file >> p1score >> p2score;
            file.close();
            }else{
            cout<<"Unable to open the score file."<<endl;
            
            }
    
                if(restart.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                    return true; // START GAME clicked
                    
                }else if(mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                  menu(window, resolutionX, want_p2, enemyCount, gameClock, timeText, levelType, p1score, p2score);
                  return true;
                    
                    
                    }else if(exit.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)){
                    window.close();
                }
            }
        }
    
    // Highlight hover
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    
    restart.setFillColor(restart.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
    Color::Red : Color::Yellow);
    
    mainmenu.setFillColor(mainmenu.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
    Color::Red : Color::Yellow);
    
    exit.setFillColor(exit.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y) ? 
    Color::Red : Color::Yellow);
    
    player1.setString("< PLAYER 1 SCORE = " + to_string(p1score));
    player2.setString("< PLAYER 2 SCORE = " + to_string(p2score));
    
    
    //Draw Elements on Screen
    window.clear();
    window.draw(backgroundSprite);
    window.draw(player1);
    window.draw(player2);
    window.draw(restart);
    window.draw(mainmenu);
    window.draw(exit);
    window.display();
    }
    
    return false;
    }	
    
    
    void positionGameoverText(RenderWindow& window, Text& restart, Text& mainmenu, Text& exit){
    
        Vector2u winSize = window.getSize();
    
        restart.setPosition(winSize.x * 0.4f - restart.getGlobalBounds().width / 3, winSize.y * 0.55f);
        mainmenu.setPosition(winSize.x * 0.4f - mainmenu.getGlobalBounds().width / 2, winSize.y * 0.65f);
        exit.setPosition(winSize.x * 0.4f - exit.getGlobalBounds().width / 2, winSize.y * 0.75f);
    }
    
    
    
    
