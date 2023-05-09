#include <iostream>
#include <windows.h>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <GL/GLM/glm.hpp>
#include <GL/GLM/gtx/euler_angles.hpp>
#include <GL/GLM/gtx/quaternion.hpp>
#include <GL/glut.h>

using namespace std;
using namespace glm;

#pragma once
class Model {
private:
    struct FaceVertex {
        int vertexIndex, normalIndex, textureIndex;
    };
    struct Face {
        vector<FaceVertex> _3vertices;
        Face() : _3vertices(3) {};
    };

    vector<vec3> vPositions;
    vector<vec3> normals;
    vector<vec2> textures;
    vector<Face> faces;

    void parseVertex(const string& line) {
        vec3 vertex{};
        istringstream iss(line.substr(2));
        iss >> vertex.x >> vertex.y >> vertex.z;
        vPositions.push_back(vertex);
    }
    void parseNormal(const string& line) {
        vec3 vertex{};
        istringstream iss(line.substr(2));
        iss >> vertex.x >> vertex.y >> vertex.z;
        normals.push_back(vertex);
    }
    void parseTextureCoord(const string& line) {
        vec2 vertex{};
        istringstream iss(line.substr(3));
        iss >> vertex.x >> vertex.y;
        textures.push_back(vertex);
    }
    void parseFace(const string& line) {
        Face faceVertices{};
        istringstream iss(line.substr(2));
        string token;
        int i = 0;
        while (getline(iss, token, ' ')) {
            istringstream viss(token);
            string vertexIndexStr, textureIndexStr, normalIndexStr;
            getline(viss, vertexIndexStr, '/');
            getline(viss, textureIndexStr, '/');
            getline(viss, normalIndexStr, '/');
            int vertexIndex = stoi(vertexIndexStr) - 1;
            int textureIndex = textureIndexStr.empty() ? -1 : stoi(textureIndexStr) - 1;
            int normalIndex = normalIndexStr.empty() ? -1 : stoi(normalIndexStr) - 1;
            faceVertices._3vertices[i].vertexIndex = vertexIndex;
            faceVertices._3vertices[i].textureIndex = textureIndex;
            faceVertices._3vertices[i++].normalIndex = normalIndex;
        }
        faces.push_back(faceVertices);
    }
    int countLines(const std::string& filename) {
        std::ifstream file(filename);
        int count = 0;
        char buffer[4096];

        while (file.read(buffer, sizeof(buffer))) {
            count += std::count(buffer, buffer + file.gcount(), '\n');
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
    void showData() {
        for (const auto& vertex : vPositions) {
            cout << "v " << vertex.x << ' ' << vertex.y << ' ' << vertex.z << endl;
        }
        for (const auto& face : faces) {
            for (const auto& faceVertex : face._3vertices) {
                cout << "f " << faceVertex.vertexIndex << '/' << faceVertex.textureIndex << '/' << faceVertex.normalIndex << ' ';
            }
            cout << endl;
        }
    }
public:
    struct ModelTransform {
        vec3 position = vec3(0.0f, 0.0f, 0.0f);
        vec3 rotation = vec3(0.0f, 0.0f, 0.0f);
        vec3 scale = vec3(1.0f, 1.0f, 1.0f);
    }transform;
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
                parseVertex(line);
            }
            else if (line.substr(0, 2) == "vn") {
                parseNormal(line);
            }
            else if (line.substr(0, 2) == "vt") {
                parseTextureCoord(line);
            }
            else if (line.substr(0, 2) == "f ") {
                parseFace(line);
            }
        }
        cout << "progress loading obj 100%" << endl;
    }
    void drawShape() {
        if (faces.empty()) {
            cout << "error: no faces found to draw." << endl;
            return;
        }
        glBegin(GL_TRIANGLES);
        static bool firstCall = true;
        int i = 1;
        for (const auto& face : faces) {
            for (const auto& faceVertex : face._3vertices) {
                if (faceVertex.normalIndex != -1 && (unsigned)faceVertex.normalIndex < normals.size()) {
                    glNormal3f(normals[faceVertex.normalIndex].x, normals[faceVertex.normalIndex].y, normals[faceVertex.normalIndex].z);
                }
                if (faceVertex.textureIndex != -1 && (unsigned)faceVertex.textureIndex < textures.size()) {
                    glTexCoord2f(textures[faceVertex.textureIndex].x, textures[faceVertex.textureIndex].y);
                }
                if ((unsigned)faceVertex.vertexIndex < vPositions.size()) {
                    glVertex3f(vPositions[faceVertex.vertexIndex].x, vPositions[faceVertex.vertexIndex].y, vPositions[faceVertex.vertexIndex].z);
                }
            }
            if (firstCall) {
                cout << "progress drawing " << (i++ * 100 / faces.size()) << "%" << "\r";
            }
        }
        firstCall = false;
        glEnd();
    }
};
