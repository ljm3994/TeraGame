#pragma once

class TextureRect : public Transform
{
private:
	Texture * texture;
	Shader * shader;
	VertexBuffer * vertexbuffer;
	IndexBuffer * indicesbuffer;
public:
	TextureRect();
	~TextureRect();
	void Update();
	void Render();
	void ChangeTexture(string Tag);
};