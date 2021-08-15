#include "Utils.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void MeshGeometry::BuildResources()
{
	glGenVertexArrays(1, &vao);
	//Create vbo, ebo
	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Vertex::normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::tangentU));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texC));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	//½â°óvao vbo ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mIndexCount = mIndices.size();

}

void MeshGeometry::BuildResources(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	glGenVertexArrays(1, &vao);
	//Create vbo, ebo
	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::tangentU));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texC));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//½â°óvao vbo ebo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mVertices = vertices;
	mIndices = indices;

	mIndexCount = mIndices.size();
	BuildBoundingBox();
}

void MeshGeometry::BuildBoundingBox()
{
	for (int i = 0; i < mVertices.size(); i++)
	{
		glm::vec3 v = mVertices[i].pos;
		bounds.bbMin.x = bounds.bbMin.x < v.x ? bounds.bbMin.x : v.x;
		bounds.bbMin.y = bounds.bbMin.y < v.y ? bounds.bbMin.y : v.y;
		bounds.bbMin.z = bounds.bbMin.z < v.z ? bounds.bbMin.z : v.z;
		bounds.bbMax.x = bounds.bbMax.x > v.x ? bounds.bbMax.x : v.x;
		bounds.bbMax.y = bounds.bbMax.y > v.y ? bounds.bbMax.y : v.y;
		bounds.bbMax.z = bounds.bbMax.z > v.z ? bounds.bbMax.z : v.z;
	}

	bounds.center = (bounds.bbMin + bounds.bbMax) / 2.0f;

	bounds.entents = (bounds.bbMax - bounds.bbMin) / 2.0f;

}
void MeshGeometry::Draw(Shader* shader)
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, &mIndices[0]);
	glBindVertexArray(0);
}

void MeshGeometry::DrawLines(Shader* shader)
{
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, mIndexCount, GL_UNSIGNED_INT, &mIndices[0]);
	glBindVertexArray(0);
}

void RenderItem::Draw(Shader* shader)
{
	geo->Draw(shader);
}

void RenderItem::DrawLines(Shader* shader)
{
	geo->DrawLines(shader);
}

void RenderItem::UpdateWorld()
{
	world = glm::scale(glm::translate(glm::mat4(1.0f), position), scale);
}

void Texture::BuildResource(std::string fileName)
{
	TextureLoader::LoadTexture(textureID, fileName);
}

void Texture::BuildResource(std::vector<std::string> cubeFilenames)
{
	TextureLoader::LoadCubemap(textureID, cubeFilenames);
}

void ModelMesh::BuildResources(std::vector<ModelData> modelData)
{
	for (auto data : modelData)
	{
		MeshGeometry modelGeo;
		modelGeo.mVertices = std::move(data.mVertices);
		modelGeo.mIndices = std::move(data.mIndices);
		modelGeo.BuildResources();
		subMeshes.push_back(std::move(modelGeo));
	}
}