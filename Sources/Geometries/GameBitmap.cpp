#include <Sources/Geometries/GameBitmap.h>

GameBitmap::GameBitmap() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_Texture(nullptr)
{

}

GameBitmap::GameBitmap(const GameBitmap& other) 
{

}

GameBitmap::~GameBitmap()
{

}

bool GameBitmap::Initialize(ID3D11Device *device, int screenWidth, int screenHeight, wchar_t *filePath, int bitmapWidth, int bitmapHeight) 
{
	m_screenWidth  = screenWidth;
	m_screenHeight = screenHeight;

	m_bitmapWidth  = bitmapWidth;
	m_bitmapHeight = bitmapHeight;


	m_prevPosX = -1;
	m_prevPosY = -1;

	bool result;
	result = InitializeBuffers(device);
	if (!result) 
	{
		return false;
	}

	result = LoadTexture(device, filePath);
	if (!result) 
	{
		return false;
	}

	return true;
}

void GameBitmap::Release() 
{
	ReleaseTexture();
	ReleaseBuffers();
}

bool GameBitmap::Render(ID3D11DeviceContext *deviceContext, int posX, int posY)
{
	bool result;

	result = UpdateBuffers(deviceContext, posX, posY);
	if (!result) 
	{
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

int GameBitmap::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* GameBitmap::GetTexture()
{
	return m_Texture->GetTexture();
}


bool GameBitmap::InitializeBuffers(ID3D11Device *device)
{
	VertexType *vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 6;
	m_indexCount  = 6;

	vertices = new VertexType[m_vertexCount];
	indices  = new unsigned long[m_indexCount];

	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void GameBitmap::ReleaseBuffers()
{
	Utils::Release(&m_indexBuffer);
	Utils::Release(&m_vertexBuffer);
}

bool GameBitmap::UpdateBuffers(ID3D11DeviceContext *deviceContext, int posX, int posY)
{
	Rect_F rect;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;

	if ((m_prevPosX == posX) && (m_prevPosY == posY))
	{
		return true;
	}

	m_prevPosX = posX;
	m_prevPosY = posY;

	rect.left   = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(posX);
	rect.right  = rect.left + static_cast<float>(m_bitmapWidth);
	rect.top    = static_cast<float>((m_screenHeight / 2) * -1) + static_cast<float>(posY);
	rect.bottom = rect.top - static_cast<float>(m_bitmapHeight);

	vertices = new VertexType[m_vertexCount];

	//Triangle - 1
	vertices[0].position = D3DXVECTOR3(rect.left, rect.top, 0.0f);
	vertices[0].texture  = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(rect.right, rect.bottom, 0.0f);
	vertices[1].texture  = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(rect.left, rect.bottom, 0.0f);
	vertices[2].texture  = D3DXVECTOR2(0.0f, 1.0f);

	//Triangle - 2
	vertices[3].position = D3DXVECTOR3(rect.left, rect.top, 0.0f);
	vertices[3].texture  = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(rect.right, rect.top, 0.0f);
	vertices[4].texture  = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(rect.right, rect.bottom, 0.0f);
	vertices[5].texture  = D3DXVECTOR2(1.0f, 1.0f);


	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	VertexType* pVertices = reinterpret_cast<VertexType*>(mappedResource.pData);

	memcpy(pVertices, reinterpret_cast<void*>(vertices), (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void GameBitmap::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool GameBitmap::LoadTexture(ID3D11Device *device, wchar_t *filePath)
{
	bool result;

	m_Texture = new GameTexture();

	result = m_Texture->Initialize(device, filePath);
	if (!result)
	{
		return false;
	}

	return true;
}

void GameBitmap::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Release();
		delete m_Texture;
		m_Texture = nullptr;
	}
}