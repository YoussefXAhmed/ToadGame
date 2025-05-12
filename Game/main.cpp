#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#ifdef _WIN32
#include <windows.h>
#define PLAY_HIT_SOUND() Beep(800, 100)
#define PLAY_KILL_SOUND() Beep(1000, 100)
#define PLAY_LEVEL_UP_SOUND() Beep(1200, 150)
#else
#define PLAY_HIT_SOUND()
#define PLAY_KILL_SOUND()
#define PLAY_LEVEL_UP_SOUND()
#endif



float winTextSize = 1.0f;
bool growing = true;
void drawTextEffect(float x, float y, const std::string& text, float r, float g, float b, float size = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size * 0.00001f, size * 0.00001f, 1);
    glColor3f(r, g, b);
    for (char c : text) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}
struct Vec2 {
    float x, y;
};

struct Bullet {
    Vec2 pos, dir;
    float speed = 0.012f;
    bool alive = true;
    bool fromEnemy = false;
};

struct Enemy {
    Vec2 pos, dir;
    float speed;
    float shootCooldown;
    int health;
    bool alive = true;
    bool isCat = false;
};

Vec2 playerPos = {0.0f, 0.0f};
float playerSpeed = 0.0039f;
std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
bool keyStates[256] = { false };
int windowWidth = 800, windowHeight = 600;
int score = 0;
bool gameWon = false;
bool showEndMenu = false;
bool gameOver = false;
bool awaitingNextLevel = false;
int currentLevel = 1;


void drawText(float x, float y, std::string text);
void applyLevelSettings();

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawFilledCircle(float cx, float cy, float r, int num_segments = 36) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawOval(float cx, float cy, float rx, float ry, int segments = 36) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawToad(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(0.05f, 0.05f, 1);

    glColor3f(1, 1, 1);
    drawOval(0, 0.5f, 1.0f, 1.2f);
    glColor3f(1, 0.1f, 0.1f);
    drawOval(0, 0.6f, 0.9f, 1.0f);
    glColor3f(1, 1, 1);
    drawFilledCircle(0.5f, 0.6f, 0.2f);
    drawFilledCircle(-0.4f, 0.7f, 0.25f);
    drawFilledCircle(0.0f, 0.9f, 0.2f);
    glColor3f(0.98f, 0.8f, 0.65f);
    drawOval(0, 0.0f, 0.8f, 0.7f);
    glColor3f(0, 0, 0);
    drawFilledCircle(-0.3f, 0.1f, 0.08f);
    drawFilledCircle(0.3f, 0.1f, 0.08f);

    glPopMatrix();
}

void drawPanda(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(0.2f, 0.2f, 1);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0.0f, 0.0f, 0.4f);
    drawFilledCircle(0.0f, 0.5f, 0.3f);

    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(-0.2f, 0.75f, 0.1f);
    drawFilledCircle(0.2f, 0.75f, 0.1f);
    drawFilledCircle(-0.1f, 0.5f, 0.08f);
    drawFilledCircle(0.1f, 0.5f, 0.08f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(-0.1f, 0.5f, 0.03f);
    drawFilledCircle(0.1f, 0.5f, 0.03f);

    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(0.0f, 0.4f, 0.05f);

    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.35f);
    glVertex2f(0.0f, 0.3f);
    glEnd();

    drawFilledCircle(-0.3f, 0.0f, 0.1f);
    drawFilledCircle(0.3f, 0.0f, 0.1f);
    drawFilledCircle(-0.2f, -0.3f, 0.1f);
    drawFilledCircle(0.2f, -0.3f, 0.1f);

    glPopMatrix();
}

void drawCat(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);  // Use the enemy's position
    float scale = 0.1f;         // Adjusted scale to match other characters
    glScalef(scale, scale, scale);

    glColor3f(1.0f, 0.8f, 0.6f);
    drawFilledCircle(0.0f, 0.0f, 0.4f, 100);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.3f, 0.3f);
    glVertex2f(-0.2f, 0.7f);
    glVertex2f(-0.1f, 0.3f);
    glVertex2f(0.1f, 0.3f);
    glVertex2f(0.2f, 0.7f);
    glVertex2f(0.3f, 0.3f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(-0.15f, 0.1f, 0.05f, 100);
    drawFilledCircle(0.15f, 0.1f, 0.05f, 100);

    glBegin(GL_LINES);
    glVertex2f(-0.2f, 0.2f); glVertex2f(-0.1f, 0.25f);
    glVertex2f(0.1f, 0.25f);  glVertex2f(0.2f, 0.2f);
    glEnd();

    glColor3f(1.0f, 0.5f, 0.5f);
    drawFilledCircle(0.0f, -0.05f, 0.03f, 100);

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.05f, -0.12f);
    glVertex2f(0.05f, -0.12f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-0.2f, 0.0f); glVertex2f(-0.4f, 0.05f);
    glVertex2f(-0.2f, -0.02f); glVertex2f(-0.4f, -0.02f);
    glVertex2f(-0.2f, -0.05f); glVertex2f(-0.4f, -0.1f);
    glVertex2f(0.2f, 0.0f); glVertex2f(0.4f, 0.05f);
    glVertex2f(0.2f, -0.02f); glVertex2f(0.4f, -0.02f);
    glVertex2f(0.2f, -0.05f); glVertex2f(0.4f, -0.1f);
    glEnd();

    glColor3f(1.0f, 0.8f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, -0.55f, 0.0f);
    glScalef(1.0f, 0.6f, 1.0f);
    drawFilledCircle(0.0f, 0.0f, 0.3f, 100);
    glPopMatrix();

    glColor3f(1.0f, 0.6f, 0.4f);
    glLineWidth(6.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.25f, -0.55f);
    glVertex2f(0.35f, -0.5f);
    glVertex2f(0.4f, -0.4f);
    glVertex2f(0.35f, -0.3f);
    glVertex2f(0.3f, -0.2f);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(1.0f, 0.8f, 0.6f);
    drawFilledCircle(-0.18f, -0.6f, 0.08f, 50);
    drawFilledCircle(0.18f, -0.6f, 0.08f, 50);

    glPopMatrix();
}

void drawBullet(float x, float y, bool fromEnemy) {
    glColor3f(fromEnemy ? 1.0f : 0.0f, 0.0f, fromEnemy ? 0.0f : 0.0f);
    drawFilledCircle(x, y, 0.03f);
}

void drawGradientBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.0588f, 0.2039f, 0.2627f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glColor3f(0.2039f, 0.9098f, 0.6196f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);

    glEnd();
}

//Control in 5 levels
void applyLevelSettings() {
    if (currentLevel == 1) {
        playerSpeed = 0.0007f;
    } else if (currentLevel == 2) {
        for (Enemy& e : enemies) e.health = 2;
        for (Enemy& e : enemies) e.speed *= 1.05f;
    } else if (currentLevel == 3) {
        for (Enemy& e : enemies) e.health = 2;
        for (Enemy& e : enemies) e.speed *= 1.1f;
    } else if (currentLevel == 4) {
        for (Enemy& e : enemies) e.health = 3;
        for (Enemy& e : enemies) e.speed *= 1.3f;
        playerSpeed *= 1.01f;
    } else if (currentLevel == 5) {
        for (Enemy& e : enemies) e.health = 5;
        for (Enemy& e : enemies) e.speed *= 1.5f;
        playerSpeed *= 1.01f;
    }
}


void spawnEnemy() {
    Enemy e;
    e.pos.x = (rand() % 200 - 100) / 100.0f;
    e.pos.y = (rand() % 200 - 100) / 100.0f;
    e.dir.x = (rand() % 200 - 100) / 5000.0f;
    e.dir.y = (rand() % 200 - 100) / 5000.0f;
    e.speed = 0.0003f;
    e.shootCooldown = 120;
    e.health = (currentLevel >= 5 ? 5 : currentLevel >= 4 ? 3 : currentLevel >= 2 ? 2 : 1);
    e.alive = true;
    e.isCat = (enemies.size() % 2 == 0);
    enemies.push_back(e);
}

void updatePlayer() {
    if (keyStates['w'] && playerPos.y < 0.95f) playerPos.y += playerSpeed;
    if (keyStates['s'] && playerPos.y > -0.95f) playerPos.y -= playerSpeed;
    if (keyStates['a'] && playerPos.x > -0.95f) playerPos.x -= playerSpeed;
    if (keyStates['d'] && playerPos.x < 0.95f) playerPos.x += playerSpeed;
}

void updateBullets() {
    for (Bullet &b : bullets) {
        if (!b.alive) continue;
        b.pos.x += b.dir.x * b.speed;
        b.pos.y += b.dir.y * b.speed;
        if (fabs(b.pos.x) > 1.2f || fabs(b.pos.y) > 1.2f) b.alive = false;
    }
}

//Players movement and bullet direction
void updateEnemies() {
    for (Enemy &e : enemies) {
        if (!e.alive) continue;
        e.pos.x += e.dir.x * e.speed * 20;
        e.pos.y += e.dir.y * e.speed * 20;
        if (e.pos.x < -1 || e.pos.x > 1) e.dir.x *= -1; //if char be in frame of screne
        if (e.pos.y < -1 || e.pos.y > 1) e.dir.y *= -1;
        e.shootCooldown--;
        if (e.shootCooldown <= 0) {
            Vec2 dir = { playerPos.x - e.pos.x, playerPos.y - e.pos.y };
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            dir.x /= len; dir.y /= len;
            Bullet b;
            b.pos = e.pos;
            b.dir = dir;
            b.alive = true;
            b.speed = (currentLevel >= 2) ? 0.0102f : 0.01f;
            b.fromEnemy = true;
            bullets.push_back(b);
            e.shootCooldown = 900;
        }
    }
}

//When you lose the game
void handlePlayerDamage() {
    int damage = currentLevel == 5 ? 5 : currentLevel == 4 ? 4 : currentLevel == 3 ? 3 : 1;
    score -= damage;
    PLAY_HIT_SOUND();
    if (score <= -10) {
        gameOver = true;
        showEndMenu = true;
    }
}

//Make enemy death
void handleEnemyDeath(Enemy& e) {
    e.alive = false;
    int points = (currentLevel == 5 ? 2 : currentLevel == 4 ? 2 : 1);
    score += points;
    PLAY_KILL_SOUND();
    if (score >= 100) {
        gameWon = true;
        showEndMenu = true;
        return;
    }
    spawnEnemy();
}

//Check requirements points for each level
void checkLevelProgression() {
    if (awaitingNextLevel || gameOver || gameWon) return;
    if ((currentLevel == 1 && score >= 5)  ||
        (currentLevel == 2 && score >= 10) ||
        (currentLevel == 3 && score >= 15) ||
        (currentLevel == 4 && score >= 20) ||
        (currentLevel == 5 && score >= 30)) {
        awaitingNextLevel = true;
        showEndMenu = true;
        PLAY_LEVEL_UP_SOUND();
    }}

//Calculate bullet if it reach the goal
void handleCollisions() {
    for (Bullet &b : bullets) {
        if (!b.alive) continue;

        if (b.fromEnemy) {
            float dx = b.pos.x - playerPos.x;
            float dy = b.pos.y - playerPos.y;
            if (dx * dx + dy * dy < 0.01f) {
                b.alive = false;
                handlePlayerDamage();
            }
        } else {
            for (Enemy &e : enemies) {
                if (!e.alive) continue;
                float dx = b.pos.x - e.pos.x;
                float dy = b.pos.y - e.pos.y;
                if (dx * dx + dy * dy < 0.01f) {
                    b.alive = false;
                    int dmg = (currentLevel == 5 ? 2 : 1);
                    e.health -= dmg;
                    if (e.health <= 0) {
                        handleEnemyDeath(e);
                        checkLevelProgression();
                    }
                }
            }
        }
    }
}

//Q for exit and N for next level
void drawLevelPrompt() {
    if (awaitingNextLevel) {
        drawText(-0.3f, 0.0f, "Level " + std::to_string(currentLevel) + " Complete!");
        drawText(-0.35f, -0.1f, "Press N to go to Level " + std::to_string(currentLevel + 1));
        drawText(-0.2f, -0.2f, "Press Q to Quit");
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGradientBackground();
    drawToad(playerPos.x, playerPos.y);
    for (Bullet &b : bullets) if (b.alive) drawBullet(b.pos.x, b.pos.y, b.fromEnemy);
    for (Enemy &e : enemies) if (e.alive) (e.isCat ? drawCat(e.pos.x, e.pos.y) : drawPanda(e.pos.x, e.pos.y));
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, 0.9f, "Score: " + std::to_string(score) + " Level: " + std::to_string(currentLevel));

    if (gameWon) {
        float goldR = 1.0f, goldG = 0.84f, goldB = 0.0f;
        drawTextEffect(-0.4f, 0.0f, "YOU WIN!", goldR, goldG, goldB, winTextSize);
    }
    if (gameOver) drawText(-0.15f, 0.2f, "YOU LOST!");
    drawLevelPrompt();
    if (showEndMenu) drawText(-0.2f, -0.3f, "Press R to Restart");
    glutSwapBuffers();
}

//Update states
void idle() {
    if (!gameWon && !gameOver && !awaitingNextLevel) {
        updatePlayer();
        updateBullets();
        updateEnemies();
        handleCollisions();
        while (enemies.size() < 2) spawnEnemy();
    }

    if (gameWon) {
        if (growing) {
            winTextSize += 0.5f;
            if (winTextSize >= 150.0f) growing = false;
        } else {
            winTextSize -= 0.5f;
            if (winTextSize <= 100.0f) growing = true;
        }
    }

    glutPostRedisplay();
}

//Respond to keyboard keys press
void keyDown(unsigned char key, int, int) {
    if (showEndMenu) {
        if (key == 'r' || key == 'R') {
            bullets.clear(); enemies.clear();
            playerPos = {0.0f, 0.0f}; score = 0; currentLevel = 1;
            gameWon = false; gameOver = false; showEndMenu = false;
            awaitingNextLevel = false;
            spawnEnemy();
        } else if (key == 'q' || key == 'Q') {
            exit(0);
        } else if ((key == 'n' || key == 'N') && awaitingNextLevel) {
            currentLevel++;
            awaitingNextLevel = false;
            showEndMenu = false;
            bullets.clear(); enemies.clear();
            playerPos = {0.0f, 0.0f};
            spawnEnemy(); spawnEnemy();
            applyLevelSettings();
        }
        return;
    }
    keyStates[key] = true;
}

//AWSD keys control
void keyUp(unsigned char key, int, int) {
    keyStates[key] = false;
}

//Mouse control
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameWon && !awaitingNextLevel) {
        float normX = (x / (float)windowWidth) * 2 - 1;
        float normY = -((y / (float)windowHeight) * 2 - 1);
        Vec2 dir = { normX - playerPos.x, normY - playerPos.y };
        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
        dir.x /= len; dir.y /= len;
        Bullet b;
        b.pos = playerPos;
        b.dir = dir;
        b.speed = 0.02f;
        b.alive = true;
        b.fromEnemy = false;
        bullets.push_back(b);
    }
}


void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Toad");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(mouseClick);
    glutReshapeFunc(reshape);
    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    spawnEnemy();
    spawnEnemy();
    glutMainLoop();
    return 0;
}
