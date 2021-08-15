#include <iostream>
#include <glad/glad.h>
#include "Collision.h"
#include "Utils.h"
#include "render.h"
#include "test_camera.h"
#include "GeometryGenerator.h"


Render::Render()
{

}
void Render::Init()
{
	GLPrepare();
	BuildFrameBuffer();
	mCamera = std::make_unique<Camera>(glm::vec3(-10.f, 5.f, 0.f), 45, 1920.f / 1080.f, 0.1f, 100.f);
	BuildLight();
	BuildTexture();
	BuildGeometries(); 
	BuildMaterials();
	BuildShaders();
	BuildModelMeshes();
	BuildRenderItems();
}

void Render::BuildFrameBuffer()
{
	mShadowMap = std::make_unique<ShadowMap>(2048, 2048);
	mShadowMap->BuildFrameBuffer();
	mSceneFB = std::make_unique<GameFrameBufferObject>(1920, 1080);
	mSceneFB->BuildFrameBuffer();

	float quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

void Render::BuildLight()
{
	auto dirLight = std::make_unique<Light>();
	dirLight->type = 0;
	dirLight->strength = glm::vec3(1.0f, 1.0f, 1.0f);
	dirLight->position = glm::vec3(1.0f, 8.0f, 0.0f);
	dirLight->focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	mLights.push_back(std::move(dirLight));
}
void Render::BuildTexture()
{
	std::vector<std::string> stbTexNames =
	{
		"brickWallDiffuseMap",
		"brickWallNormalMap"
	};

	std::vector<std::string> stbTexFilenames =
	{
		"Texture/brickwall.jpg",
		"Texture/brickwall_normal.jpg"
	};

	std::vector<std::string> stbCubeNames =
	{
		"skyBox"
	};

	std::vector<std::vector<std::string>> stbCubeFilenames =
	{
		{
			"Texture/right.jpg",
			"Texture/left.jpg",
			"Texture/top.jpg",
			"Texture/bottom.jpg",
			"Texture/front.jpg",
			"Texture/back.jpg"
		}
	};

	for (int i = 0; i < stbTexNames.size(); ++i)
	{
		auto texMap = std::make_unique<Texture>();
		texMap->name = stbTexNames[i];
		texMap->path = stbTexFilenames[i];

		texMap->BuildResource(texMap->path.c_str());

		mTextures[texMap->name] = std::move(texMap);
	}

	for (int i = 0; i < stbCubeNames.size(); ++i)
	{
		auto texMap = std::make_unique<Texture>();
		texMap->name = stbCubeNames[i];
		texMap->cubepath = stbCubeFilenames[i];

		texMap->BuildResource(texMap->cubepath);
		mTextures[texMap->name] = std::move(texMap);
	}
}
void Render::BuildMaterials()
{

	//skybox
	auto sky = std::make_unique<Material>();
	sky->name = "sky";
	sky->diffuseID = mTextures["skyBox"]->textureID;

	auto defaultMat = std::make_unique <Material>();
	defaultMat->name = "default";
	defaultMat->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	defaultMat->metallic = 0.1f;
	defaultMat->roughness = 1.f;
	defaultMat->ambientOcclusion = 0.f;
	//box
	auto brick0 = std::make_unique<Material>();
	brick0->name = "brick0";
	brick0->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	brick0->metallic = 0.1f;
	brick0->roughness = 0.8f;
	brick0->ambientOcclusion = 0.f;
	brick0->useDiffuse = true;
	brick0->useNormal = true;
	brick0->diffuseID = mTextures["brickWallDiffuseMap"]->textureID;
	brick0->normalID = mTextures["brickWallNormalMap"]->textureID;

	//floor
	auto tile = std::make_unique<Material>();
	tile->name = "tile";
	tile->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	tile->metallic = 0.1f;
	tile->roughness = 0.8f;
	tile->ambientOcclusion = 0.f;
	tile->useDiffuse = true;
	tile->useNormal = true;
	tile->diffuseID = mTextures["brickWallDiffuseMap"]->textureID;
	tile->normalID = mTextures["brickWallNormalMap"]->textureID;

	//light
	auto light0 = std::make_unique<Material>();
	light0->name = "light0";
	light0->emmisive = mLights[0]->strength;

	mMaterials["light0"] = std::move(light0);
	mMaterials["sky"] = std::move(sky);
	mMaterials["brick0"] = std::move(brick0);
	mMaterials["tile"] = std::move(tile);
}
void Render::BuildGeometries()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0);
	GeometryGenerator::MeshData cube = geoGen.CreateBox(2.0f, 2.0f, 2.0f, 0);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.f, 30.f, 60., 40);

	MeshGeometry boxMesh("box");
	MeshGeometry cubeMesh("cube");
	MeshGeometry gridMesh("grid");

	std::vector<Vertex> boxVertices(box.Vertices.size());
	std::vector<Vertex> cubeVertices(cube.Vertices.size());
	std::vector<Vertex> gridVertices(grid.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		boxVertices[i].pos = box.Vertices[i].Position;
		boxVertices[i].normal = box.Vertices[i].Normal;
		boxVertices[i].tangentU = box.Vertices[i].TangentU;
		boxVertices[i].texC = box.Vertices[i].TexC;
	}
	for (size_t i = 0; i < cube.Vertices.size(); ++i)
	{
		cubeVertices[i].pos = cube.Vertices[i].Position;
		cubeVertices[i].normal = cube.Vertices[i].Normal;
		cubeVertices[i].tangentU = cube.Vertices[i].TangentU;
		cubeVertices[i].texC = cube.Vertices[i].TexC;
	}
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		gridVertices[i].pos = grid.Vertices[i].Position;
		gridVertices[i].normal = grid.Vertices[i].Normal;
		gridVertices[i].tangentU = grid.Vertices[i].TangentU;
		gridVertices[i].texC = grid.Vertices[i].TexC;
	}
	//一开始加载数据，并存储到自身的结构中
	boxMesh.BuildResources(boxVertices, box.Indices32);
	cubeMesh.BuildResources(cubeVertices, cube.Indices32);
	gridMesh.BuildResources(gridVertices, grid.Indices32);

	mGeometries["box"] = std::move(std::make_unique<MeshGeometry>(boxMesh));
	mGeometries["cube"] = std::move(std::make_unique<MeshGeometry>(cubeMesh));
	mGeometries["grid"] = std::move(std::make_unique<MeshGeometry>(gridMesh));
}
void Render::BuildShaders()
{
	//SkyBox
	auto skyShader = std::make_unique<Shader>();
	skyShader->CreateVS("Shader/SkyBox.vert");
	skyShader->CreateFS("Shader/SkyBox.frag");
	skyShader->Attach();
	skyShader->Link();
	skyShader->use();
	skyShader->SetInt("skybox", 0);
	mShaders["Sky"] = std::move(skyShader);

	//screen
	auto sreenShader = std::make_unique<Shader>();
	sreenShader->CreateVS("Shader/screen.vert");
	sreenShader->CreateFS("Shader/screen.frag");
	sreenShader->Attach();
	sreenShader->Link();
	sreenShader->use();
	sreenShader->SetInt("screenTexture", 0);
	mShaders["Screen"] = std::move(sreenShader);

	//shadow
	auto shadowShader = std::make_unique<Shader>();
	shadowShader->CreateVS("Shader/Shadow.vert");
	shadowShader->CreateFS("Shader/Shadow.frag");
	shadowShader->Attach();
	shadowShader->Link();
	shadowShader->use();
	
	glm::mat4 lightView = glm::lookAt(mLights[0]->position, mLights[0]->focalPoint, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightProj = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 100.f);
	shadowShader->SetMat4("lightProjView", lightProj * lightView);
	mShaders["Shadow"] = std::move(shadowShader);

	//Opaque objects
	auto shapeShader = std::make_unique<Shader>();
	shapeShader->CreateVS("Shader/Shape.vert");
	shapeShader->CreateFS("Shader/Shape.frag");
	//shapeShader->CreateGS("Shape.geo");
	shapeShader->Attach();
	shapeShader->Link();
	shapeShader->use();
	for (int i = 0; i < mLights.size(); ++i)
	{
		shapeShader->SetInt("light[" + std::to_string(i) + "].type", mLights[i]->type);
		shapeShader->SetVec3("light[" + std::to_string(i) + "].pos", mLights[i]->position);
		shapeShader->SetVec3("light[" + std::to_string(i) + "].strength", mLights[i]->strength);
		shapeShader->SetVec3("light[" + std::to_string(i) + "].dir", mLights[i]->focalPoint - mLights[i]->position);
		shapeShader->SetFloat("light[" + std::to_string(i) + "].fallStart", mLights[i]->falloffStart);
		shapeShader->SetFloat("light[" + std::to_string(i) + "].fallEnd", mLights[i]->falloffEnd);
		shapeShader->SetFloat("light[" + std::to_string(i) + "].spotPower", mLights[i]->spotPower);
	}
	shapeShader->SetInt("diffuseMap", 0);
	shapeShader->SetInt("normalMap", 1);
	shapeShader->SetInt("shadowMap", 2);
	shapeShader->SetMat4("lightProjView", lightProj * lightView);
	mShaders["Shape"] = std::move(shapeShader);

	//Light
	auto lightShader = std::make_unique<Shader>();
	lightShader->CreateVS("Shader/Light.vert");
	lightShader->CreateFS("Shader/Light.frag");
	lightShader->Attach();
	lightShader->Link();
	lightShader->use();
	mShaders["Light"] = std::move(lightShader);

	//Boundingbox
	auto boundingBoxShader = std::make_unique<Shader>();
	boundingBoxShader->CreateVS("Shader/BoundingBox.vert");
	boundingBoxShader->CreateFS("Shader/BoundingBox.frag");
	boundingBoxShader->Attach();
	boundingBoxShader->Link();
	boundingBoxShader->use();
	mShaders["BoundingBox"] = std::move(boundingBoxShader);


}

void Render::BuildModelMeshes()
{
	auto modelData = mMeshLoader.LoadModel("Model/nanosuit.obj");
	auto model = std::make_unique<ModelMesh>();
	model->name = "nanosuit";
	model->position = glm::vec3(0.0f, 0.0f, 1.0f);
	model->BuildResources(modelData);
	mModelMeshes[model->name] = std::move(model);
}

void Render::BuildRenderItems()
{	
	//model
	for (auto iter = mModelMeshes.begin(); iter != mModelMeshes.end(); ++iter)
	{
		for (auto& subMesh : iter->second->subMeshes)
		{
			auto item = std::make_unique<RenderItem>();
			item->name = iter->second->name;
			item->position = iter->second->position;
			item->scale = glm::vec3(0.2f);
			item->UpdateWorld();
			item->geo = &subMesh;
			item->mat = mMaterials["tile"].get();
			item->textureScale = 1;
			mOpaqueItems.push_back(item.get());
			mRenderItems.push_back(std::move(item));
		}
	}

	//skybox
	auto skyItem = std::make_unique<RenderItem>();
	skyItem->name = "Sky";
	skyItem->UpdateWorld();
	skyItem->mat = mMaterials["sky"].get();
	skyItem->geo = mGeometries["cube"].get();
	mSkyItem = std::move(skyItem);

	//box
	
	auto boxItem = std::make_unique<RenderItem>();
	boxItem->name = "Box";
	boxItem->position = glm::vec3(0.0f, 2.0f, 0.0f);
	boxItem->UpdateWorld();
	boxItem->mat = mMaterials["brick0"].get();
	boxItem->geo = mGeometries["box"].get();
	boxItem->textureScale = 1;
	mOpaqueItems.push_back(boxItem.get());
	mRenderItems.push_back(std::move(boxItem));
	
	//grid

	auto gridItem = std::make_unique<RenderItem>();
	gridItem->name = "Floor";
	gridItem->mat = mMaterials["tile"].get();
	gridItem->geo = mGeometries["grid"].get();
	gridItem->textureScale = 1;
	mOpaqueItems.push_back(gridItem.get());
	mRenderItems.push_back(std::move(gridItem));

	//light
	auto lightItem = std::make_unique<RenderItem>();
	lightItem->name = "DirectionalLight";
	lightItem->position = mLights[0]->position;
	lightItem->scale = glm::vec3(0.5f);
	lightItem->UpdateWorld();
	lightItem->textureScale = 1;
	lightItem->mat = mMaterials["light0"].get();
	lightItem->geo = mGeometries["cube"].get();
	mLightItems.push_back(lightItem.get());
	mRenderItems.push_back(std::move(lightItem));

	for (int i = 0; i < mRenderItems.size(); i++)
	{
		auto opaqueItem = mRenderItems[i].get();
		auto item = std::make_unique<RenderItem>();
		item->position = opaqueItem->position + opaqueItem->geo->bounds.center;
		//item->scale = glm::vec3(opaqueItem->geo->bounds.entents.x * 2 * opaqueItem->scale.x, opaqueItem->geo->bounds.entents.y * 2 * opaqueItem->scale.y, opaqueItem->geo->bounds.entents.z * 2 * opaqueItem->scale.z);
		item->scale = glm::vec3(opaqueItem->geo->bounds.entents.x * opaqueItem->scale.x, opaqueItem->geo->bounds.entents.y * opaqueItem->scale.y, opaqueItem->geo->bounds.entents.z * opaqueItem->scale.z);
		item->UpdateWorld();
		item->mat = mMaterials["default"].get();
		item->geo = mGeometries["cube"].get();
		mBoundingBoxRenderItems.push_back(std::move(item));
	}
}
void Render::Update()
{
	UpdateCamera();
	UpdateObject();
	UpdateDrawData();
}

void Render::UpdateCamera()
{
	mPassCb.view = mCamera->GetViewMatrix();
	mPassCb.proj = mCamera->GetProjMatrix();
	mPassCb.eyePos = mCamera->position;
	mPassCb.lightNum = mLights.size();
}

void Render::UpdateObject()
{
	for (int i = 0; i < mRenderItems.size(); ++i)
	{
		mRenderItems[i]->UpdateWorld();
	}
}
void Render::UpdateDrawData()
{
	glm::mat4 vp = mCamera->GetViewProjMatrix();
	numObjVisible = 0;
	for (int i = 0; i < mRenderItems.size(); i++)
	{
		glm::mat4 mvp = vp * mRenderItems[i]->world;
		BoundingFrustum localFrustum;
		localFrustum.ExtractFrustumFromMatrix(mvp);
		IntersectType type = localFrustum.ContainAxisAlignedBox(mRenderItems[i]->geo->bounds);
		if (type == IntersectType::DISJOINT) mRenderItems[i]->bCulled = true;
		else
		{
			mRenderItems[i]->bCulled = false;
			numObjVisible++;
		}
		
	}
}
void Render::GLPrepare()
{
	//glfw initalize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfw window creation
	mWindow = glfwCreateWindow(screenWidth, screenHeight, "Render", NULL, NULL);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(mWindow);
	typedef void* (*FUNC)(GLFWwindow*, int, int);

	//glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initalize GLAD" << std::endl;
		return;
	}

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	


	glGenFramebuffers(1, &shadowFB);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFB);
	// create a depth attachment texture
	glGenTextures(1, &shadowRB);
	glBindTexture(GL_TEXTURE_2D, shadowRB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowRB, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Render::Draw()
{


	while (!glfwWindowShouldClose(mWindow))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ProcessInput(mWindow);

		Update();
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, screenWidth, screenHeight);

		DrawLight();
		DrawShadow();
		DrawSkyBox();
		DrawOpaque();
		DrawBoundingBox();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		DrawFrameBuffer();

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	glfwTerminate();

}

void Render::DrawLight()
{
	auto light = mShaders["Light"].get();
	light->use();
	light->SetMat4("passCb.view", mPassCb.view);
	light->SetMat4("passCb.proj", mPassCb.proj);
	for (auto item : mLightItems)
	{
		glm::mat4 model = glm::scale(item->world,glm::vec3(0.2f));
		light->SetMat4("model", item->world);
		light->SetVec3("emmisive", item->mat->emmisive);
		item->Draw(light);
	}
}
void Render::DrawOpaque()
{
	auto shape = mShaders["Shape"].get();
	shape->use();
	shape->SetMat4("passCb.view", mPassCb.view);
	shape->SetMat4("passCb.proj", mPassCb.proj);
	shape->SetVec3("passCb.eyePos", mPassCb.eyePos);
	shape->SetVec4("passCb.ambientLight", mPassCb.ambientLight);
	shape->SetInt("passCb.lightNum", mPassCb.lightNum);
	shape->SetFloat("time", glfwGetTime());
	for (unsigned int i = 0; i < mPassCb.lightNum; i++)
	{
		mLights[i]->position = mRenderItems[mRenderItems.size() - i - 1]->position;
		shape->SetVec3("light[" + std::to_string(i) + "].pos", mLights[i]->position);
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowRB);
	for (auto item : mOpaqueItems)
	{
		if (item->bCulled) continue;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, item->mat->diffuseID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, item->mat->normalID);
		shape->SetMat4("model", item->world);
		shape->SetInt("textureScale", item->textureScale);
		shape->SetVec3("material.albedo", item->mat->albedo);
		shape->SetFloat("material.metallic", item->mat->metallic);
		shape->SetFloat("material.roughness", item->mat->roughness);
		shape->SetFloat("material.ao", item->mat->ambientOcclusion);
		shape->SetFloat("material.useDiffuseMap", item->mat->useDiffuse);
		shape->SetFloat("material.useNormalMap", item->mat->useNormal);
		item->Draw(shape);
	}
}

void Render::DrawShadow()
{
	glViewport(0, 0, mShadowMap->GetWidth(), mShadowMap->GetHeight());
	auto shadow = mShaders["Shadow"].get();
	shadow ->use();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto item : mOpaqueItems)
	{
		shadow->SetMat4("model", item->world);
		item->Draw(shadow);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, screenWidth, screenHeight);

}

void Render::DrawFrameBuffer()
{	
	glDisable(GL_DEPTH_TEST);
	auto screen = mShaders["Screen"].get();
	screen->use();
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Render::DrawSkyBox()
{
	glDepthFunc(GL_LEQUAL);
	auto sky = mShaders["Sky"].get();
	sky->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyItem->mat->densityID);
	sky->SetMat4("view", glm::mat4(glm::mat3(mCamera->GetViewMatrix())));
	sky->SetMat4("proj", mCamera->GetProjMatrix());
	mSkyItem->Draw(sky);
	glDepthFunc(GL_LESS);
}

void Render::DrawBoundingBox()
{
	auto boundingBox = mShaders["BoundingBox"].get();
	boundingBox->use();
	boundingBox->SetMat4("passCb.view", mPassCb.view);
	boundingBox->SetMat4("passCb.proj", mPassCb.proj);
	for (int i = 0; i < mBoundingBoxRenderItems.size(); i++)
	{
		auto item = mBoundingBoxRenderItems[i].get();
		if (item->bCulled) continue;
		boundingBox->SetMat4("model", item->world);
		item->DrawLines(boundingBox);
	}
}
void Render::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		exit = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mCamera->ProcessKeyboard(EcameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mCamera->ProcessKeyboard(EcameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mCamera->ProcessKeyboard(EcameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mCamera->ProcessKeyboard(EcameraMovement::RIGHT, deltaTime);

	double x, y;
	glfwGetCursorPos(mWindow, &x, &y);
	mCamera->ProcessMouseMovement(x, y, GetPressedButton(mWindow));
}

EInputButton Render::GetPressedButton(GLFWwindow* window)
{
	EInputButton result = EInputButton::NONE;

	if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
		return EInputButton::LEFT;	
	else if (glfwGetMouseButton(window, 1) == GLFW_PRESS)
		return EInputButton::RIGHT;
	else if (glfwGetMouseButton(window, 2) == GLFW_PRESS)
		return EInputButton::MIDDLE;

	return EInputButton::NONE;
}