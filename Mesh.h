#ifndef MESH_H
#define MESH_H

#include <vector>

//Simple class to hold the vertex data for a mesh, nothing else for now
class Mesh {
public:
    Mesh();
    const std::vector<float>& getVertexData() const;
	int size() const { return vertexCount; }

private:
    std::vector<float> vertexData;
	int vertexCount;
};

#endif // MESH_H
