#include "Framework.h"
#include "DXEngine.h"
#include "FrameWork/Environment/Camera/FreeCamera.h"
#include <FrameWork\Environment\Camera\FollowCarmera.h>
#include <FrameWork\Environment\Camera\DefaultCamera.h>
#include <FrameWork\Render\Model\SkinningModel.h>
#include <FrameWork\Render\Model\StaticModel.h>
#include "FrameWork\Render\PostProcess\PostProcessRender.h"
shared_ptr<DXEngine> DXEngine::_Instance = nullptr;
once_flag DXEngine::flag;

DXEngine::DXEngine()
	: ClientWidth(0), ClientHeight(0)
{
}

DXEngine::~DXEngine()
{
	delete postprocess;
}

void DXEngine::Init(ID3D11RenderTargetView* backbuffer, UINT Width, UINT Height, ID3D11Device* device, ID3D11DeviceContext* Context)
{
	ClientWidth = Width;
	ClientHeight = Height;
	this->device = device;
	this->Context = Context;
	isLoadingScene = true;
	
	postprocess = new PostProcessRender();
	postprocess->Init(backbuffer);
	environment.reset(new Environment);
	CurrentLight = environment->DirectionLightCreate();
	environment->PointLightCreate();
	environment->SpotLightCreate();
	CurrentLight->SetAmbientLight(D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f));
	environment->SetProjection(Width, Height);
	SoundSystem.reset(new GameSound());
	AddCamera("DefaultCam", CarmeraType::FREECARMERA);
	ChangeCamera("DefaultCam");
	cameras.CurrentCarmera->SetEye(10, -5, 0.0f);
	frustumculling.reset(new Frustum());
}

void DXEngine::Update(ID3D11RenderTargetView* backbuffer, UINT ClienWidth, UINT ClientHeight, float DeltaTime, float TotalTime)
{
	if (cameras.CurrentCarmera)
		cameras.CurrentCarmera->Update(DeltaTime);

	this->ClientWidth = ClienWidth;
	this->ClientHeight = ClientHeight;

	SoundSystem->Update();
	environment->Update();
	postprocess->ConstantUpdate(backbuffer, TotalTime, DeltaTime);
	postprocess->Update(DeltaTime);

	frustumculling->BulidFrustum();

	if (scenes.CurrentScene)
		scenes.CurrentScene->SceneUpdate(DeltaTime, 0);
}

void DXEngine::PreRender()
{
	environment->Render();

	if (scenes.CurrentScene)
		scenes.CurrentScene->ScenePreRender();
}

void DXEngine::Render()
{
	if (!isLoadingScene)
	{
		postprocess->Set();
	}
	else
	{
		environment->Render();

		if (scenes.CurrentScene)
			scenes.CurrentScene->SceneRender();
	}
}

void DXEngine::PostRender()
{
	if (!isLoadingScene)
	{
		postprocess->Render();
	
		if (scenes.CurrentScene)
			scenes.CurrentScene->ScenePostRender();
	}
	else
	{
		if (scenes.CurrentScene)
			scenes.CurrentScene->ScenePostRender();
	}
}

SceneBase* DXEngine::AddScene(string Tag, SceneBase* scene)
{
	if (scenes.SceneList.count(Tag) > 0)
	{
		return scenes.SceneList[Tag].get();
	}
	
	scenes.SceneList[Tag].reset(scene);

	return scenes.SceneList[Tag].get();
}

void DXEngine::ChangeScene(string Tag)
{
	if (scenes.CurrentScene)
		scenes.CurrentScene->SceneRelease();

	if (scenes.SceneList.count(Tag) > 0)
	{
		scenes.CurrentScene = scenes.SceneList[Tag].get();
	}
}

SceneBase* DXEngine::FindScene(string Tag)
{
	if (scenes.SceneList.count(Tag) > 0)
	{
		return scenes.SceneList[Tag].get();
	}

	return nullptr;
}

SceneBase* DXEngine::GetCurrentScene()
{
	return scenes.CurrentScene;
}

Camera* DXEngine::AddCamera(string Tag, CarmeraType type)
{
	if (cameras.CarmeraList.count(Tag) > 0)
	{
		return cameras.CarmeraList[Tag].get();
	}

	switch (type)
	{
	case CarmeraType::FREECARMERA:
		cameras.CarmeraList[Tag].reset(new FreeCarmera());
		break;
	case CarmeraType::FOLLOWCAMERA:
		cameras.CarmeraList[Tag].reset(new FollowCamera());
		break;
	case CarmeraType::DEFAULTCARMERA:
		cameras.CarmeraList[Tag].reset(new DefaultCamera());
		break;
	}

	return cameras.CarmeraList[Tag].get();
}

void DXEngine::ChangeCamera(string Tag)
{
	if (cameras.CarmeraList.count(Tag) > 0)
	{
		cameras.CurrentCarmera = cameras.CarmeraList[Tag].get();
	}
}

Camera* DXEngine::GetCurrentCamera()
{
	return cameras.CurrentCarmera;
}

Shader* DXEngine::AddShader(string Key, wstring Path, ShaderCreateFlag shadercreateFlag, string VSName, string PSName, string CSName, string GSName)
{
	if (ShaderList.count(Key) > 0)
	{
		return ShaderList[Key].get();
	}

	ShaderList[Key].reset(new Shader(Path, shadercreateFlag, VSName, PSName, CSName, GSName));

	return ShaderList[Key].get();
}

Shader* DXEngine::AddShader(wstring Path, ShaderCreateFlag shadercreateFlag, string VSName, string PSName, string CSName, string GSName)
{
	if (ShaderList.count(Helper::wstringTostring(&Path)) > 0)
	{
		return ShaderList[Helper::wstringTostring(&Path)].get();
	}

	ShaderList[Helper::wstringTostring(&Path)].reset(new Shader(Path, shadercreateFlag, VSName, PSName, CSName, GSName));
	return ShaderList[Helper::wstringTostring(&Path)].get();
}

Shader* DXEngine::FindShader(string Key)
{
	if (ShaderList.count(Key) > 0)
	{
		return ShaderList[Key].get();
	}

	return nullptr;
}

Texture* DXEngine::AddRandomTexture(string Key, int Width, int Height, DXGI_FORMAT format)
{
	if (TextureList.count(Key) > 0)
	{
		return TextureList[Key].get();
	}

	TextureList[Key].reset(new Texture());
	TextureList[Key]->CreateRandomTexture(Width, Height, format);

	return TextureList[Key].get();
}

Texture* DXEngine::AddTexture(string Key, wstring _Path, bool isSampler, bool isDDS, D3D11_FILTER Filter, D3D11_TEXTURE_ADDRESS_MODE Mode)
{
	if (TextureList.count(Key) > 0)
	{
		return TextureList[Key].get();
	}

	TextureList.emplace(make_pair(Key, unique_ptr<Texture>(new Texture(_Path, isSampler, Filter, isDDS, Mode))));

	return TextureList[Key].get();
}

Texture* DXEngine::FindTexture(string Key)
{
	if (TextureList.count(Key) > 0)
	{
		return TextureList[Key].get();
	}

	return nullptr;
}

void DXEngine::AddModel(string Tag, string FileName, MODELTYPE Type, bool isCollision, UINT instanceCount)
{
	if (ModelList.count(Tag) > 0)
	{
		return;
	}

	switch (Type)
	{
	case MODELTYPE::SKINNING_MODELTYPE:
		ModelList[Tag].reset(new SkinningModel(instanceCount, FileName, "Model", isCollision));
		break;
	case MODELTYPE::STATIC_MODELTYPE:
		ModelList[Tag].reset(new StaticModel(FileName, "StaticMeshShader", isCollision, instanceCount));
		break;
	}
}


void DXEngine::DeleteModel(string Tag)
{
	if (ModelList.count(Tag) > 0)
	{
		ModelList[Tag].release();
		ModelList.erase(Tag);
	}
}

ObjectBase* DXEngine::AddObject(string Tag, ObjectBase* object)
{
	if (ObjectList.count(Tag) > 0)
	{
		return ObjectList[Tag].get();
	}

	ObjectList[Tag].reset(object);

	return ObjectList[Tag].get();
}

void DXEngine::DestoryObject(string Tag)
{
	if (ObjectList.count(Tag) > 0)
	{
		ObjectList[Tag].reset();
		ObjectList.erase(Tag);
	}
}

Ray DXEngine::GetMouseRay(D3DXVECTOR2 MousePos)
{
	D3DXVECTOR2 Screen = D3DXVECTOR2(ClientWidth, ClientHeight);

	D3DXVECTOR2 pt;

	pt.x = (2.0f * MousePos.x / Screen.x) - 1.0f;
	pt.y = ((2.0f * MousePos.y / Screen.y) - 1.0f) * -1.0f;

	D3DXMATRIX Projection = environment->GetViewProjection()->GetProjectionData();
	D3DXMATRIX ViewMat = environment->GetViewProjection()->GetViewData();

	pt.x /= Projection._11;
	pt.y /= Projection._22;

	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, nullptr, &ViewMat);

	Ray ray;
	ray.position =	cameras.CurrentCarmera->GetEye();

	D3DXVec3TransformNormal(&ray.direction, &D3DXVECTOR3(pt.x, pt.y, 1), &invView);

	return ray;
}
