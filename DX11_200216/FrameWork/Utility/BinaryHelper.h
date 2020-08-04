#pragma once

class BinaryWrite
{
private:
	HANDLE File;
	DWORD Size;
public:
	BinaryWrite(wstring Path);
	~BinaryWrite();

	void Int(const int* Data);
	void Uint(const UINT* Data);
	void Float(const float* Data);
	void Double(const double* Data);
	void String(const string* Data);
	void Matrix(const D3DXMATRIX* Data);
	void D3DXVector3(const D3DXVECTOR3* Data);
	void D3DXVector2(const D3DXVECTOR2* Data);
	void Byte(const void* Data, const UINT* ArraySize);
};

class BinaryReader
{
private:
	HANDLE File;
	DWORD Size;
public:
	BinaryReader(wstring Path);
	~BinaryReader();

	int Int();
	UINT Uint();
	float Float();
	double Double();
	string String();
	D3DXVECTOR3 D3DXVector3();
	D3DXVECTOR2 D3DXVector2();
	D3DXMATRIX Matrix();
	bool Byte(void** Data, const UINT* ArraySize);
};