#include "Framework.h"
#include "D3DDevice.h"

bool D3DDevice::SetSwapChain()
{
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;

	HRESULT hr = DXDevice.As(&dxgiDevice);

	DEBUG(hr);

	hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
	DEBUG(hr);

	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf()));
	DEBUG(hr);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = ClientWidth;
	sd.BufferDesc.Height = ClientHeight;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT numerator = 60;
	UINT denominator = 1;

	if (IsVsyncy)
	{
		GetGrapicCardRefreshRate(numerator, denominator);
	}

	sd.BufferDesc.RefreshRate.Numerator = numerator;
	sd.BufferDesc.RefreshRate.Denominator = denominator;

	//RefreshRate : 디스플레이 모드 갱신율(주사율 : Numerator / Denominator)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	//  스캔 라인 순서 및 크기 지정하지 않음
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (isEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = MsaaQulity - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;

	}

	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	hr = dxgiFactory1->CreateSwapChain(DXDevice.Get(), &sd, SwapChain.GetAddressOf());

	DEBUG(hr);

	//ALT + ENTER키에 대한 설정
	//dxgiFactory1->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	OnResize();

	return true;
}

bool D3DDevice::SetDevice()
{
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)  // 디버그 모드일경우 디버그 설정을 한다
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE drivertype[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT DriverTypeSize = ARRAYSIZE(drivertype);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT featureSize = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featurelevel;
	D3D_DRIVER_TYPE d3dDrivertype;
	HRESULT hr;

	for (UINT i = 0; i < DriverTypeSize; i++)
	{
		d3dDrivertype = drivertype[i];
		hr = D3D11CreateDevice(nullptr, d3dDrivertype, nullptr, createDeviceFlags, featureLevels,
			featureSize, D3D11_SDK_VERSION, DXDevice.GetAddressOf(), &featurelevel, Context.GetAddressOf());

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return false;

	DXDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &MsaaQulity);

	return true;
}

void D3DDevice::GetGrapicCardRefreshRate(OUT UINT& numerator, OUT UINT& denominator)
{
	ComPtr<IDXGIFactory> factory;
	//다이렉트 x 그래픽 인터페이스 팩토리 생성
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf());

	DEBUG(hr);

	// 그래픽 카드 어뎁터를 가져온다
	ComPtr<IDXGIAdapter> Adapter;
	hr = factory->EnumAdapters(0, Adapter.GetAddressOf());

	ComPtr<IDXGIOutput> OutPut;

	hr = Adapter->EnumOutputs(0, OutPut.GetAddressOf());

	DEBUG(hr);

	UINT Mode = 0;

	hr = OutPut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &Mode, nullptr);

	DEBUG(hr);
	DXGI_MODE_DESC* DisplayModeList = new DXGI_MODE_DESC[Mode];

	hr = OutPut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &Mode, DisplayModeList);

	DEBUG(hr);

	for (UINT i = 0; i < Mode; i++)
	{
		if (DisplayModeList[i].Width == ClientWidth)
		{
			if (DisplayModeList[i].Height == ClientHeight)
			{
				numerator = DisplayModeList[i].RefreshRate.Numerator;
				denominator = DisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC Desc;
	hr = Adapter->GetDesc(&Desc);

	DEBUG(hr);

	VideoCardMemory = static_cast<int>(Desc.DedicatedVideoMemory / 1024 / 1024);

	size_t Length = 0;
	wstring ws(Desc.Description);
	VideoCardDesc = string(ws.begin(), ws.end());

	DEBUG(hr);

	delete[] DisplayModeList;
}

bool D3DDevice::SetRasterRizer()
{
	D3D11_RASTERIZER_DESC Desc;
	Desc.AntialiasedLineEnable = false;
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.DepthBias = 0;
	Desc.DepthBiasClamp = 0.0f;
	Desc.DepthClipEnable = true;
	Desc.FillMode = D3D11_FILL_SOLID;
	Desc.FrontCounterClockwise = false;
	Desc.MultisampleEnable = false;
	Desc.ScissorEnable = false;
	Desc.SlopeScaledDepthBias = 0.0f;

	HRESULT hr = DXDevice->CreateRasterizerState(&Desc, Rasterrizer.GetAddressOf());
	DEBUG(hr);

	Context->RSSetState(Rasterrizer.Get());

	return true;
}

void D3DDevice::SizeChanged(WPARAM wparam)
{
	if (DXDevice)
	{
		if (wparam == SIZE_MINIMIZED)
		{
			isAppPaused = true;
			isMinmized = true;
			isMaxMized = false;
		}
		else if (wparam == SIZE_MAXIMIZED)
		{
			isAppPaused = false;
			isMinmized = false;
			isMaxMized = true;
			OnResize();
		}
		else if (wparam == SIZE_RESTORED)
		{
			if (isMinmized)
			{
				isAppPaused = false;
				isMinmized = false;
				OnResize();
			}
			else if (isMaxMized)
			{
				isAppPaused = false;
				isMaxMized = false;
				OnResize();
			}
			else if (isResizing)
			{

			}
			else
				OnResize();
		}
	}
}

void D3DDevice::OnResize()
{
	RenderTargetView.Reset();
	DepthView.Reset();
	DepthState.Reset();

	ComPtr<ID3D11Texture2D> backbuffer;
	HRESULT hr = SwapChain->ResizeBuffers(1, ClientWidth, ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	DEBUG(hr);

	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backbuffer.GetAddressOf()));

	DEBUG(hr);
	hr = DXDevice->CreateRenderTargetView(backbuffer.Get(), nullptr, RenderTargetView.GetAddressOf());

	DEBUG(hr);
	backbuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = ClientWidth;
	depthStencilDesc.Height = ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (isEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = MsaaQulity - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = DXDevice->CreateTexture2D(&depthStencilDesc, nullptr, Depth.GetAddressOf());
	DEBUG(hr);
	hr = DXDevice->CreateDepthStencilView(Depth.Get(), nullptr, DepthView.GetAddressOf());
	DEBUG(hr);

	Context->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthView.Get());

	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = ClientWidth;
	vp.Height = ClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &vp);
}

void D3DDevice::BeginRender(D3DXCOLOR clear)
{
	Context->ClearRenderTargetView(RenderTargetView.Get(), clear);
	Context->ClearDepthStencilView(DepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DDevice::EndRender()
{
	if (IsVsyncy)
	{
		SwapChain->Present(1, 0);
	}
	else
	{
		SwapChain->Present(0, 0);
	}
}

D3DDevice::D3DDevice(HINSTANCE hinstance, wstring AppName, bool Enable4xMsaa, bool isVsync)
	:System(hinstance, AppName, Enable4xMsaa)
{
	IsVsyncy = isVsync;
}

D3DDevice::~D3DDevice()
{
}

bool D3DDevice::InitDevice()
{
	if (!AppInit())
		return false;
	if (!SetDevice())
		return false;
	if (!SetSwapChain())
		return false;
	
	SetRasterRizer();

	Engine->Init(RenderTargetView.Get(), ClientWidth, ClientHeight, DXDevice.Get(), Context.Get());

	return true;
}

int D3DDevice::Run()
{
	MSG msg = {};
	Time.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			Time.Update();
			m_Cpu.Update();

			if (!isAppPaused)
			{
				Update();
				Render();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}
