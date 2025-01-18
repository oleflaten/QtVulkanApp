#include "Mesh.h"

Mesh::Mesh() {
    // Initialize the vertex data
	// Hardcoded mesh for now. Could be loaded from a file or generated procedurally
    vertexData = {
        // Y up, front = CCW
        // X,     Y,     Z,     R,    G,    B
        0.0f,   0.5f,  0.0f,   1.0f, 0.0f, 0.0f,    // top vertex - red
        -0.5f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    // bottom left vertex - green
        0.9f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,    // bottom right vertex - blue

        1.0f,  -0.0f,  0.1f,   0.0f, 1.0f, 1.0f,
        0.0f,  0.8f,  0.1f,   1.0f, 0.0f, 1.0f,
        -0.9f,  0.7f,  0.1f,   1.0f, 1.0f, 1.0f,

		1.0f,  -0.9f,  0.8f,   0.0f, 0.0f, 0.0f    // drawing a line strip, so OK with 7 vertices
    };
	vertexCount = vertexData.size() / 6; // 6 floats per vertex
}

const std::vector<float>& Mesh::getVertexData() const {
    return vertexData;
}
