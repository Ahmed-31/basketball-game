#define M_PI 3.14159265358979323846
#define CAMERA_STATE_FIXED_1 0
#define CAMERA_STATE_FIXED_2 1
#define CAMERA_STATE_FOLLOW 2
#define CAMERA_STATE_FREE 3
#define MOUSE_SENSITIVITY 0.3
#define GRAVITY 10
#define REFRESH_RATE 16
#include "model.h";
#include "ModelGroup.h"

Model basketball_panel;
ModelGroup player_body;

struct Camera {
    GLfloat position[3] = { 0.0f, 5.5f, 1.0f };
    GLfloat lookat[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat up[3] = { 0.0f, 1.0f, 0.0f };
    GLfloat offset[2] = { -15.0f, 8.0f };
    GLfloat angleX = 0.0f;
    GLfloat angleY = 0.0f;
}camera;
struct Keyframe {
    float time = 0.0f;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};
struct Animate {
    bool jump = false;
    bool run = false;
}animate;
// Define keyframes for the player's jumping animation
vector<Keyframe> jump_keyframes = {
    {0.1f * 0.15f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.94f, 1.0f}}, // Starting position
    {0.5f * 0.15f, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.84f, 1.0f}},
    {1.0f * 0.15f, {0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {1.5f * 0.15f, {0.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {2.0f * 0.15f, {0.0f, 4.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {2.5f * 0.15f, {0.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {3.0f * 0.15f, {0.0f, 4.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {3.5f * 0.15f, {0.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {4.0f * 0.15f, {0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {4.5f * 0.15f, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.95f, 1.0f}},
    {5.0f * 0.15f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
};
// Define keyframes for the player's running animation legs
vector<Keyframe> run_keyframes_legs = {
    {0.0f, {0.0f, 0.45f, 1.4f}, {-20.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}, // Starting position
    {0.0f, {0.0f, 0.4f, 1.2f}, {-15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.3f, 0.8f}, {-10.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.2f, 0.4f}, {-5.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.2f, -0.4f}, {5.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.3f, -0.9f}, {10.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.4f, -1.3f}, {15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.45f, -1.5f}, {20.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
};
// Define keyframes for the player's running animation legs
vector<Keyframe> run_keyframes_hands = {
    {0.0f, {0.0f, 0.3f, -2.4f}, {20.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}, // Starting position
    {0.0f, {0.0f, 0.2f, -1.8f}, {15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.1f, -1.2f}, {10.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.0f, -0.6f}, {5.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.0f, 0.6f}, {-5.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.2f, 1.2f}, {-10.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.3f, 1.8f}, {-15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {0.0f, {0.0f, 0.4f, 2.2f}, {-18.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
};
unsigned int camera_state = 0;
float player_move_speed = 0.05;
bool keys[256] = { false }; // Array to store the state of each key
bool pause = false;
float jump_start_time = 0.0f;
float run_start_time = 0.0f;
int frame_count = 0;
int current_time = 0;
int previous_time = 0;
float frame_rate = 0.0f;
int last_x, last_y;


const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

GLuint basketball_panel_list, basketball_court_list, long_wall, short_wall;

void init();
void resize(int, int);
void setCamera(int);
void display();
void mouse(int, int, int, int);
void motion(int, int);
void key(unsigned char, int, int);
void keyUp(unsigned char, int , int);
void special(int, int, int);
void specialUp(int, int, int);
void idle();
void timer(int);
void interpolateKeyframes(float, const Keyframe&, const Keyframe&, vec3&, vec3&, vec3&);
void showFrameRate();
/* Program entry point */
int main(int argc, char* argv[])
{
    vector<string> filenames{
        "Models/core_body.obj",
        "Models/left_hand.obj",
        "Models/right_hand.obj",
        "Models/left_leg.obj",
        "Models/right_leg.obj"
    };
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("BasketBall Game");
    player_body.loadChildModel(filenames);
    basketball_panel.loadObjFile("Models/basketBall panel_me.obj");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);

    glutMouseFunc(mouse); // set the mouse function
    glutMotionFunc(motion); // set the motion function
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(special);
    //glutSpecialUpFunc(specialUp);
    glutIdleFunc(idle);
    glutTimerFunc(1000, timer, 0);

    init();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cerr << "OpenGL error: " << error << endl;
    }
    glutMainLoop();
    return 0;
}

void init() {
    // initialize the time of keyframes.
    for (int i = 0; i < jump_keyframes.size(); i++) {
        jump_keyframes[i].time = 0.1 + i * 0.1f;
    }
    for (int i = 0; i < run_keyframes_hands.size(); i++) {
        run_keyframes_legs[i].time = 0.1 + i * 0.1f;
        run_keyframes_hands[i].time = 0.1 + i * 0.1f;
    }

    glClearColor(1, 1, 1, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    basketball_court_list = glGenLists(1);
    glNewList(basketball_court_list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(-40.0, 0.0, -65.0);
    glVertex3f(-40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, -65.0);
    glEnd();
    glPopMatrix();
    glEndList();

    long_wall = glGenLists(1);
    glNewList(long_wall, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(0.0, -20.0, -65.0);
    glVertex3f(0.0, -20.0, 65.0);
    glVertex3f(0.0, 20.0, 65.0);
    glVertex3f(0.0, 20.0, -65.0);
    glEnd();
    glPopMatrix();
    glEndList();

    short_wall = glGenLists(1);
    glNewList(short_wall, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(0.0, -20.0, -40.0);
    glVertex3f(0.0, -20.0, 40.0);
    glVertex3f(0.0, 20.0, 40.0);
    glVertex3f(0.0, 20.0, -40.0);
    glEnd();
    glPopMatrix();
    glEndList();

    basketball_panel_list = glGenLists(1);
    glNewList(basketball_panel_list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glPushMatrix();
    glRotatef(basketball_panel.transform.rotation.y, 0, 1, 0);
    glScalef(1.6, 1.6, 1.2);
    basketball_panel.drawShape();
    glPopMatrix();
    glEndList();
}

/* GLUT callback Handlers */
void resize(int width, int height)
{
    const float ar = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ar, 2.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void setCamera(int camera_state) {
    if (camera_state == CAMERA_STATE_FIXED_1) {
        camera.position[0] = 39.0f;
        camera.position[1] = 20.0f;
        camera.position[2] = -63.0f;
        camera.lookat[0] = 0.0f;
        camera.lookat[1] = 0.0f;
        camera.lookat[2] = 0.0f;
    }
    else if (camera_state == CAMERA_STATE_FIXED_2) {
        camera.position[0] = -39.0f;
        camera.position[1] = 20.0f;
        camera.position[2] = 63.0f;
        camera.lookat[0] = 0.0f;
        camera.lookat[1] = 0.0f;
        camera.lookat[2] = 0.0f;
    }
    else if (camera_state == CAMERA_STATE_FOLLOW) {
        // Calculate camera position and target based on player position and orientation
        camera.lookat[0] = player_body.transform.position.x;
        camera.lookat[1] = player_body.transform.position.y + camera.offset[1];
        camera.lookat[2] = player_body.transform.position.z;

        camera.position[0] = player_body.transform.position.x + camera.offset[0] * sin(player_body.transform.rotation.y * M_PI / 180.0f);
        camera.position[1] = player_body.transform.position.y + camera.offset[0] * sin(player_body.transform.rotation.x * M_PI / 180.0f) + camera.offset[1];
        camera.position[2] = player_body.transform.position.z + camera.offset[0] * cos(player_body.transform.rotation.y * M_PI / 180.0f);
    }
    else if (camera_state == CAMERA_STATE_FREE) {
        // Update the camera position and orientation based on user input
        float camera_angle_x_rad = camera.angleX * M_PI / 180.0f;
        float camera_angle_y_rad = camera.angleY * M_PI / 180.0f;
        camera.position[0] = camera.lookat[0] + camera.offset[0] * sin(camera_angle_x_rad) * sin(camera_angle_y_rad);
        camera.position[1] = camera.lookat[1] + camera.offset[0] * cos(camera_angle_x_rad);
        camera.position[2] = camera.lookat[2] + camera.offset[0] * sin(camera_angle_x_rad) * cos(camera_angle_y_rad);

    }
}
void display()
{
    showFrameRate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up camera
    setCamera(camera_state);

    // Reset the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply the camera transformation to the modelview matrix
    gluLookAt(camera.position[0], camera.position[1], camera.position[2],
        camera.lookat[0], camera.lookat[1], camera.lookat[2], camera.up[0], camera.up[1], camera.up[2]);

    // draw the basketball court
    glPushMatrix();
    glColor3f(0.0, 0.6, 0.0); // set the color to green
    glCallList(basketball_court_list);
    glPopMatrix();

    // draw the basketball coart walls
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.0);
    glTranslatef(40.0f, 20.0f, 0.0f);
    glCallList(long_wall);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.0);
    glTranslatef(-40.0f, 20.0f, 0.0f);
    glCallList(long_wall);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.0);
    glTranslatef(0.0f, 20.0f, 65.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glCallList(short_wall);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.0);
    glTranslatef(0.0f, 20.0f, -65.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glCallList(short_wall);
    glPopMatrix();

    // draw the player body
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    player_body.drawShape();
    glPopMatrix();

    // draw basketball panels
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    basketball_panel.transform.position.z = -59.5;
    glTranslated(basketball_panel.transform.position.x, 0.0, basketball_panel.transform.position.z);
    glCallList(basketball_panel_list);
    glPopMatrix();
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    basketball_panel.transform.position.z = 59.5;
    basketball_panel.transform.rotation.y = 180;
    glTranslated(basketball_panel.transform.position.x, 0.0, basketball_panel.transform.position.z);
    glRotatef(basketball_panel.transform.rotation.y, 0, 1, 0);
    glCallList(basketball_panel_list);
    glPopMatrix();

    glutSwapBuffers();
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        last_x = x;
        last_y = y;
    }
}
void motion(int x, int y) {

    int dx = x - last_x;
    int dy = y - last_y;

    camera.angleX -= dy * MOUSE_SENSITIVITY;
    camera.angleY -= dx * MOUSE_SENSITIVITY;

    last_x = x;
    last_y = y;

    glutPostRedisplay();
}
void key(unsigned char key, int x, int y)
{
    keys[key] = true; // Set the state of the pressed key to true
    switch (key)
    {
    case 27:
    case 'q':
        exit(0);
        break;
    case ' ':
        if (!animate.jump) {
            // Start jump animation
            animate.jump = true;
            jump_start_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        }
        break;
    case 'C':
    case 'c':
        camera_state = ++camera_state % 4;
        camera.lookat[0] = -15.0;
        camera.lookat[1] = 10.0;
        camera.lookat[2] = 0.0;
        camera.angleX = 100.0;
        camera.angleY = 50.0;
        break;
    }

    glutPostRedisplay();
}
void keyUp(unsigned char key, int x, int y)
{
    keys[key] = false; // Set the state of the released key to false
}
void special(int key, int x, int y)
{
    switch (key)
    {
        break;
    }
}
void specialUp(int key, int x, int y)
{
    switch (key)
    {
        break;
    }
}
void idle()
{
    float camera_free_speed = 0.01;
    float forward_x = sin(player_body.transform.rotation.y * M_PI / 180.0f);
    float forward_z = -cos(player_body.transform.rotation.y * M_PI / 180.0f);
    // Check the state of each key and take appropriate action
    if (keys['w']) {
        if (!animate.run) {
            animate.run = true;
            run_start_time = glutGet(GLUT_ELAPSED_TIME) / 1000;
        }
        player_body.transform.position.x += forward_x * player_move_speed;
        player_body.transform.position.z -= forward_z * player_move_speed;
    }
    if (keys['s']) {
        if (!animate.run) {
            animate.run = true;
            run_start_time = glutGet(GLUT_ELAPSED_TIME) / 1000;
        }
        player_body.transform.position.x -= forward_x * player_move_speed;
        player_body.transform.position.z += forward_z * player_move_speed;
    }
    if (keys['a']) {
        player_body.transform.rotation.y += player_move_speed + 0.15;
    }
    if (keys['d']) {
        player_body.transform.rotation.y -= player_move_speed + 0.15;
    }
    if (keys['i']) {
        camera.position[1] += camera_free_speed;
        camera.lookat[1] += camera_free_speed;
    }
    if (keys['k']) {
        camera.position[1] -= camera_free_speed;
        camera.lookat[1] -= camera_free_speed;
    }
    if (keys['l']) {
        camera.position[0] += camera_free_speed;
        camera.lookat[0] += camera_free_speed;
    }
    if (keys['j']) {
        camera.position[0] -= camera_free_speed;
        camera.lookat[0] -= camera_free_speed;
    }
    if (keys['o']) {
        camera.position[2] += camera_free_speed;
        camera.lookat[2] += camera_free_speed;
    }
    if (keys['u']) {
        camera.position[2] -= camera_free_speed;
        camera.lookat[2] -= camera_free_speed;
    }
    glutPostRedisplay();
}
void timer(int value) {
    glutPostRedisplay();

    // Update player position based on animation
    if (animate.jump) {
        float time_since_jump_start = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - jump_start_time;
        if (time_since_jump_start >= jump_keyframes[jump_keyframes.size() - 1].time) {
            // Animation is finished
            animate.jump = false;
            player_body.transform.position.y = 0.0f;
        }
        else {
            // Find current keyframe
            int i = 0;
            while (i < jump_keyframes.size() - 1 && time_since_jump_start >= jump_keyframes[i + 1].time) {
                i++;
            }
            // Interpolate between keyframes
            float t = (time_since_jump_start - jump_keyframes[i].time) / (jump_keyframes[i + 1].time - jump_keyframes[i].time);
            player_body.transform.position.y = jump_keyframes[i].position.y * (1 - t) + jump_keyframes[i + 1].position.y * t;
            player_body.transform.scale.y = jump_keyframes[i].scale.y * (1 - t) + jump_keyframes[i + 1].scale.y * t;
        }
        // Clamp the object's vertical position to the ground level
        if (player_body.transform.position.y < 0.0f)
        {
            player_body.transform.position.y = 0.0f;
        }
    }

    // Update hands based on animation
    if (animate.run) {
        for (int i = 0; i < run_keyframes_hands.size(); i++) {

        }
        float time_since_run_start = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - run_start_time;
        // left hand
        if (time_since_run_start >= run_keyframes_hands[run_keyframes_hands.size() - 1].time) {
            // Animation is finished
            animate.run = false;
            player_body.childModels[1].transform.rotation.x = 0.0f;
            player_body.childModels[1].transform.position.z = 0.0f;
            player_body.childModels[1].transform.position.y = 0.0f;
        }
        else {
            // Find current keyframe
            int i = 0;
            while (i < run_keyframes_hands.size() - 1 && time_since_run_start >= run_keyframes_hands[i + 1].time) {
                i++;
            }
            // Interpolate between keyframes
            float t = (time_since_run_start - run_keyframes_hands[i].time) / (run_keyframes_hands[i + 1].time - run_keyframes_hands[i].time);
            player_body.childModels[1].transform.rotation.x = run_keyframes_hands[i].rotation.x * (1 - t) + run_keyframes_hands[i + 1].rotation.x * t;
            player_body.childModels[1].transform.position.z = run_keyframes_hands[i].position.z * (1 - t) + run_keyframes_hands[i + 1].position.z * t;
            player_body.childModels[1].transform.position.y = run_keyframes_hands[i].position.y * (1 - t) + run_keyframes_hands[i + 1].position.y * t;

        }
        // right hand
        if (time_since_run_start >= run_keyframes_hands[run_keyframes_hands.size() - 1].time) {
            // Animation is finished
            animate.run = false;
            player_body.childModels[2].transform.rotation.x = 0.0f;
            player_body.childModels[2].transform.position.z = 0.0f;
            player_body.childModels[2].transform.position.y = 0.0f;
        }
        else {
            // Find current keyframe
            int i = 0;
            while (i < run_keyframes_hands.size() - 1 && time_since_run_start >= run_keyframes_hands[i + 1].time) {
                i++;
            }
            // Interpolate between keyframes
            float t = (time_since_run_start - run_keyframes_hands[i].time) / (run_keyframes_hands[i + 1].time - run_keyframes_hands[i].time);
            player_body.childModels[2].transform.rotation.x = (-run_keyframes_hands[i].rotation.x) * (1 - t) + (-run_keyframes_hands[i + 1].rotation.x) * t;
            player_body.childModels[2].transform.position.z = (-run_keyframes_hands[i].position.z) * (1 - t) + (-run_keyframes_hands[i + 1].position.z) * t;
            player_body.childModels[2].transform.position.y = run_keyframes_hands[i].position.y * (1 - t) + run_keyframes_hands[i + 1].position.y * t;
        }

        // left leg
        if (time_since_run_start >= run_keyframes_legs[run_keyframes_legs.size() - 1].time) {
            // Animation is finished
            animate.run = false;
            player_body.childModels[3].transform.rotation.x = 0.0f;
            player_body.childModels[3].transform.position.z = 0.0f;
            player_body.childModels[3].transform.position.y = 0.0f;
        }
        else {
            // Find current keyframe
            int i = 0;
            while (i < run_keyframes_legs.size() - 1 && time_since_run_start >= run_keyframes_legs[i + 1].time) {
                i++;
            }
            // Interpolate between keyframes
            float t = (time_since_run_start - run_keyframes_legs[i].time) / (run_keyframes_legs[i + 1].time - run_keyframes_legs[i].time);
            player_body.childModels[3].transform.rotation.x = run_keyframes_legs[i].rotation.x * (1 - t) + run_keyframes_legs[i + 1].rotation.x * t;
            player_body.childModels[3].transform.position.z = run_keyframes_legs[i].position.z * (1 - t) + run_keyframes_legs[i + 1].position.z * t;
            player_body.childModels[3].transform.position.y = run_keyframes_legs[i].position.y * (1 - t) + run_keyframes_legs[i + 1].position.y * t;
            
        }

        // right leg
        if (time_since_run_start >= run_keyframes_legs[run_keyframes_legs.size() - 1].time) {
            // Animation is finished
            animate.run = false;
            player_body.childModels[4].transform.rotation.x = 0.0f;
            player_body.childModels[4].transform.position.z = 0.0f;
            player_body.childModels[4].transform.position.y = 0.0f;
        }
        else {
            // Find current keyframe
            int i = 0;
            while (i < run_keyframes_legs.size() - 1 && time_since_run_start >= run_keyframes_legs[i + 1].time) {
                i++;
            }
            // Interpolate between keyframes
            float t = (time_since_run_start - run_keyframes_legs[i].time) / (run_keyframes_legs[i + 1].time - run_keyframes_legs[i].time);
            player_body.childModels[4].transform.rotation.x = (-run_keyframes_legs[i].rotation.x) * (1 - t) + (-run_keyframes_legs[i + 1].rotation.x) * t;
            player_body.childModels[4].transform.position.z = (-run_keyframes_legs[i].position.z) * (1 - t) + (-run_keyframes_legs[i + 1].position.z) * t;
            player_body.childModels[4].transform.position.y = run_keyframes_legs[i].position.y * (1 - t) + run_keyframes_legs[i + 1].position.y * t;
        }
    }
    glutTimerFunc(REFRESH_RATE, timer, 0);
}
void showFrameRate() {
    // Increment the frame count
    frame_count++;

    // Get the current time in milliseconds
    current_time = glutGet(GLUT_ELAPSED_TIME);

    // Calculate the time elapsed since the last frame
    int elapsed_time = current_time - previous_time;

    // Calculate the frame rate
    if (elapsed_time > 1000) {
        frame_rate = frame_count / (elapsed_time / 1000.0f);
        previous_time = current_time;
        frame_count = 0;
        //cout << "frame rate is : " << frame_rate << endl;
    }
}