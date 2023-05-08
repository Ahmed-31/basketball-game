#define M_PI 3.14159265358979323846
#define CAMERA_STATE_FIXED_1 0
#define CAMERA_STATE_FIXED_2 1
#define CAMERA_STATE_FOLLOW 2
#define CAMERA_STATE_FREE 3
#define MOUSE_SENSITIVITY 0.3
#define GRAVITY 10
#define REFRESH_RATE 16
#include "model.h";

Model player_body, basketball_panel1, basketball_panel2;

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
// Define keyframes for the player's running animation
std::vector<Keyframe> player_keyframes = {
    { 0.0f, vec3(-8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 0.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 1.0f, vec3(8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 1.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 2.0f, vec3(-8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 2.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 3.0f, vec3(8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 3.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 4.0f, vec3(-8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 4.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 5.0f, vec3(8.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) },
    { 5.5f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -45.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f) }
};
float vertical_velocity = 0.0f;
float prev_time = 0.0f;
unsigned int camera_state = 0;
float player_move_speed = 0.15;
bool keys[256] = { false }; // Array to store the state of each key
bool pause = false;
int last_x, last_y;


const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

void init();
static void resize(int, int);
static void setCamera(int);
static void display();
void mouse(int, int, int, int);
void motion(int, int);
static void key(unsigned char, int, int);
static void keyUp(unsigned char, int , int);
static void special(int, int, int);
static void specialUp(int, int, int);
static void idle();
static void timer(int);
void interpolateKeyframes(float, const Keyframe&, const Keyframe&, vec3&, vec3&, vec3&);
/* Program entry point */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("BasketBall Game");
    player_body.loadObjFile("Models/low_poly_body.obj");
    basketball_panel1.loadObjFile("Models/basketBall panel_me.obj");
    basketball_panel2 = basketball_panel1;
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
    glClearColor(1, 1, 1, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    const float ar = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ar, 2.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
static void setCamera(int camera_state) {
    if (camera_state == CAMERA_STATE_FIXED_1) {
        camera.position[0] = 0.0f;
        camera.position[1] = 30.0f;
        camera.position[2] = -100.0f;
        camera.lookat[0] = 0.0f;
        camera.lookat[1] = 0.0f;
        camera.lookat[2] = 0.0f;
    }
    else if (camera_state == CAMERA_STATE_FIXED_2) {
        camera.position[0] = 100.0f;
        camera.position[1] = 50.0f;
        camera.position[2] = 0.0f;
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
static void display()
{
    const double current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    // Calculate the time delta since the last frame
    float dt = current_time - prev_time;
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
    glTranslatef(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.6, 0.0); // set the color to green
    glVertex3f(-40.0, 0.0, -65.0);
    glVertex3f(-40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, -65.0);
    glEnd();
    glPopMatrix();

    // Update the player's position, rotation, and scale based on the current time
    for (int i = 0; i < player_keyframes.size() - 1; i++) {
        if (player_keyframes[i].time <= current_time && player_keyframes[i + 1].time > current_time) {
            interpolateKeyframes(current_time, player_keyframes[i], player_keyframes[i + 1],
                player_body.transform.position,
                player_body.transform.rotation,
                player_body.transform.scale);
            break;
        }
    }
    // draw the player body
    glPushMatrix();
    vertical_velocity -= GRAVITY * dt;
    player_body.transform.position.y += vertical_velocity * dt;
    // Clamp the object's vertical position to the ground level
    if (player_body.transform.position.y < 1)
    {
        player_body.transform.position.y = 1;
        vertical_velocity = 0.0f;
    }
    glTranslated(player_body.transform.position.x, player_body.transform.position.y + 4.557, player_body.transform.position.z);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(player_body.transform.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(player_body.transform.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(player_body.transform.rotation.z, 1.0f, 0.0f, 1.0f);
    glScalef(player_body.transform.scale.x, player_body.transform.scale.y, player_body.transform.scale.z);
    player_body.drawShape();
    glPopMatrix();

    glPushMatrix();
    basketball_panel1.transform.position.z = -59.5;
    glTranslated(basketball_panel1.transform.position.x, 0.0, basketball_panel1.transform.position.z);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(basketball_panel1.transform.rotation.y, 0, 1, 0);
    glScalef(1.6, 1.6, 1.2);
    basketball_panel1.drawShape();
    glPopMatrix();

    glPushMatrix();
    basketball_panel2.transform.position.z = 59.5;
    glTranslated(basketball_panel2.transform.position.x, 0.0, basketball_panel2.transform.position.z);
    glColor3f(1.0, 0.0, 0.0);
    basketball_panel2.transform.rotation.y = 180;
    glScalef(1.6, 1.6, 1.2);
    glRotatef(basketball_panel2.transform.rotation.y, 0, 1, 0);
    basketball_panel2.drawShape();
    glPopMatrix();

    glutSwapBuffers();
    prev_time = current_time;
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
static void key(unsigned char key, int x, int y)
{
    keys[key] = true; // Set the state of the pressed key to true
    switch (key)
    {
    case 27:
    case 'q':
        exit(0);
        break;
    case 'C':
    case 'c':
        camera_state = ++camera_state % 4;
        camera.lookat[0] = 0.0;
        camera.lookat[1] = 20.0;
        camera.lookat[2] = 20.0;
        camera.angleX = 100.0;
        camera.angleY = 50.0;
        break;
    }

    glutPostRedisplay();
}
static void keyUp(unsigned char key, int x, int y)
{
    keys[key] = false; // Set the state of the released key to false
}
static void special(int key, int x, int y)
{
    switch (key)
    {
        break;
    }
}
static void specialUp(int key, int x, int y)
{
    switch (key)
    {
        break;
    }
}
static void idle()
{
    float forward_x = sin(player_body.transform.rotation.y * M_PI / 180.0f);
    float forward_z = -cos(player_body.transform.rotation.y * M_PI / 180.0f);
    // Check the state of each key and take appropriate action
    if (keys['w']) {
        player_body.transform.position.x += forward_x * player_move_speed;
        player_body.transform.position.z -= forward_z * player_move_speed;
    }
    if (keys['s']) {
        player_body.transform.position.x -= forward_x * player_move_speed;
        player_body.transform.position.z += forward_z * player_move_speed;
    }
    if (keys['a']) {
        player_body.transform.rotation.y += player_move_speed + 0.85;
    }
    if (keys['d']) {
        player_body.transform.rotation.y -= player_move_speed + 0.65;
    }
    if (keys[' ']) {
        vertical_velocity = 5.0f;
    }
    if (keys['i']) {
        camera.position[1] += 0.5;
        camera.lookat[1] += 0.5;
    }
    if (keys['k']) {
        camera.position[1] -= 0.5;
        camera.lookat[1] -= 0.5;
    }
    if (keys['l']) {
        camera.position[0] += 0.5;
        camera.lookat[0] += 0.5;
    }
    if (keys['j']) {
        camera.position[0] -= 0.5;
        camera.lookat[0] -= 0.5;
    }
    if (keys['o']) {
        camera.position[2] += 0.5;
        camera.lookat[2] += 0.5;
    }
    if (keys['u']) {
        camera.position[2] -= 0.5;
        camera.lookat[2] -= 0.5;
    }

    glutPostRedisplay();
}
static void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(REFRESH_RATE, timer, 0);
}
void interpolateKeyframes(float t, const Keyframe& kf1, const Keyframe& kf2, vec3& position, vec3& rotation, vec3& scale) {
    float dt = kf2.time - kf1.time;
    float t1 = (t - kf1.time) / dt;
    float t2 = 1.0f - t1;

    // Interpolate position
    position = t2 * kf1.position + t1 * kf2.position;

    // Interpolate rotation
    quat q1 = quat(radians(kf1.rotation));
    quat q2 = quat(radians(kf2.rotation));
    quat q = slerp(q1, q2, t1);
    rotation = degrees(eulerAngles(q));

    // Interpolate scale
    scale = t2 * kf1.scale + t1 * kf2.scale;
}