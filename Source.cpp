#define M_PI 3.14159265358979323846
#define CAMERA_STATE_FIXED_1 0
#define CAMERA_STATE_FIXED_2 1
#define CAMERA_STATE_FOLLOW 2
#include <iostream>
#include <windows.h>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <GL/glut.h>

using namespace std;


class Model {
public:
    struct Vertex {
        float x, y, z;
    };

    struct Normal {
        float x, y, z;
    };

    struct TextureCoord {
        float u, v;
    };

    struct FaceVertex {
        int vertexIndex, normalIndex, textureIndex;
    };

    struct Position {
        float x = 1.0, y = 1.0, z = 1.0;
    }position;

    struct Rotation {
        float x = 1.0, y = 1.0, z = 1.0;
    }rotation;

    vector<Vertex> vertices;
    vector<Normal> normals;
    vector<TextureCoord> textureCoords;
    vector<vector<FaceVertex>> faces;

    void parseVertex(const std::string& line, std::vector<Vertex>& vertices) {
        Vertex vertex;
        std::istringstream iss(line.substr(2));
        iss >> vertex.x >> vertex.y >> vertex.z;
        vertices.push_back(vertex);
    }

    void parseNormal(const std::string& line, std::vector<Normal>& normals) {
        Normal normal;
        std::istringstream iss(line.substr(3));
        iss >> normal.x >> normal.y >> normal.z;
        normals.push_back(normal);
    }

    void parseTextureCoord(const std::string& line, std::vector<TextureCoord>& textureCoords) {
        TextureCoord textureCoord;
        std::istringstream iss(line.substr(3));
        iss >> textureCoord.u >> textureCoord.v;
        textureCoords.push_back(textureCoord);
    }

    void parseFace(const std::string& line, std::vector<FaceVertex>& faceVertices) {
        faceVertices.clear();
        std::istringstream iss(line.substr(2));
        std::string token;
        while (std::getline(iss, token, ' ')) {
            std::istringstream viss(token);
            std::string vertexIndexStr, textureIndexStr, normalIndexStr;
            std::getline(viss, vertexIndexStr, '/');
            std::getline(viss, textureIndexStr, '/');
            std::getline(viss, normalIndexStr, '/');
            int vertexIndex = std::stoi(vertexIndexStr) - 1;
            int textureIndex = textureIndexStr.empty() ? -1 : std::stoi(textureIndexStr) - 1;
            int normalIndex = normalIndexStr.empty() ? -1 : std::stoi(normalIndexStr) - 1;
            FaceVertex faceVertex;
            faceVertex.vertexIndex = vertexIndex;
            faceVertex.textureIndex = textureIndex;
            faceVertex.normalIndex = normalIndex;
            faceVertices.push_back(faceVertex);
        }
    }

    void loadObjFile(string filename) {
        ifstream file(filename.c_str());
        if (!file) {
            cerr << "Error: could not open file " << filename << endl;
            exit(1);
        }
        int numberOfLines = countLines(filename), currLine = 1;

        cout << "number of lines of " << filename << " is : " << numberOfLines << endl;
        string line;
        while (getline(file, line)) {
            if (currLine++ % (numberOfLines / 100) == 0) {
                cout << "progress loading obj " << (currLine * 100 / numberOfLines) << "%" << "\r";
            }

            if (line.substr(0, 2) == "v ") {
                parseVertex(line, vertices);
            }
            else if (line.substr(0, 2) == "vn") {
                parseNormal(line, normals);
            }
            else if (line.substr(0, 2) == "vt") {
                parseTextureCoord(line, textureCoords);
            }
            else if (line.substr(0, 2) == "f ") {
                std::vector<FaceVertex> faceVertices;
                parseFace(line, faceVertices);
                faces.push_back(faceVertices);
            }
        }
        cout << "progress loading obj 100%" << endl;
    }
    void showData() {
        for (const auto& vertex : vertices) {
            cout << "v " << vertex.x << ' ' << vertex.y << ' ' << vertex.z << endl;
        }
        for (const auto& face : faces) {
            for (const auto& faceVertex : face) {
                cout << "f " << faceVertex.vertexIndex << '/' << faceVertex.textureIndex << '/' << faceVertex.normalIndex << ' ';
            }
            cout << endl;
        }
    }
    int countLines(const std::string& filename) {
        std::ifstream file(filename);
        int count = 0;
        char buffer[4096];

        while (file.read(buffer, sizeof(buffer))) {
            count += std::count(buffer, buffer + file.gcount(), '\n');
            //count += count(buffer, buffer + file.gcount(), '\n');
        }

        if (file.eof()) {
            // The last line of the file may not end with a newline character,
            // so we need to check for this case and increment the line count if necessary.
            if (file.gcount() > 0 && buffer[file.gcount() - 1] != '\n') {
                count++;
            }
        }
        else {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            std::cerr << "Error opening file: " << errorMessage << std::endl;
        }

        return count;
    }
}player_body, basketball_panel1, basketball_panel2;
struct Camera {
    GLfloat position[3] = { 0.0f, 5.5f, 1.0f };
    GLfloat lookat[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat up[3] = { 0.0f, 1.0f, 0.0f };
    GLfloat offset[2] = { -15.0f, 8.0f };
    GLfloat angleX = 0.0f;
    GLfloat angleY = 0.0f;
}camera;
int rotSpeed = 20;
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

static void resize(int, int);
static void drawShape(Model);
static void setCamera(int);
static void display();
void mouse(int, int, int, int);
void motion(int, int);
static void key(unsigned char, int, int);
static void keyUp(unsigned char, int , int);
static void special(int, int, int);
static void specialUp(int, int, int);
static void idle();
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

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cerr << "OpenGL error: " << error << endl;
    }
    glutMainLoop();
    return 0;
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
static void drawShape(Model model) {
    glBegin(GL_TRIANGLES);
    static bool firstCall = true;
    int i = 1;
    for (const auto& faceVertices : model.faces) {
        for (const auto& faceVertex : faceVertices) {
            if (faceVertex.normalIndex != -1 && (unsigned)faceVertex.normalIndex < model.normals.size()) {
                glNormal3f(model.normals[faceVertex.normalIndex].x, model.normals[faceVertex.normalIndex].y, model.normals[faceVertex.normalIndex].z);
            }
            if (faceVertex.textureIndex != -1 && (unsigned)faceVertex.textureIndex < model.textureCoords.size()) {
                glTexCoord2f(model.textureCoords[faceVertex.textureIndex].u, model.textureCoords[faceVertex.textureIndex].v);
            }
            if ((unsigned)faceVertex.vertexIndex < model.vertices.size()) {
                glVertex3f(model.vertices[faceVertex.vertexIndex].x, model.vertices[faceVertex.vertexIndex].y, model.vertices[faceVertex.vertexIndex].z);
            }
        }
        if (firstCall) {
            cout << "progress drawing " << (i++ * 100 / model.faces.size()) << "%" << "\r";
        }
    }

    firstCall = false;
    glEnd();
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
        camera.angleY = player_body.rotation.y;
        camera.angleX = player_body.rotation.x;

        // Calculate camera position and target based on player position and orientation
        camera.lookat[0] = player_body.position.x;
        camera.lookat[1] = player_body.position.y + camera.offset[1];
        camera.lookat[2] = player_body.position.z;

        camera.position[0] = player_body.position.x + camera.offset[0] * sin(camera.angleY * M_PI / 180.0f);
        camera.position[1] = player_body.position.y + camera.offset[0] * sin(camera.angleX * M_PI / 180.0f) + camera.offset[1];
        camera.position[2] = player_body.position.z + camera.offset[0] * cos(camera.angleY * M_PI / 180.0f);
    }
}
static void display()
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t * rotSpeed;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up camera
    setCamera(camera_state);

    // Reset the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Apply the camera transformation to the modelview matrix
    gluLookAt(camera.position[0], camera.position[1], camera.position[2],
        camera.lookat[0], camera.lookat[1], camera.lookat[2], camera.up[0], camera.up[1], camera.up[2]);

    glTranslatef(0.0, 0.0, 0.0);
    glPushMatrix();
    // draw the basketball court
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.6, 0.0); // set the color to green
    glVertex3f(-40.0, 0.0, -65.0);
    glVertex3f(-40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, 65.0);
    glVertex3f(40.0, 0.0, -65.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(player_body.position.x, player_body.position.y + 4.557, player_body.position.z);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(player_body.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(player_body.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(player_body.rotation.z, 1.0f, 0.0f, 1.0f);
    drawShape(player_body);
    glPopMatrix();

    glPushMatrix();
    basketball_panel1.position.z = -59.5;
    glTranslated(basketball_panel1.position.x, 0.0, basketball_panel1.position.z);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(basketball_panel1.rotation.y, 0, 1, 0);
    glScalef(1.6, 1.6, 1.2);
    drawShape(basketball_panel1);
    glPopMatrix();

    glPushMatrix();
    basketball_panel2.position.z = 59.5;
    glTranslated(basketball_panel2.position.x, 0.0, basketball_panel2.position.z);
    glColor3f(1.0, 0.0, 0.0);
    basketball_panel2.rotation.y = 180;
    glScalef(1.6, 1.6, 1.2);
    glRotatef(basketball_panel2.rotation.y, 0, 1, 0);
    drawShape(basketball_panel2);
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
    float dx = (float)(x - last_x) / (float)800;
    float dy = (float)(y - last_y) / (float)600;

    camera.angleY -= dx * 2.0f * M_PI;
    camera.position[1] += dy * 10.0f;

    last_x = x;
    last_y = y;

    glutPostRedisplay();
}
static void key(unsigned char key, int x, int y)
{
    keys[key] = true; // Set the state of the pressed key to true
    static int rotSpeedStates = 1;
    switch (key)
    {
    case 27:
    case 'q':
        exit(0);
        break;
    case 'u':
        basketball_panel2.rotation.y += 0.1;
        cout << "y : " << basketball_panel2.rotation.y << endl;
        break;
    case 'i':
        basketball_panel2.rotation.y -= 0.1;
        cout << "y : " << basketball_panel2.rotation.y << endl;
        break;
    case 'p':
        switch (rotSpeedStates)
        {
        case 0:
            rotSpeed = 20;
            break;
        case 1:
            rotSpeed = 70;
            break;
        case 2:
            rotSpeed = 120;
            break;
        case 3:
            rotSpeed = 170;
            break;
        case 4:
            rotSpeed = 220;
        default:
            break;
        }
        rotSpeedStates = ++rotSpeedStates % 5;
        break;
    case 'C':
    case 'c':
        camera_state = ++camera_state % 3;
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
    case GLUT_KEY_UP:
        //basketball_panel1.position.x += 1.0;
        cout << "x " << ++basketball_panel1.position.x << endl;
        break;
    case GLUT_KEY_DOWN:
        cout << "x " << --basketball_panel1.position.x << endl;
        break;
    case GLUT_KEY_RIGHT:
        cout << "z " << ++basketball_panel1.position.z << endl;
        break;
    case GLUT_KEY_LEFT:
        cout << "z " << --basketball_panel1.position.z << endl;
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
    float forward_x = sin(player_body.rotation.y * M_PI / 180.0f);
    float forward_z = -cos(player_body.rotation.y * M_PI / 180.0f);
    // Check the state of each key and take appropriate action
    if (keys['w']) {
        player_body.position.x += forward_x * player_move_speed;
        player_body.position.z -= forward_z * player_move_speed;
    }
    if (keys['s']) {
        player_body.position.x -= forward_x * player_move_speed;
        player_body.position.z += forward_z * player_move_speed;
    }
    if (keys['a']) {
        player_body.rotation.y += player_move_speed + 0.85;
    }
    if (keys['d']) {
        player_body.rotation.y -= player_move_speed + 0.65;
    }

    glutPostRedisplay();
}