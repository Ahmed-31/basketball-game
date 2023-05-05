#define M_PI 3.14159265358979323846
#include <iostream>
#include <windows.h>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <cstdlib>
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
}player_body, player2, basketball_panel, maleBody;

static int rotSpeed = 20;
GLfloat cameraPosition[] = { 0.0f, 7.5f, 1.0f };
GLfloat cameraDistance = 25.0;
GLfloat cameraAngleX = 0.0f;
GLfloat cameraAngleY = 100.0f;
GLfloat z_movement = -4.0;
GLfloat x_movement = 0.0;
GLfloat y_mov = 4.557;
bool keys[256] = { false }; // Array to store the state of each key
bool pause = false;
int last_x, last_y;

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    const float ar = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ar, 2.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
static void drawShape(Model model) {
    glBegin(GL_TRIANGLES);
    static bool firstCall = true;
    int i = 1;
    for (const auto& faceVertices : model.faces) {
        for (const auto& faceVertex : faceVertices) {
            if (faceVertex.normalIndex != -1 && faceVertex.normalIndex < model.normals.size()) {
                glNormal3f(model.normals[faceVertex.normalIndex].x, model.normals[faceVertex.normalIndex].y, model.normals[faceVertex.normalIndex].z);
            }
            if (faceVertex.textureIndex != -1 && faceVertex.textureIndex < model.textureCoords.size()) {
                glTexCoord2f(model.textureCoords[faceVertex.textureIndex].u, model.textureCoords[faceVertex.textureIndex].v);
            }
            if (faceVertex.vertexIndex < model.vertices.size()) {
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
//static void drawShape(Model model) {
//    glBegin(GL_TRIANGLES);
//    static bool firstCall = true;
//    int i = 1;
//    for (const auto& faceVertices : model.faces) {
//        for (const auto& faceVertex : faceVertices) {
//            if(faceVertex.normalIndex != -1)
//                glNormal3f(model.normals[faceVertex.normalIndex].x, model.normals[faceVertex.normalIndex].y, model.normals[faceVertex.normalIndex].z);
//            if(faceVertex.textureIndex != -1)
//                glTexCoord2f(model.textureCoords[faceVertex.textureIndex].u, model.textureCoords[faceVertex.textureIndex].v);
//            glVertex3f(model.vertices[faceVertex.vertexIndex].x, model.vertices[faceVertex.vertexIndex].y, model.vertices[faceVertex.vertexIndex].z);
//        }
//         //Add two degenerate triangles to connect the strips
//        glNormal3f(model.normals[faceVertices.back().normalIndex].x, model.normals[faceVertices.back().normalIndex].y, model.normals[faceVertices.back().normalIndex].z);
//        glTexCoord2f(model.textureCoords[faceVertices.back().textureIndex].u, model.textureCoords[faceVertices.back().textureIndex].v);
//        glVertex3f(model.vertices[faceVertices.back().vertexIndex].x, model.vertices[faceVertices.back().vertexIndex].y, model.vertices[faceVertices.back().vertexIndex].z);
//
//        glNormal3f(model.normals[faceVertices.back().normalIndex].x, model.normals[faceVertices.back().normalIndex].y, model.normals[faceVertices.back().normalIndex].z);
//        glTexCoord2f(model.textureCoords[faceVertices.back().textureIndex].u, model.textureCoords[faceVertices.back().textureIndex].v);
//        glVertex3f(model.vertices[faceVertices.back().vertexIndex].x, model.vertices[faceVertices.back().vertexIndex].y, model.vertices[faceVertices.back().vertexIndex].z);
//
//        glNormal3f(model.normals[faceVertices.front().normalIndex].x, model.normals[faceVertices.front().normalIndex].y, model.normals[faceVertices.front().normalIndex].z);
//        glTexCoord2f(model.textureCoords[faceVertices.front().textureIndex].u, model.textureCoords[faceVertices.front().textureIndex].v);
//        glVertex3f(model.vertices[faceVertices.front().vertexIndex].x, model.vertices[faceVertices.front().vertexIndex].y, model.vertices[faceVertices.front().vertexIndex].z);
//        if (firstCall) {
//            cout << "progress drawing " << (i++ * 100 / model.faces.size()) << "%" << "\r";
//        }
//    }
//
//    firstCall = false;
//    glEnd();
//}
static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t * rotSpeed;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up camera
    cameraPosition[0] = cameraDistance * sin(cameraAngleY);
    cameraPosition[2] = cameraDistance * cos(cameraAngleY);
    GLfloat up[3] = { 0.0f, 1.0f, 0.0f };
    if (cameraAngleX < 0.0f) {
        up[1] = -1.0f;
    }

    // Reset the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply the camera transformation to the modelview matrix
    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
        0.0, 7.0, -4.0, up[0], up[1], up[2]);

    glTranslatef(0.0, 0.0, 0.0);
    glPushMatrix();
    // draw the basketball court
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.6, 0.0); // set the color to green
    glVertex3f(-25.0, 0.0, -50.0);
    glVertex3f(-25.0, 0.0, 50.0);
    glVertex3f(25.0, 0.0, 50.0);
    glVertex3f(25.0, 0.0, -50.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(x_movement, y_mov, z_movement);
    glColor3f(1.0, 0.0, 0.0);
    glRotated(a, 0, 1, 0);
    drawShape(player_body);
    glPopMatrix();

    glPushMatrix();
    glTranslated(x_movement + 10, 0.0, z_movement);
    glColor3f(1.0, 0.0, 0.0);
    glRotated(a, 0, 1, 0);
    drawShape(basketball_panel);
    glPopMatrix();

    //glPushMatrix();
    //glTranslated(x_movement + 15, 0.0, z_movement + 5);
    //glColor3f(1.0, 0.0, 0.0);
    //glRotated(-a, 0, 1, 0);

    //
    ////drawShape(player2);
    //glPopMatrix();
    glutSwapBuffers();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }
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

    cameraAngleY -= dx * 2.0f * M_PI;
    cameraPosition[1] += dy * 10.0f;

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
        y_mov += 0.01;
        cout << "y : " << y_mov << endl;
        break;
    case 'i':
        y_mov -= 0.01;
        cout << "y : " << y_mov << endl;
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
    }
    

    glutPostRedisplay();
}
static void keyUp(unsigned char key, int x, int y)
{
    keys[key] = false; // Set the state of the released key to false
}
static void idle(void)
{
    // Check the state of each key and take appropriate action
    if (keys['w']) {
        z_movement -= 0.05;
    }
    if (keys['s']) {
        z_movement += 0.05;
    }
    if (keys['a']) {
        x_movement -= 0.05;
    }
    if (keys['d']) {
        x_movement += 0.05;
    }
    if (keys['m']) {
        cameraAngleY += 0.01;
    }
    if (keys['n']) {
        cameraAngleY -= 0.01;
    }
    glutPostRedisplay();
}
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
/* Program entry point */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("BasketBall Game");
    player_body.loadObjFile("Models/low_poly_body.obj");
    basketball_panel.loadObjFile("Models/basketBall panel_me.obj");
    //maleBody.loadObjFile("Models/male_body.obj");
    //loadObjFile("Models/box.obj");
    //player2.loadObjFile("Models/male_body.obj");
    //player2.loadObjFile("Models/Orange.obj");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutMouseFunc(mouse); // set the mouse function
    glutMotionFunc(motion); // set the motion function
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
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
    glutMainLoop();
    return 0;
}