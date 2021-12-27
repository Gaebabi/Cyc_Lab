#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "AssimpLoader.h"


AssimpLoader::AssimpLoader(ModelData* modelData, std::string path)
{
    AssimpLoader::loadModel(modelData, path);
}
void AssimpLoader::loadModel(ModelData* modelData, std::string path)
{
    //unsigned int importOptions = aiProcess_Triangulate | aiProcess_FlipUVs;
    unsigned int importOptions = aiProcess_Triangulate
        | aiProcess_OptimizeMeshes
        | aiProcess_JoinIdenticalVertices
        | aiProcess_CalcTangentSpace
        | aiProcess_FlipUVs;

    this->modelData = modelData;
    modelData->scene = import.ReadFile(path, importOptions);// aiProcess_Triangulate | aiProcess_FlipUVs);
    scene = modelData->scene;

    ///////////////////////////////////////////////////////// 여기선 읽히는데
    //unsigned int numPosKeys = modelData->scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    modelData->directory = path.substr(0, path.find_last_of('/'));

    /*if (scene->HasAnimations()) {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        {
            animations->push_back(scene->mAnimations[i]);
        }
    }*/
    loadBones(scene->mRootNode, scene);
    modelData->m_BoneInfo.resize(modelData->Bone_Mapping.size());

    processNode(scene->mRootNode, scene);
}

void AssimpLoader::processNode(aiNode* node, const aiScene* scene)
{
    // 노드의 모든 mesh들을 처리(만약 있다면)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        modelData->total_vertices += mesh->mNumVertices;
        modelData->meshes.push_back(processMesh(mesh, scene));
    }
    // 그런 다음 각 자식들에게도 동일하게 적용
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        AssimpLoader::processNode(node->mChildren[i], scene);
    }
}

Mesh AssimpLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    modelData->Bones.resize(modelData->total_vertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // vertex 위치, 법선, 텍스처 좌표를 처리
        glm::vec3 vector;
        // vertex 위치
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // 법선
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // 텍스처 좌표
        if (mesh->mTextureCoords[0]) // mesh가 텍스처 좌표를 가지고 있는가?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        
        vertices.push_back(vertex);
    }
    // indices 처리
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // material 처리
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    loadMeshBones(mesh, modelData->Bones, scene);
    return Mesh::Mesh(vertices, indices, modelData->m_BoneInfo, modelData->Bones, textures);
}

void AssimpLoader::loadBones(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        std::string NodeName(node->mChildren[i]->mName.data);
        if (NodeName.find(":") != std::string::npos) {
            std::string BoneName = NodeName;
            unsigned int BoneIndex = 0;

            if (modelData->Bone_Mapping.find(BoneName) == modelData->Bone_Mapping.end()) {
                BoneIndex = modelData->m_NumBones;
                modelData->m_NumBones++;
                modelData->Bone_Mapping[BoneName] = BoneIndex;
            }
        }
        if (NodeName != "Body" && NodeName != "metarig" && NodeName != "parasiteZombie" && NodeName != "Armature" && NodeName != "MutantMesh" && NodeName != "Cylinder") {
            std::string BoneName = NodeName;
            unsigned int BoneIndex = 0;

            if (modelData->Bone_Mapping.find(BoneName) == modelData->Bone_Mapping.end()) {
                BoneIndex = modelData->m_NumBones;
                modelData->m_NumBones++;
                modelData->Bone_Mapping[BoneName] = BoneIndex;
            }
        }
        //only uncomment if we need to load cylinder model
    /*else {
    string BoneName(node->mChildren[i]->mName.data);
    unsigned int BoneIndex = 0;
    if (NodeName != "parasiteZombie" || NodeName != "Armature") {
        if (Bone_Mapping.find(BoneName) == Bone_Mapping.end()) {
            BoneIndex = m_NumBones;
            m_NumBones++;
            Bone_Mapping[BoneName] = BoneIndex;
        }
    }

}*/
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        loadBones(node->mChildren[i], scene);
}

void AssimpLoader::loadMeshBones(aiMesh* mesh, std::vector<VertexBoneData>& VertexBoneData, const aiScene* scene)
{
    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        unsigned int BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);

        std::cout << BoneName << std::endl;


        if (modelData->Bone_Mapping.find(BoneName) != modelData->Bone_Mapping.end())
        {
            BoneIndex = modelData->Bone_Mapping[BoneName];
            //BoneInfo bi;
            //m_BoneInfo.push_back(bi);

            aiMatrix4x4 tp1 = mesh->mBones[i]->mOffsetMatrix;
            modelData->m_BoneInfo[BoneIndex].offset = glm::transpose(glm::make_mat4(&tp1.a1));
        }
        else {
            std::cout << "error" << std::endl;
        }

        int nn = mesh->mBones[i]->mNumWeights;
        for (unsigned int n = 0; n < nn; n++) {
            unsigned int vid = mesh->mBones[i]->mWeights[n].mVertexId + modelData->NumVertices;//absolute index
            float weight = mesh->mBones[i]->mWeights[n].mWeight;
            VertexBoneData[vid].AddBoneData(BoneIndex, weight);
        }
        loadAnimations(scene, BoneName, modelData->Animations);
    }
    modelData->NumVertices += mesh->mNumVertices;
}
void AssimpLoader::loadAnimations(const aiScene* scene, std::string BoneName, std::map<std::string, std::map<std::string, const aiNodeAnim*>>& animations)
{
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        const aiAnimation* pAnimation = scene->mAnimations[i];
        std::string animName = pAnimation->mName.data;
        for (unsigned int j = 0; j < pAnimation->mNumChannels; j++) {
            std::string name = pAnimation->mChannels[j]->mNodeName.data;
            if (name == BoneName) {

                animations[animName][BoneName] = pAnimation->mChannels[j];
                break;
            }
            /*if (subname == BoneName) {
                std::cout << "Anim= " << subname << std::endl;
                animations[animName][BoneName] = pAnimation->mChannels[j];
                break;
            }*/
        }
    }
}


unsigned int AssimpLoader::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = changePath((char*)path);
    filename = directory + '/' + filename;

    
    // OpenGL 텍스처 생성
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, STBI_rgb_alpha);//_alpha
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // 새롭게 생성된 텍스처를 "Bind"합니다. : 이제 앞으로 모든 Texutre 관련 함수는 이 친구를 건듭니다. 
        glBindTexture(GL_TEXTURE_2D, textureID);

        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // 파일을 읽고, 매개 변수로 glTexImage2D를 호출
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 괜찮은 3중 필터링. 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
        //std::cout << filename << std::endl;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
std::vector<Texture> AssimpLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures_loaded;
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // 텍스처가 이미 불러와져있지 않다면 불러옵니다.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), modelData->directory, true);
            texture.type = typeName;
            texture.path = changePath((char*)str.C_Str()).c_str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // 불러온 텍스처를 삽입합니다.
        }
    }
    return textures;
}
std::string AssimpLoader::changePath(char* path) {
    int index = -1;
    std::string newPath;
    for (int i = 0; i < 200; i++)
    {
        if (path[i] != NULL) {
            if (path[i] == '\\') {
                index = i;
            }
        }
        else {
            break;
        }
    }
    for (int i = index+1; i < 200; i++)
    {
        if (path[i] != NULL) {
            newPath += path[i];
        }
        else {
            break;
        }
    }
    std::cout << path << std::endl;
    std::cout << "changed path: " + newPath << std::endl;
    return newPath;
}