#pragma once

#include <Sources/GameApp.h>
#include <Sources/Camera/GameCamera.h>
#include <Sources/Shaders/TextureShader.h>
#include <Sources/GUI/Button.h>
#include <Sources/Interface/ILevel.h>

#include <Sources/Level/TestLevel1.h>

class LobbyLevel : public ILevel {
private:

	enum Menu_ID {
		ID_GAMESTART = 0,
		ID_GAMEINTRO,
		ID_GAMEHOWTO,
		ID_GAMERANK,
		ID_GAMESHUTDOWN
	};

	D3DXVECTOR2 button_pos[5] = {
		{1520, 50},
		{1520, 250},
		{1520, 450},
		{1520, 650},
		{1520, 850}
	};

	int currSelection;
	Button m_button[5];
	CGameCamera m_Camera;

	GameBitmap m_BackgroundBitmap;
public:
	LobbyLevel();
	~LobbyLevel();

	// ILevel��(��) ���� ��ӵ�
	virtual bool Load() override;
	virtual void Unload() override;
	virtual void Update(float dt) override;
	virtual bool Render(ID3D11DeviceContext * deviceContext, int screenWidth, int screenHeight) override;
	virtual void onStart() override;
	virtual void onEnd() override;
};