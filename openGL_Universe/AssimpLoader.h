#pragma once

#include "Loader.h"
#include <glm/glm.hpp> // 순서 상관 없음
#include <glm/gtc/type_ptr.hpp> // make_mat4()

#include <vector>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Interval.h"

struct ModelData {
    /*  Model 데이터  */
    const aiScene* scene;
    std::string directory;
    unsigned int m_NumBones = 0;
    unsigned int NumVertices = 0;
    unsigned int total_vertices = 0;
    std::vector<Mesh> meshes;
    std::vector<VertexBoneData> Bones;
    std::map<std::string, unsigned int> Bone_Mapping;
    std::vector<BoneInfo> m_BoneInfo;
    std::map<std::string, std::map<std::string, const aiNodeAnim*>> Animations;
    glm::fdualquat IdentityDQ = glm::fdualquat(glm::quat(1.f, 0.f, 0.f, 0.f), glm::quat(0.f, 0.f, 0.f, 0.f));
    std::vector<Interval> m_constraints;
    std::map<unsigned int, glm::vec3> skeleton_pose;

    bool animationMatricesExists[2500] = { false };
    std::vector<glm::mat2x4> animationMatrices[2500];
};
/*struct Texture {
    unsigned int id;
    std::string type;
    std::string path;  // 다른 텍스처와 비교하기 위해 텍스처의 경로를 저장
};*/

class AssimpLoader
{
public:
    AssimpLoader(ModelData* modelData, std::string path);
private:
    Assimp::Importer import;
    const aiScene* scene;
    //std::string directory;
    /*  Scene 데이터  */
    ModelData* modelData;

    /*  함수  */
    void loadModel(ModelData* modelData, std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void loadBones(aiNode* node, const aiScene* scene);
    void loadMeshBones(aiMesh* mesh, std::vector<VertexBoneData>& VertexBoneData, const aiScene* scene);
    void loadAnimations(const aiScene* scene, std::string BoneName, 
        std::map<std::string, std::map<std::string, const aiNodeAnim*>>& animations);

    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    std::string changePath(char* path);
};