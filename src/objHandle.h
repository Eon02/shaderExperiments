// //
// // Created by haley on 10/8/25.
// //
// using namespace std;


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Vec2 {
    float u, v;
    Vec2(float u = 0, float v = 0) : u(u), v(v) {}
};

class OBJParser {
private:
    vector<Vec3> positions;      
    vector<Vec3> normals;        
    vector<float> vertices;
    vector<Vec2> texCoords;

public:
    // parse based on // delimiter
    bool parse(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return false;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            stringstream ss(line);
            string prefix;
            ss >> prefix;

            if (prefix == "v") {
                 // v x y z
                float x, y, z;
                ss >> x >> y >> z;
                positions.push_back(Vec3(x, y, z));
            }
            else if (prefix == "vt") {
                // vt u v
                float u, v;
                ss >> u >> v;
                texCoords.push_back(Vec2(u, v));
            }
            else if (prefix == "vn") {
                // vn x y z
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back(Vec3(x, y, z));
            }
            else if (prefix == "f") {
                // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                parseFace(ss);
            }
        }

        file.close();
        return true;
    }

    
    const vector<float>& getVertices() const {
        return vertices;
    }

    
    int getVertexCount() const {
        return vertices.size() / 6;  // 6 floats per vertex (3 pos + 3 normal)
    }

    void printStats() const {
        cout << "Loaded " << positions.size() << " positions" << endl;
        cout << "Loaded " << texCoords.size() << " texture coordinates" << endl;
        cout << "Loaded " << normals.size() << " normals" << endl;
        cout << "Generated " << getVertexCount() << " vertices for rendering" << endl;
    }

private:
    void parseFace(stringstream& ss) {
        string vertexData;
        vector<int> faceVertexIndices;
        vector<int> faceNormalIndices;
        vector<int> faceVertexColors;
        vector<int> faceTexCoordIndices;

        
        while (ss >> vertexData) {
            int v = -1, vt = -1, vn = -1;

            // Parse v/vt/vn or v//vn or v/vt or v
            size_t slash1 = vertexData.find('/');
            if (slash1 != string::npos) {
                v = stoi(vertexData.substr(0, slash1)) - 1;

                size_t slash2 = vertexData.find('/', slash1 + 1);
                if (slash2 != string::npos) {
                    // Format: v/vt/vn or v//vn
                    if (slash2 != slash1 + 1) {
                        vt = stoi(vertexData.substr(slash1 + 1, slash2 - slash1 - 1)) - 1;
                    }
                    vn = stoi(vertexData.substr(slash2 + 1)) - 1;
                } else {
                    // Format: v/vt
                    vt = stoi(vertexData.substr(slash1 + 1)) - 1;
                }
            } else {
                // Format: v
                v = stoi(vertexData) - 1;
            }

            faceVertexIndices.push_back(v);
            faceTexCoordIndices.push_back(vt);
            faceNormalIndices.push_back(vn);
        }

        // Triangulation of non-triangle faces because omg all of these files have funky faces
        //  https://stackoverflow.com/questions/23723993/converting-quadriladerals-in-an-obj-file-into-triangles
        // thank you again stackoverflow...
        for (size_t i = 1; i + 1 < faceVertexIndices.size(); i++) {
            addVertex(faceVertexIndices[0], faceTexCoordIndices[0], faceNormalIndices[0]);
            addVertex(faceVertexIndices[i], faceTexCoordIndices[i],faceNormalIndices[i]);
            addVertex(faceVertexIndices[i + 1], faceTexCoordIndices[i+1], faceNormalIndices[i + 1]);
        }
    }

    void addVertex(int pos, int tex, int norm) {
        // Add position
        if (pos >= 0 && pos < positions.size()) {
            vertices.push_back(positions[pos].x);
            vertices.push_back(positions[pos].y);
            vertices.push_back(positions[pos].z);
          } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        // Add normal
        if (norm >= 0 && norm < normals.size()) {
            vertices.push_back(normals[norm].x);
            vertices.push_back(normals[norm].y);
            vertices.push_back(normals[norm].z);

        }
        else {
            // Default normal if not provided
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
        }
        if (tex >= 0 && tex < texCoords.size()) {
            vertices.push_back(texCoords[tex].u);
            vertices.push_back(texCoords[tex].v);
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.f);

        }

        // Color of vertice is determined by its index in the model
        // results in a cool ghost-ish look =)
        // float t = 1;
        // vertices.push_back(t - (0.00009*pos));
        // vertices.push_back(t - (0.00005*pos));
        // vertices.push_back(t - (0.00001*pos));
    }
};

