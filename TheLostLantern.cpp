// The Lost Lantern: A Light Journey
// CS2206 - Computer Graphics Project
// Project Type: Story Game
// Controls:
//   N / n        : Next scene
//   B / b        : Previous scene
//   Arrow keys   : Move the character
//   Space        : Collect light fragment / activate scene action
//   R / r        : Reset current scene
//   Left Mouse   : Activate lantern light
//   Q / ESC      : Quit

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// ------------------------------------------------------------
// Global Constants and Variables
// ------------------------------------------------------------
const float PI = 3.14159265359f;

int winWidth = 900;
int winHeight = 650;

int currentScene = 0;
const int TOTAL_SCENES = 6;

float heroX = -0.70f;
float heroY = -0.55f;
float cloudOffset = 0.0f;
float starRotation = 0.0f;
float lightScale = 1.0f;
float bridgeSwing = 0.0f;
float towerGlow = 0.0f;

bool animationOn = true;
bool fragmentCollected[3] = { false, false, false };
bool lanternActivated = false;

GLuint grassTexture = 0;
GLuint stoneTexture = 0;

// ------------------------------------------------------------
// Utility Functions
// ------------------------------------------------------------
void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

void drawText(float x, float y, const string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(font, text[i]);
    }
}

void drawLargeText(float x, float y, const string& text) {
    drawText(x, y, text, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawFilledCircle(float cx, float cy, float radius, int segments = 60) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}

void drawHollowCircle(float cx, float cy, float radius, int segments = 80) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}

void drawRectangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2, float width = 2.0f) {
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.0f);
}

void drawStar(float cx, float cy, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 10; ++i) {
        float angle = PI / 2.0f + i * PI / 5.0f;
        float r = (i % 2 == 0) ? radius : radius * 0.42f;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }
    glEnd();
}

void drawMoon(float x, float y, float r) {
    setColor(1.0f, 0.92f, 0.55f);
    drawFilledCircle(x, y, r);
    setColor(0.05f, 0.07f, 0.16f);
    drawFilledCircle(x + r * 0.35f, y + r * 0.12f, r * 0.9f);
}

void drawCloud(float x, float y, float s) {
    setColor(0.85f, 0.88f, 0.95f);
    drawFilledCircle(x, y, 0.045f * s);
    drawFilledCircle(x + 0.045f * s, y + 0.02f * s, 0.055f * s);
    drawFilledCircle(x + 0.095f * s, y, 0.045f * s);
    drawRectangle(x - 0.02f * s, y - 0.04f * s, x + 0.13f * s, y + 0.005f * s);
}

void drawGround() {
    setColor(0.13f, 0.24f, 0.16f);
    drawRectangle(-1.0f, -1.0f, 1.0f, -0.68f);
}

void drawTree(float x, float y, float s) {
    setColor(0.28f, 0.14f, 0.07f);
    drawRectangle(x - 0.025f * s, y, x + 0.025f * s, y + 0.22f * s);

    setColor(0.03f, 0.32f, 0.12f);
    drawTriangle(x - 0.15f * s, y + 0.12f * s, x + 0.15f * s, y + 0.12f * s, x, y + 0.35f * s);
    drawTriangle(x - 0.13f * s, y + 0.23f * s, x + 0.13f * s, y + 0.23f * s, x, y + 0.46f * s);
    drawTriangle(x - 0.11f * s, y + 0.34f * s, x + 0.11f * s, y + 0.34f * s, x, y + 0.55f * s);
}

void drawHouse(float x, float y, float s, bool lit) {
    setColor(0.34f, 0.24f, 0.23f);
    drawRectangle(x - 0.12f * s, y, x + 0.12f * s, y + 0.18f * s);

    setColor(0.18f, 0.09f, 0.06f);
    drawTriangle(x - 0.15f * s, y + 0.18f * s, x + 0.15f * s, y + 0.18f * s, x, y + 0.34f * s);

    if (lit) setColor(1.0f, 0.78f, 0.20f);
    else setColor(0.03f, 0.04f, 0.06f);
    drawRectangle(x - 0.075f * s, y + 0.07f * s, x - 0.02f * s, y + 0.13f * s);
    drawRectangle(x + 0.02f * s, y + 0.07f * s, x + 0.075f * s, y + 0.13f * s);

    setColor(0.12f, 0.07f, 0.04f);
    drawRectangle(x - 0.025f * s, y, x + 0.025f * s, y + 0.08f * s);
}

void drawLantern(float x, float y, float s, bool glowing) {
    setColor(0.10f, 0.08f, 0.05f);
    drawRectangle(x - 0.035f * s, y, x + 0.035f * s, y + 0.12f * s);

    setColor(0.22f, 0.17f, 0.10f);
    drawRectangle(x - 0.055f * s, y + 0.12f * s, x + 0.055f * s, y + 0.15f * s);
    drawRectangle(x - 0.045f * s, y - 0.02f * s, x + 0.045f * s, y);

    if (glowing) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.75f, 0.15f, 0.25f);
        drawFilledCircle(x, y + 0.06f * s, 0.18f * s * lightScale);
        glDisable(GL_BLEND);
        setColor(1.0f, 0.75f, 0.15f);
    }
    else {
        setColor(0.28f, 0.22f, 0.12f);
    }
    drawFilledCircle(x, y + 0.06f * s, 0.035f * s);
}

void drawHero(float x, float y, float s) {
    // Body
    setColor(0.15f, 0.35f, 0.75f);
    drawRectangle(x - 0.04f * s, y, x + 0.04f * s, y + 0.12f * s);

    // Head
    setColor(0.92f, 0.72f, 0.48f);
    drawFilledCircle(x, y + 0.17f * s, 0.055f * s);

    // Hair / hood
    setColor(0.08f, 0.06f, 0.05f);
    drawTriangle(x - 0.055f * s, y + 0.18f * s, x + 0.055f * s, y + 0.18f * s, x, y + 0.245f * s);

    // Eyes
    setColor(0.0f, 0.0f, 0.0f);
    drawFilledCircle(x - 0.018f * s, y + 0.175f * s, 0.006f * s);
    drawFilledCircle(x + 0.018f * s, y + 0.175f * s, 0.006f * s);

    // Legs
    setColor(0.08f, 0.08f, 0.14f);
    drawRectangle(x - 0.035f * s, y - 0.07f * s, x - 0.005f * s, y);
    drawRectangle(x + 0.005f * s, y - 0.07f * s, x + 0.035f * s, y);

    // Small hand lantern
    drawLantern(x + 0.075f * s, y + 0.025f * s, 0.55f * s, lanternActivated || fragmentCollected[0]);
}

void drawLightFragment(float x, float y, float s, bool collected) {
    if (collected) return;

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(starRotation, 0.0f, 0.0f, 1.0f);
    glScalef(s, s, 1.0f);
    setColor(1.0f, 0.85f, 0.18f);
    drawStar(0.0f, 0.0f, 0.06f);
    setColor(1.0f, 0.96f, 0.55f);
    drawStar(0.0f, 0.0f, 0.032f);
    glPopMatrix();
}

void drawStatusBar() {
    setColor(1.0f, 1.0f, 1.0f);
    stringstream ss;
    ss << "Scene " << currentScene + 1 << " / " << TOTAL_SCENES;
    drawText(-0.96f, 0.91f, ss.str(), GLUT_BITMAP_HELVETICA_18);

    stringstream ss2;
    ss2 << "Light fragments: "
        << (fragmentCollected[0] ? 1 : 0)
        + (fragmentCollected[1] ? 1 : 0)
        + (fragmentCollected[2] ? 1 : 0)
        << " / 3";
    drawText(0.47f, 0.91f, ss2.str(), GLUT_BITMAP_HELVETICA_18);
}

// ------------------------------------------------------------
// Procedural Textures
// ------------------------------------------------------------
GLuint createCheckerTexture(int size, unsigned char r1, unsigned char g1, unsigned char b1,
    unsigned char r2, unsigned char g2, unsigned char b2) {
    unsigned char* data = new unsigned char[size * size * 3];

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int checker = ((x / 16) + (y / 16)) % 2;
            int index = (y * size + x) * 3;

            if (checker == 0) {
                data[index] = r1;
                data[index + 1] = g1;
                data[index + 2] = b1;
            }
            else {
                data[index] = r2;
                data[index + 1] = g2;
                data[index + 2] = b2;
            }
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}

void drawTexturedGround(GLuint textureID, float yTop) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(5.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(5.0f, 1.5f); glVertex2f(1.0f, yTop);
    glTexCoord2f(0.0f, 1.5f); glVertex2f(-1.0f, yTop);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawTexturedStoneBlock(float x1, float y1, float x2, float y2) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y1);
    glTexCoord2f(2.0f, 0.0f); glVertex2f(x2, y1);
    glTexCoord2f(2.0f, 2.0f); glVertex2f(x2, y2);
    glTexCoord2f(0.0f, 2.0f); glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ------------------------------------------------------------
// Scene Drawing Functions
// ------------------------------------------------------------
void drawStartScene() {
    setColor(0.04f, 0.06f, 0.15f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    drawMoon(0.68f, 0.62f, 0.13f);

    setColor(1.0f, 0.82f, 0.18f);
    drawLargeText(-0.36f, 0.34f, "The Lost Lantern");

    setColor(0.90f, 0.90f, 0.95f);
    drawText(-0.45f, 0.20f, "A Computer Graphics Story Game");
    drawText(-0.62f, 0.04f, "A small traveler searches for lost light fragments to restore the village lantern.");

    setColor(0.82f, 0.88f, 1.0f);
    drawText(-0.39f, -0.18f, "Controls:");
    drawText(-0.39f, -0.27f, "N: Next Scene    B: Previous Scene    Arrow Keys: Move");
    drawText(-0.39f, -0.36f, "Space: Collect / Action    Mouse Left: Activate Lantern    R: Reset");

    setColor(1.0f, 0.82f, 0.18f);
    drawText(-0.24f, -0.58f, "Press N to begin");
}

void drawDarkVillageScene() {
    setColor(0.04f, 0.05f, 0.13f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    drawMoon(0.72f, 0.70f, 0.10f);

    // Moving clouds - Translation animation
    drawCloud(-0.75f + cloudOffset, 0.72f, 1.0f);
    drawCloud(-0.25f + cloudOffset * 0.6f, 0.62f, 0.8f);

    drawTexturedGround(grassTexture, -0.68f);

    drawHouse(-0.62f, -0.68f, 1.2f, lanternActivated);
    drawHouse(-0.18f, -0.68f, 1.0f, lanternActivated);
    drawHouse(0.35f, -0.68f, 1.25f, lanternActivated);

    // Main village lantern
    setColor(0.10f, 0.08f, 0.06f);
    drawRectangle(-0.02f, -0.68f, 0.02f, -0.18f);
    drawLine(-0.16f, -0.18f, 0.16f, -0.18f, 4.0f);
    drawLantern(0.0f, -0.28f, 1.4f, lanternActivated);

    drawHero(heroX, heroY, 1.0f);

    setColor(1.0f, 1.0f, 1.0f);
    drawText(-0.94f, -0.90f, "The village is dark. Find the missing light fragments.");
    drawStatusBar();
}

void drawForestScene() {
    setColor(0.03f, 0.06f, 0.09f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    drawMoon(-0.72f, 0.70f, 0.09f);
    drawTexturedGround(grassTexture, -0.70f);

    drawTree(-0.82f, -0.70f, 1.0f);
    drawTree(-0.58f, -0.70f, 0.85f);
    drawTree(-0.25f, -0.70f, 1.1f);
    drawTree(0.18f, -0.70f, 0.95f);
    drawTree(0.52f, -0.70f, 1.15f);
    drawTree(0.80f, -0.70f, 0.8f);

    // Rotating light fragment
    drawLightFragment(0.55f, -0.33f, 1.0f, fragmentCollected[0]);

    drawHero(heroX, heroY, 1.0f);

    setColor(1.0f, 1.0f, 1.0f);
    drawText(-0.94f, -0.90f, "Forest Path: Move near the glowing fragment and press Space.");
    drawStatusBar();
}

void drawBridgeScene() {
    setColor(0.04f, 0.07f, 0.16f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    drawMoon(0.76f, 0.74f, 0.08f);

    // Water
    setColor(0.02f, 0.18f, 0.34f);
    drawRectangle(-1.0f, -1.0f, 1.0f, -0.62f);

    // Moving waves
    setColor(0.20f, 0.55f, 0.75f);
    for (int i = 0; i < 8; ++i) {
        float x = -0.95f + i * 0.28f + sin(starRotation * PI / 180.0f) * 0.02f;
        drawLine(x, -0.82f, x + 0.12f, -0.80f, 2.0f);
    }

    // Bridge with slight rotation effect
    glPushMatrix();
    glTranslatef(0.0f, -0.48f, 0.0f);
    glRotatef(sin(bridgeSwing) * 1.7f, 0.0f, 0.0f, 1.0f);
    setColor(0.38f, 0.22f, 0.10f);
    drawRectangle(-0.82f, -0.08f, 0.82f, 0.03f);
    setColor(0.20f, 0.12f, 0.07f);
    for (int i = 0; i < 9; ++i) {
        float x = -0.72f + i * 0.18f;
        drawRectangle(x, -0.11f, x + 0.07f, 0.06f);
    }
    glPopMatrix();

    drawLightFragment(0.15f, -0.22f, 1.0f, fragmentCollected[1]);

    drawHero(heroX, heroY + 0.08f, 1.0f);

    setColor(1.0f, 1.0f, 1.0f);
    drawText(-0.94f, -0.90f, "Windy Bridge: The bridge rotates slightly. Press Space to collect the light.");
    drawStatusBar();
}

void drawTowerScene() {
    setColor(0.04f, 0.05f, 0.12f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    drawMoon(-0.72f, 0.72f, 0.09f);
    drawTexturedGround(grassTexture, -0.72f);

    // Tower base with stone texture
    drawTexturedStoneBlock(-0.25f, -0.72f, 0.25f, 0.38f);

    setColor(0.13f, 0.12f, 0.15f);
    drawTriangle(-0.32f, 0.38f, 0.32f, 0.38f, 0.0f, 0.68f);

    // Windows
    setColor(0.04f, 0.05f, 0.08f);
    drawRectangle(-0.08f, -0.38f, 0.08f, -0.10f);
    drawRectangle(-0.08f, 0.02f, 0.08f, 0.20f);

    // Top lantern glow scaling
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.78f, 0.18f, 0.20f + towerGlow * 0.25f);
    drawFilledCircle(0.0f, 0.46f, 0.20f + towerGlow * 0.12f);
    glDisable(GL_BLEND);

    drawLantern(0.0f, 0.37f, 1.6f, fragmentCollected[2] || lanternActivated);

    drawLightFragment(-0.46f, -0.30f, 1.0f, fragmentCollected[2]);

    drawHero(heroX, heroY, 1.0f);

    setColor(1.0f, 1.0f, 1.0f);
    drawText(-0.94f, -0.90f, "Lantern Tower: Collect the final light and activate the tower lantern.");
    drawStatusBar();
}

void drawFinalScene() {
    setColor(0.04f, 0.08f, 0.17f);
    drawRectangle(-1.0f, -1.0f, 1.0f, 1.0f);

    // Warm final glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.75f, 0.16f, 0.20f);
    drawFilledCircle(0.0f, 0.05f, 0.95f * lightScale);
    glDisable(GL_BLEND);

    drawMoon(0.72f, 0.70f, 0.10f);
    drawTexturedGround(grassTexture, -0.68f);

    drawHouse(-0.62f, -0.68f, 1.2f, true);
    drawHouse(-0.18f, -0.68f, 1.0f, true);
    drawHouse(0.35f, -0.68f, 1.25f, true);

    setColor(0.10f, 0.08f, 0.06f);
    drawRectangle(-0.02f, -0.68f, 0.02f, -0.18f);
    drawLine(-0.16f, -0.18f, 0.16f, -0.18f, 4.0f);
    drawLantern(0.0f, -0.28f, 1.6f, true);

    drawHero(0.58f, -0.55f, 1.0f);

    setColor(1.0f, 0.86f, 0.20f);
    drawLargeText(-0.30f, 0.34f, "Light Restored!");

    setColor(1.0f, 1.0f, 1.0f);
    drawText(-0.47f, 0.20f, "The traveler collected the missing light fragments");
    drawText(-0.35f, 0.11f, "and restored the village lantern.");
    drawText(-0.36f, -0.88f, "Press B to review scenes or Q to exit.");
    drawStatusBar();
}

void drawScene() {
    switch (currentScene) {
    case 0:
        drawStartScene();
        break;
    case 1:
        drawDarkVillageScene();
        break;
    case 2:
        drawForestScene();
        break;
    case 3:
        drawBridgeScene();
        break;
    case 4:
        drawTowerScene();
        break;
    case 5:
        drawFinalScene();
        break;
    default:
        drawStartScene();
        break;
    }
}

// ------------------------------------------------------------
// Scene Logic
// ------------------------------------------------------------
void resetCurrentScene() {
    heroX = -0.70f;
    heroY = -0.55f;
    lanternActivated = false;
}

void nextScene() {
    if (currentScene < TOTAL_SCENES - 1) {
        currentScene++;
        resetCurrentScene();
    }
}

void previousScene() {
    if (currentScene > 0) {
        currentScene--;
        resetCurrentScene();
    }
}

void collectAction() {
    if (currentScene == 2) {
        fragmentCollected[0] = true;
        lanternActivated = true;
    }
    else if (currentScene == 3) {
        fragmentCollected[1] = true;
        lanternActivated = true;
    }
    else if (currentScene == 4) {
        fragmentCollected[2] = true;
        lanternActivated = true;
    }
}

void updateAnimation() {
    if (!animationOn) return;

    cloudOffset += 0.0008f;
    if (cloudOffset > 1.7f) cloudOffset = -1.2f;

    starRotation += 0.7f;
    if (starRotation > 360.0f) starRotation -= 360.0f;

    lightScale = 1.0f + 0.08f * sin(starRotation * PI / 180.0f);
    bridgeSwing += 0.018f;
    towerGlow = 0.5f + 0.5f * sin(starRotation * PI / 180.0f);
}

// ------------------------------------------------------------
// GLUT Callback Functions
// ------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawScene();

    glFlush();
    glutSwapBuffers();
}

void idle() {
    updateAnimation();
    glutPostRedisplay();
}

void reshape(int newWidth, int newHeight) {
    if (newHeight == 0) newHeight = 1;

    winWidth = newWidth;
    winHeight = newHeight;

    glViewport(0, 0, winWidth, winHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'n':
    case 'N':
        nextScene();
        break;

    case 'b':
    case 'B':
        previousScene();
        break;

    case 'r':
    case 'R':
        resetCurrentScene();
        break;

    case ' ':
        collectAction();
        break;

    case 'q':
    case 'Q':
    case 27:
        exit(0);
        break;

    default:
        cout << "Controls: N-next, B-back, Arrow keys-move, Space-action, R-reset, Q-quit" << endl;
        break;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    const float step = 0.04f;

    switch (key) {
    case GLUT_KEY_LEFT:
        heroX -= step;
        break;

    case GLUT_KEY_RIGHT:
        heroX += step;
        break;

    case GLUT_KEY_UP:
        heroY += step;
        break;

    case GLUT_KEY_DOWN:
        heroY -= step;
        break;

    case GLUT_KEY_HOME:
        resetCurrentScene();
        break;

    default:
        break;
    }

    if (heroX < -0.92f) heroX = -0.92f;
    if (heroX > 0.92f) heroX = 0.92f;
    if (heroY < -0.68f) heroY = -0.68f;
    if (heroY > 0.55f) heroY = 0.55f;

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        lanternActivated = true;
    }
    glutPostRedisplay();
}

// ------------------------------------------------------------
// Initialization
// ------------------------------------------------------------
void init() {
    glClearColor(0.04f, 0.05f, 0.12f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Two required textures are generated inside the program to avoid missing image-file errors.
    // Texture 1: grass-like checker texture.
    grassTexture = createCheckerTexture(128, 22, 90, 38, 38, 125, 55);

    // Texture 2: stone-like checker texture.
    stoneTexture = createCheckerTexture(128, 95, 95, 105, 130, 130, 145);
}

// ------------------------------------------------------------
// Main Function
// ------------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 60);
    glutCreateWindow("The Lost Lantern - CS2206 Computer Graphics Project");


    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
