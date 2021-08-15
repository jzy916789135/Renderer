#pragma once
#include <string>
#include "test_shader.h"
#include "test_camera.h"
#include "GeometryGenerator.h"
#include "TextureLoader.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 tangentU;
	glm::vec2 texC;
};


//AABB
struct AxisAlignedBoundingBox
{
	glm::vec3 bbMin = std::numeric_limits<glm::vec3>::max();//返回可取的有限最大值
	glm::vec3 bbMax = std::numeric_limits<glm::vec3>::min();

	glm::vec3 center;
	glm::vec3 entents;
};
struct MeshGeometry
{
	MeshGeometry() {};
	MeshGeometry(std::string name) : name(name) {};

	std::string name;

	unsigned int vao;
	unsigned int mIndexCount = 0;

	AxisAlignedBoundingBox bounds;

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;


	
	void BuildResources();
	void BuildResources(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	void BuildBoundingBox();
	void Draw(Shader* shader);
	void DrawLines(Shader* shader);
};

struct ModelData
{
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
};

struct ModelMesh
{
	std::string name;
	std::vector<MeshGeometry> subMeshes;

	glm::vec3 position;

	AxisAlignedBoundingBox bounds;

	void BuildResources(std::vector<ModelData> modelData);
};


struct Light
{
	//0 directonL, 1 point , 2 spot
	int type = 0;
	//Commom
	glm::vec3 strength = { 0.1f,0.1f,0.1f };
	glm::vec3 position = { 0.0f,0.0f,0.0f };
	//point and spot
	float falloffStart = 1.0f;
	float falloffEnd = 10.0f;
	//directional and spot
	glm::vec3 focalPoint = { 0.0f,0.0f,0.0f };
	//spot
	float spotPower = 64.0f;
};

struct Material
{
	std::string name;

	glm::vec3 albedo = glm::vec3(0.1);//正入射时的反射率
	float metallic = 0.1;//金属表面系数
	float roughness = 0.1;//粗糙度
	float ambientOcclusion = 1.f;
	glm::vec3 emmisive = glm::vec3(0.0, 0.0, 0.0);//光源颜色

	bool useDiffuse = false;
	bool useNormal = false;

	unsigned int diffuseID = -1;
	unsigned int normalID = -1;
	// 3D texture
	unsigned int densityID = -1;
};
struct Texture
{
	std::string name;
	std::string path;
	std::vector<std::string> cubepath;

	unsigned int textureID;
	void BuildResource(std::string fileName);
	void BuildResource(std::vector<std::string> cubeFilenames);
};
struct PassCb
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.f);
	glm::vec3 eyePos = glm::vec3(1.f);
	glm::vec4 ambientLight = glm::vec4(glm::vec3(0.01), 1.f);
	int lightNum = 0;
};
struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	std::string name = "";

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 world = glm::mat4(1.0f);

	MeshGeometry* geo = nullptr;
	Material* mat = nullptr;

	int textureScale = 1;

	bool bCulled = false;

	void  UpdateWorld();

	void Draw(Shader* shader);
	void DrawLines(Shader* shader);

};

