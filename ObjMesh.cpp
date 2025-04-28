#include "ObjMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QVector3D>

ObjMesh::ObjMesh(const std::string& filename)
{
    if (!readObjFile(filename))  //If file not read, just make a triangle
    {
        mVertices.push_back(Vertex{ -0.5f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f });
        mVertices.push_back(Vertex{ -0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f });
        mVertices.push_back(Vertex{ 0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f });
        qDebug("Made you a triangle instead...");
    }

    mMatrix.translate(1.f, 0, 0);
}

bool ObjMesh::readObjFile(const std::string& filename)
{

    qDebug() << "Reading " << filename.c_str();
    std::ifstream fileIn;
    fileIn.open(filename, std::ifstream::in);
    if (!fileIn)
    {
        qDebug() << "ERROR: Could not open file for reading: " << filename.c_str();
        return false;
    }
    //One line at a time-variable
    std::string oneLine{};
    //One word at a time-variable
    std::string oneWord{};

    // temp variables to store the data from the file
    std::vector<QVector3D> tempVertecies;
    std::vector<QVector3D> tempNormals;
    std::vector<QVector2D> tempUVs;

    // Varible for constructing the indices vector
    // All obj files use indices
    unsigned int temp_index = 0;

    //Reading one line at a time from file to oneLine
    while (std::getline(fileIn, oneLine))
    {
        //Doing a trick to get one word at a time
        std::stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#")
        {
            //Ignore this line
            //qDebug() << "Line is comment "  << QString::fromStdString(oneWord);
            continue;
        }
        if (oneWord == "")
        {
            //Ignore this line
            //qDebug() << "Line is blank ";
            continue;
        }
        if (oneWord == "v")
        {
            //qDebug() << "Line is vertex "  << QString::fromStdString(oneWord) << " ";
            QVector3D tempVertex;
            sStream >> oneWord;
            tempVertex.setX(std::stof(oneWord));
            sStream >> oneWord;
            tempVertex.setY(std::stof(oneWord));
            sStream >> oneWord;
            tempVertex.setZ(std::stof(oneWord));

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);

            continue;   //jump out of the while loop
        }
        if (oneWord == "vt")
        {
            //qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            QVector2D tempUV;
            sStream >> oneWord;
            tempUV.setX(std::stof(oneWord));
            sStream >> oneWord;
            tempUV.setY(std::stof(oneWord));

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn")
        {
            //qDebug() << "Line is normal "  << QString::fromStdString(oneWord) << " ";
            QVector3D tempNormal;
            sStream >> oneWord;
            tempNormal.setX(std::stof(oneWord));
            sStream >> oneWord;
            tempNormal.setY(std::stof(oneWord));
            sStream >> oneWord;
            tempNormal.setZ(std::stof(oneWord));

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f")
        {
            //qDebug() << "Line is a face "  << QString::fromStdString(oneWord) << " ";
            int index, normal, uv;
            for (int i = 0; i < 3; i++)
            {
                sStream >> oneWord;                             //one word read
                std::stringstream tempWord(oneWord);            //to use getline on this one word
                std::string segment;                            //the numbers in the f-line
                std::vector<std::string> segmentArray;          //temp array of the numbers
                while (std::getline(tempWord, segment, '/'))    //splitting word in segments at /
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]);             //first is vertex
                if (segmentArray[1] != "")                      //second is uv
                    uv = std::stoi(segmentArray[1]);
                else
                {
                    qDebug() << "No UVs in mesh!!!";            //uv not present
                    uv = 0;                                     //this will become -1 in a couple of lines
                }
                normal = std::stoi(segmentArray[2]);            //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1)    //uv present!
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], tempUVs[uv]);
                    mVertices.push_back(tempVert);
                }
                else            //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], QVector2D(0.0f, 0.0f));
                    mVertices.push_back(tempVert);
                }
                //We have now handeled one Vertex on the f-line - add it to indices
                mIndices.push_back(temp_index++);
            }
            continue;
        }
    }
    // beeing a nice boy and closing the file after use
    fileIn.close();
    
    qDebug() << filename.c_str() << " successfully loaded";

    return true;
}

