#pragma once

#include "FrameWork/Environment/Camera/Camera.h"
#include "Framework/Environment/Environment.h"
#include "FrameWork/Render/Shader/Shader.h"
#include "FrameWork/Render/Shader/Texture.h"
#include <FrameWork\Game\Scene\SceneBase.h>
#include <FrameWork\Render\Model\ObjectBase.h>
#include "FrameWork\Render\Culling\Frustum.h"
#include <FrameWork\Render\Model\ModelInstance.h>
#include "FrameWork\Game\Sound.h"

class PostProcessRender;
class DXEngine
{
private:
	struct Scens
	{
		SceneBase* CurrentScene;
		map<string, unique_ptr<SceneBase>> SceneList;

		Scens()
		{
			CurrentScene = nullptr;
		}
	}scenes;

	struct Cameras
	{
		Camera* CurrentCarmera;
		map<string, unique_ptr<Camera>> CarmeraList;

		Cameras()
		{
			CurrentCarmera = nullptr;
		}
		~Cameras()
		{

		}
	}cameras;

	ID3D11Device * device;
	ID3D11DeviceContext* Context;
	DirectionLightBuffer* CurrentLight;
	UINT ClientWidth;
	UINT ClientHeight;
	bool isLoadingScene;
	PostProcessRender* postprocess;
	unique_ptr<Environment> environment;

	unique_ptr<GameSound> SoundSystem;
	map<string, unique_ptr<Shader>> ShaderList;
	map<string, unique_ptr<Texture>> TextureList;
	map<string, unique_ptr<ObjectBase>> ObjectList;
	map<string, unique_ptr<ModelInstance>> ModelList;
	unique_ptr<Frustum> frustumculling;
	static shared_ptr<DXEngine> _Instance;
	static once_flag flag;

public:
	static DXEngine* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<DXEngine>();
			});

		return _Instance.get();
	}
	DXEngine();
	~DXEngine();
	void Init(ID3D11RenderTargetView* backbuffer, UINT Width, UINT Height, ID3D11Device* device, ID3D11DeviceContext* Context);
	void Update(ID3D11RenderTargetView* backbuffer, UINT ClienWidth, UINT ClientHeight, float DeltaTime, float TotalTime);
	void PreRender();
	void Render();
	void PostRender();

	SceneBase* AddScene(string Tag, SceneBase* scene);
	void ChangeScene(string Tag);
	SceneBase* FindScene(string Tag);
	SceneBase* GetCurrentScene();

	Camera* AddCamera(string Tag, CarmeraType type);
	void ChangeCamera(string Tag);
	template<class T>
	T* FindCamera(string Tag);
	Camera* GetCurrentCamera();

	Shader* AddShader(string Key, wstring Path, ShaderCreateFlag shadercreateFlag = ShaderCreateFlag::SHADER_FLAG_VS | ShaderCreateFlag::SHADER_FLAG_PS, string VSName = "VS", string PSName = "PS", string CSName = "CS", string GSName = "GS");
	Shader* AddShader(wstring Path, ShaderCreateFlag shadercreateFlag = ShaderCreateFlag::SHADER_FLAG_VS | ShaderCreateFlag::SHADER_FLAG_PS, string VSName = "VS", string PSName = "PS", string CSName = "CS", string GSName = "GS");
	Shader* FindShader(string Key);

	Texture* AddRandomTexture(string Key, int Width, int Height, DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT);
	Texture* AddTexture(string Key, wstring _Path, bool isSampler = true, bool isDDS = false, D3D11_FILTER Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE Mode = D3D11_TEXTURE_ADDRESS_WRAP);
	Texture* FindTexture(string Key);

	void AddModel(string Tag, string FileName, MODELTYPE Type, bool isCollision, UINT instanceCount);
	template<class T>
	T* GetComponentModel(string Tag);
	void DeleteModel(string Tag);

	ObjectBase* AddObject(string Tag, ObjectBase* object);
	template<class T>
	T* GetComponentObject(string Tag);

	void DestoryObject(string Tag);

	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetContext() { return Context; }
	UINT GetClientWidth() { return ClientWidth; }
	UINT GetClientHeight() { return ClientHeight; }
	//LightBuffer* GetLight() { return environment.GetLight(); }
	Ray GetMouseRay(D3DXVECTOR2 MousePos);

	D3DXMATRIX GetProjectionMatrix() { return environment->GetProjection(); }
	DirectionLightBuffer* GetLight() { return CurrentLight; }
	Frustum* GetFrustum() { return frustumculling.get(); }
	GameSound * GetSoundSystem() { return SoundSystem.get(); }
	Environment* GetEnviroment() { return environment.get(); }
	void SetLoading(bool val) { isLoadingScene = val; }
	//void GetClientSize(UINT& Width, UINT& Height);
};

template<class T>
inline T* DXEngine::GetComponentObject(string Tag)
{
	auto object = ObjectList.find(Tag);
	if (object != ObjectList.end())
		return dynamic_cast<T*>(object->second.get());
	else
		return nullptr;
}

template<class T>
inline T* DXEngine::FindCamera(string Tag)
{
	if (cameras.CarmeraList.count(Tag) > 0)
	{
		return dynamic_cast<T*>(cameras.CarmeraList[Tag].get());
	}

	return nullptr;
}

template<class T>
inline T* DXEngine::GetComponentModel(string Tag)
{
	if (ModelList.count(Tag) > 0)
	{
		return dynamic_cast<T*>(ModelList[Tag].get());
	}

	return nullptr;
}