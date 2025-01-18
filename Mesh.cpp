#include "Mesh.h"

Mesh::Mesh() {
    // Initialize the vertex data
	// Hardcoded mesh for now. Could be loaded from a file or generated procedurally
    vertexData = {
        // Y up, front = CCW
        // X,     Y,     Z,     R,    G,    B
        0.0f,   0.5f,  0.0f,   1.0f, 0.0f, 0.0f,    // top vertex - red
        -0.5f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    // bottom left vertex - green
        0.5f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f     // bottom right vertex - blue
    };
	vertexCount = vertexData.size() / 6;
}

const std::vector<float>& Mesh::getVertexData() const {
    return vertexData;
}
