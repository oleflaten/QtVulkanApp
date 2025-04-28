#include "HeightMap.h"
#include "Vertex.h"
#include "stb_image.h"

HeightMap::HeightMap()
{ }

void HeightMap::makeTerrain(std::string heightMapImage)
{
	//Load the heightmap image
	//Using stb_image to load the image
	stbi_uc* pixelData = stbi_load(heightMapImage.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
	if (pixelData == nullptr)
    {
	    qDebug() << "Failed to load heightmap image!";
	    return;
	}
	//Make the terrain from the pixel data
	makeTerrain(pixelData, mWidth, mHeight);
	stbi_image_free(pixelData);
}

//Function that makes a terrain grid from a heightmap, using the values in the heightmap as height.
//This function will crash if the width and height of the heightmap is not set correct!
//The size of the textureData array is widthIn * heightIn.
// The function is not tested in this codebase, and is provided as an example.
void HeightMap::makeTerrain(unsigned char* textureData, int widthIn, int heightIn)
{
    //Default normal pointing straight up - should be calculated correctly for lights to work!!!
    float normal[3]{0.f, 1.f, 0.f};

    //How many meters(units) between each vertex in both x and z direction
    //This should be sent in as a parameter!
    float horisontalSpacing{.2f};

    //Scaling the height read from the heightmap. 0 -> 255 meters(units) if this is set to 1
    //This should be sent in as a parameter!
    float heightSpacing{.02f};

    //Offset the whole terrain in y (height) axis
    //Moves the terrain mesh up or down
    //Because of Barycentric calculations, we want the terrain to be in World coordinates!
	//So we don't want to move the terrain up or down in the Y axis after it is made
    float heightPlacement{-5.f};

    //Getting the scale of the heightmap
    //Using depth as the name of texture height, to not confuse with terrain height
    unsigned short width = widthIn;       //Width == x-axis
    unsigned short depth = heightIn;      //Depth == z-axis

    //Temp variables for creating the mesh
    //Adding offset so the middle of the terrain will be in World origo
    float vertexXStart{ 0.f - width * horisontalSpacing / 2 };            // if world origo should be at center use: {0.f - width * horisontalSpacing / 2};
    float vertexZStart{ 0.f + depth * horisontalSpacing / 2 };            // if world origo should be at center use: {0.f + depth * horisontalSpacing / 2};

    //Loop to make the mesh from the values read from the heightmap (textureData)
	//Double for-loop to make the depth and the width of the terrain in one go
    for(int d{0}; d < depth; ++d)       //depth loop
    {
        for(int w{0}; w < width; ++w)   //width loop
        {
            //Heightmap image is actually stored as an one dimentional array - so calculating the correct index for column and row
            //and scale it according to variables
            // Calculate the correct index for the R value of each pixel
            int index = (w + d * width) * 4; // Each pixel has 4 bytes (RGBA)
            float heightFromBitmap = static_cast<float>(textureData[index]) * heightSpacing + heightPlacement;
			//                                          x - value                      y-value               z-value
            mVertices.emplace_back(Vertex{vertexXStart + (w * horisontalSpacing), heightFromBitmap, vertexZStart - (d * horisontalSpacing),
				//  dummy normal=0,1,0                  Texture coordinates
                normal[0],normal[1],normal[2],           w / (width - 1.f), d / (depth - 1.f)});
        }
    }

    // The mesh(grid) is drawn in quads with diagonals from lower left to upper right
    //          _ _
    //         |/|/|
    //          - -
    //         |/|/|
    //          - -
    //Making the indices for this mesh:
    for(int d{0}; d < depth-1; ++d)        //depth - 1 because we draw the last quad from depth - 1 and in negative z direction
    {
        for(int w{0}; w < width-1; ++w)    //width - 1 because we draw the last quad from width - 1 and in positive x direction
        {
			//Indices for one quad:
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(w + d * width + width);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }

	//Calculating the normals for the mesh
    //Function not made yet:
    //calculateHeighMapNormals();
}
