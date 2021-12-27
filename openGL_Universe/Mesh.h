#pragma once

#include <glm/glm.hpp> // 순서 상관 없음
#include "Loader.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include <vector>

#define NUM_BONES_PER_VERTEX 4
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_MATERIAL 0xFFFFFFFF

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};
/*struct Bone
{
    glm::mat4 boneOffset;
    glm::mat4 finalTransform;
};*/

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    //float boneWeights[2][4] = { 0.0f };
    //float boneIds[2][4] = { 0.0f };
};
struct BoneInfo {
    glm::mat4 offset;
    glm::mat4 FinalTransformation;
    glm::fdualquat FinalTransDQ;
    /*  함수         */
    BoneInfo();
};
struct VertexBoneData {
    unsigned int BoneIDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];
    unsigned int NumVertices = 0;
    /*  함수         */
    VertexBoneData();
    void Reset();
    void AddBoneData(unsigned int BoneID, float Weight);
};
class Mesh {
public:
    /*  Mesh 데이터  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    /*  Bone 데이터  */
	std::vector<BoneInfo> bones;
	std::vector<VertexBoneData> vertexBoneData;
    //std::vector<Bone> bones;
    unsigned int texture;

    /*  함수         */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<BoneInfo> bones, std::vector<VertexBoneData> vertexBoneData, std::vector<Texture> textures);
    void Draw(ShaderProgram* shader);
    void Draw(ShaderProgram* shaderProgram, int num);
    //void Draw();

private:
    /*  렌더 데이터  */
    unsigned int VAO, VBO, EBO, vertexBones_vbo;
    /*  함수         */
    void setupMesh();
};