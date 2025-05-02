#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>

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
};

Vec2 playerPos = {0.0f, 0.0f};
float playerSpeed = 0.0004f;
std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
bool keyStates[256] = { false };
int windowWidth = 800, windowHeight = 600;
int score = 0;
bool gameWon = false;
bool showEndMenu = false;

void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawFilledCircle(float cx, float cy, float r, int segments = 36) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
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

void drawEnemy(float x, float y) {
    glColor3f(0.2f, 0.2f, 0.8f);
    drawOval(x, y, 0.04f, 0.06f);
    glColor3f(0, 0, 0);
    drawFilledCircle(x - 0.015f, y + 0.015f, 0.008f);
    drawFilledCircle(x + 0.015f, y + 0.015f, 0.008f);
}

void drawBullet(float x, float y, bool fromEnemy) {
    glColor3f(fromEnemy ? 1.0f : 0.0f, 0.0f, fromEnemy ? 0.0f : 0.0f);
    drawFilledCircle(x, y, 0.01f);
}

void spawnEnemy() {
    Enemy e;
    e.pos.x = (rand() % 200 - 100) / 100.0f;
    e.pos.y = (rand() % 200 - 100) / 100.0f;
    e.dir.x = (rand() % 200 - 100) / 5000.0f;
    e.dir.y = (rand() % 200 - 100) / 5000.0f;
    e.speed = score >= 10 ? 0.006f : 0.003f;
    e.shootCooldown = 120;
    e.health = score >= 10 ? 2 : 1;
    e.alive = true;
    enemies.push_back(e);
}

void updatePlayer() {
    if (keyStates['w'] && playerPos.y < 0.95f) playerPos.y += playerSpeed;
    if (keyStates['s'] && playerPos.y > -0.95f) playerPos.y -= playerSpeed;
    if (keyStates['a'] && playerPos.x > -0.95f) playerPos.x -= playerSpeed;
    if (keyStates['d'] && playerPos.x < 0.95f) playerPos.x += playerSpeed;
}

void updateBullets() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        Bullet &b = bullets[i];
        if (!b.alive) continue;
        b.pos.x += b.dir.x * b.speed;
        b.pos.y += b.dir.y * b.speed;
        if (fabs(b.pos.x) > 1.2f || fabs(b.pos.y) > 1.2f) b.alive = false;
    }
}

void updateEnemies() {
    for (size_t i = 0; i < enemies.size(); ++i) {
        Enemy &e = enemies[i];
        if (!e.alive) continue;
        e.pos.x += e.dir.x * e.speed;
        e.pos.y += e.dir.y * e.speed * 10;

        if (e.pos.x < -1 || e.pos.x > 1) e.dir.x *= -1;
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
            b.speed = 0.009f;
            b.fromEnemy = true;
            bullets.push_back(b);
            e.shootCooldown = 5000;
        }
    }
}

void handleCollisions() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        Bullet &b = bullets[i];
        if (!b.alive) continue;

        if (b.fromEnemy) {
            float dx = b.pos.x - playerPos.x;
            float dy = b.pos.y - playerPos.y;
            if (dx * dx + dy * dy < 0.01f) {
                b.alive = false;
                // score--; // لم نعد نقلل النقاط
            }
        } else {
            for (size_t j = 0; j < enemies.size(); ++j) {
                Enemy &e = enemies[j];
                if (!e.alive) continue;
                float dx = b.pos.x - e.pos.x;
                float dy = b.pos.y - e.pos.y;
                if (dx * dx + dy * dy < 0.01f) {
                    b.alive = false;
                    e.health--;
                    if (e.health <= 0) {
                        e.alive = false;
                        score++;
                        if (score >= 10) {
                            gameWon = true;
                            showEndMenu = true;
                        } else {
                            spawnEnemy();
                        }
                    }
                }
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawToad(playerPos.x, playerPos.y);
    for (size_t i = 0; i < bullets.size(); ++i)
        if (bullets[i].alive) drawBullet(bullets[i].pos.x, bullets[i].pos.y, bullets[i].fromEnemy);
    for (size_t i = 0; i < enemies.size(); ++i)
        if (enemies[i].alive) drawEnemy(enemies[i].pos.x, enemies[i].pos.y);

    drawText(-0.95f, 0.9f, "Score: " + std::to_string(score));

    if (gameWon) {
        drawText(-0.15f, 0.2f, "YOU WIN!");
        if (showEndMenu) {
            drawText(-0.2f, 0.0f, "Press R to Restart");
            drawText(-0.2f, -0.1f, "Press Q to Quit");
        }
    }

    glutSwapBuffers();
}

void idle() {
    if (!gameWon) {
        updatePlayer();
        updateBullets();
        updateEnemies();
        handleCollisions();
        while (enemies.size() < 2) spawnEnemy();
    }
    glutPostRedisplay();
}

void keyDown(unsigned char key, int, int) {
    if (showEndMenu) {
        if (key == 'r' || key == 'R') {
            // إعادة التشغيل
            bullets.clear();
            enemies.clear();
            playerPos = {0.0f, 0.0f};
            score = 0;
            gameWon = false;
            showEndMenu = false;
            spawnEnemy();
        } else if (key == 'q' || key == 'Q') {
            exit(0);
        }
        return;
    }

    keyStates[key] = true;
}

void keyUp(unsigned char key, int, int) {
    keyStates[key] = false;
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameWon) {
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
    glutCreateWindow("Toad 2D Shooter - Smart Enemies");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(mouseClick);
    glutReshapeFunc(reshape);
    glClearColor(1, 1, 1, 1);
    glutMainLoop();
    return 0;
}
