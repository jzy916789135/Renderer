#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#define PI 3.141592654

class GeometryGenerator
{
public:
	using uint16 = uint16_t;
	using uint32 = uint32_t;

	//存储顶点数据
	struct  Vertex
	{
		Vertex() {}
		Vertex(
			const glm::vec3& p,
			const glm::vec3& n,
			const glm::vec3& t,
			const glm::vec2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Normal = glm::vec3(0.f);
		glm::vec3 TangentU = glm::vec3(0.f); // 切线
		glm::vec2 TexC = glm::vec2(0.f);//纹理坐标

	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;//绘制顶点
		std::vector<uint32> Indices32;//绘制顺序

		std::vector<uint16> GetIndices()//获取物体绘制顺序，只不过将uint32转为了uint16
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);

			}
			return mIndices16;
		}
	private:
		std::vector<uint16> mIndices16;
	};


	
	MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);
private:
	void Subdivide(MeshData& meshdata);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};