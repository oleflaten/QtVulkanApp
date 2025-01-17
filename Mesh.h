#ifndef MESH_H
#define MESH_H

#include <vector>

//Simple class to hold the vertex data for a mesh, nothing else for now
class Mesh {
public:
    Mesh();
    const std::vector<float>& getVertexData() const;

private:
    std::vector<float> vertexData;
};

#endif // MESH_H
