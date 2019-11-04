#ifndef THIS_MODEL_H
#define THIS_MODEL_H
#this file comes from learnopengl I just code it by myself and modified the texture part and some variable name.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Mesh.h"
#include <string>
#include "stb_image.h"
using namespace std;
class Model {
	public:
		vector<Mesh> Meshes;
		Model(const string path)
		{

			loadModel(path);
		}
		void Draw(Shader shader) {
			for(int i = 0;i < Meshes.size();i++)
				Meshes[i].Draw(shader);
		}	
		string directory;
		void loadModel(const string path)
		{
			
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
			if (!scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene) {
				cout << "LOAD MODEL ERROR: " << importer.GetErrorString() << endl;
				return;
			}
			directory = path.substr(0, path.find_last_of('/'));
			processNode(scene->mRootNode, scene);
			
		}
		void processNode(aiNode *Node, const aiScene *scene)
		{
			
			for (int i = 0; i < Node->mNumMeshes; i++) {
				aiMesh *mesh;
				mesh = scene->mMeshes[Node->mMeshes[i]];
				Meshes.push_back(processMesh(mesh, scene));
			}
			for (int i = 0; i < Node->mNumChildren; i++) {
				processNode(Node->mChildren[i], scene);
			}
		}
		Mesh processMesh(aiMesh *mesh, const aiScene *scene)
		{
			vector <Vertex> vertices;
			vector <unsigned int > indices;
			vector <Texture> textures;
			vector < vec3 > colors;
			for (int i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				vertex.Position.x = mesh->mVertices[i].x;
				vertex.Position.y = mesh->mVertices[i].y;
				vertex.Position.z = mesh->mVertices[i].z;
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
				if (mesh->mTextureCoords[0]) {
					vertex.Coords.s = mesh->mTextureCoords[0][i].x;
					vertex.Coords.t = mesh->mTextureCoords[0][i].y;
				}
				else
					vertex.Coords = vec2(0.0);
				vertices.push_back(vertex);
			}
			for (int i = 0; i < mesh->mNumFaces; i++) {
				for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
					indices.push_back(mesh->mFaces[i].mIndices[j]);
				}
			}
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial *material;
				material = scene->mMaterials[mesh->mMaterialIndex];
			
				vector<Texture> diffusemaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			
				textures.insert(textures.end(), diffusemaps.begin(), diffusemaps.end());
				vector<Texture> specularmaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularmaps.begin(), specularmaps.end());
				colors=  loadDiffColor(material);
			}
			return Mesh(vertices, indices, textures,colors);
		}
		vector<vec3> loadDiffColor(aiMaterial *matrl)
		{
			vector<vec3> colors;
			aiColor3D color;
			matrl->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			colors.push_back(vec3(color.r, color.g, color.b));
			return colors;
		}
		vector<Texture> loadMaterialTextures(aiMaterial *matrl, aiTextureType texturtp, string typena)
		{
			vector<Texture> textures;
			for (int i = 0; i < matrl->GetTextureCount(texturtp); i++) {
				aiString str;
				matrl->GetTexture(texturtp, i, &str);
				Texture texture;
				texture.type = typena;
				{
					int width, height, channels;
					//unsigned char *data = stbi_load(     (  directory + '/' + string(str.C_Str())  ).c_str(), &width, &height, &channels, 3);
					unsigned char *data = stbi_load(string(str.C_Str()).c_str(), &width, &height, &channels, 3);
					unsigned int textur;
					glGenTextures(1, &textur);
					glBindTexture(GL_TEXTURE_2D, textur);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glBindTexture(GL_TEXTURE_2D, 0);
					texture.id = textur;
					stbi_image_free(data);
				}
				textures.push_back(texture);
			}
			return textures;
		}
};
#endif 