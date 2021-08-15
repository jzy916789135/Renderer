#pragma once
class ShadowMap
{
public:
	ShadowMap(int width, int height) :width(width), height(height)
	{

	}

	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap operator=(const ShadowMap& rhs) = delete;

	// Return frame buffer ID
	void BuildFrameBuffer();

	inline int GetWidth() { return width; };
	inline int GetHeight() { return height; };

	inline unsigned int GetFrameBufferID() { return frameBufferID; };
	inline unsigned int GetDepthTextureID() { return depthTextrueID; };

private:
	unsigned int frameBufferID = -1;
	unsigned int depthTextrueID = -1;
	int width = 0;
	int height = 0;
};


class GameFrameBufferObject
{
public:
	GameFrameBufferObject(int width, int height) :width(width), height(height)
	{

	}

	GameFrameBufferObject(const GameFrameBufferObject& rhs) = delete;//×èÖ¹¿½±´
	GameFrameBufferObject operator=(const GameFrameBufferObject& rhs) = delete;//×é×èÖ¹¸³Öµ

	//Return Buffer ID
	void BuildFrameBuffer();
	inline int GetWidth() { return width; };
	inline int GetHeight() { return height; };

	inline unsigned int GetFrameBufferID() { return frameBufferID; };
	inline unsigned int GetColorTextureID() { return colorTextureID; };
	inline unsigned int GetDepthTextureID() { return depthTextureID; };
private:
	unsigned int frameBufferID  = -1;
	unsigned int colorTextureID = -1;
	unsigned int depthTextureID = -1;
	int width = 0;
	int height = 0;
};