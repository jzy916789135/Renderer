#pragma once
#include "Utils.h"
#include "test_shader.h"
#include <GLFW/glfw3.h>
#include<glfw/glfw3.h>
#include "MeshLoader.h"
#include "test_camera.h"
#include "FrameBufferObject.h"
#include <unordered_map>
#include <mutex>
class Render
{
public:
	Render();
	void Init();
	void BuildFrameBuffer();
	void BuildLight();
	void BuildGeometries();
	void BuildTexture();
	void BuildShaders();
	void BuildMaterials();
	void BuildModelMeshes();
	void Update();
	void UpdateCamera();
	void UpdateObject();
	void UpdateDrawData();
	void GLPrepare();
	void Draw();
	void DrawLight();
	void DrawOpaque();
	void DrawSkyBox();
	void DrawFrameBuffer();
	void DrawShadow();
	void DrawBoundingBox();
	void BuildRenderItems();
	void ProcessInput(GLFWwindow* window);
	static EInputButton GetPressedButton(GLFWwindow* window);


	void SetupImgui();
	void DrawImgui();

	int exit = 0;

private:
	
	std::unique_ptr<Camera> mCamera;
	MeshLoader mMeshLoader;


	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

	std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;

	std::unordered_map<std::string, std::unique_ptr<ModelMesh>> mModelMeshes;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;


	std::vector<std::unique_ptr<RenderItem>> mBoundingBoxRenderItems;
	std::vector<std::unique_ptr<RenderItem>> mRenderItems;
	std::vector<std::unique_ptr<Light>> mLights;

	std::vector<RenderItem*> mOpaqueItems;
	std::vector<RenderItem*> mLightItems;


	std::unique_ptr<RenderItem> mSkyItem;

	std::unique_ptr<GameFrameBufferObject> mSceneFB;
	std::unique_ptr<ShadowMap> mShadowMap;
	GLFWwindow* mWindow = nullptr;

	PassCb mPassCb;
	const unsigned int screenWidth = 1920;
	const unsigned int screenHeight = 1080;
	
	unsigned int quadVAO, quadVBO;
	unsigned int framebuffer, textureColorbuffer, rbo;
	unsigned int shadowFB, shadowCB, shadowRB;
	unsigned int CB, FB, RB;
	unsigned int intermediateFBO, screenTexture;
	float deltaTime = 0.f;
	float lastFrame = 0.f;

	bool isBbox = true;
	unsigned int lastFrameForBbox = 0;

	int numObjVisible = 0;
};