#pragma once

class CCamera;

namespace psys {
	class CPSystem;
}

class CMainApp
{
public:
	enum PARTICLE_TYPE { SNOW, FIREWORK, GUN, END };
public:
	CMainApp() = default;
	~CMainApp();
public: 
	bool Setup();
	void Cleanup();
	void Update(float timeDelta);
	bool Display();
private:
	bool DrawBasicScene(bool bClear, float fScale);	
	void KeyInput(float timeDelta);
private:
	PARTICLE_TYPE m_eParticleType{ GUN };
	CCamera* m_pCamera{nullptr};
	psys::CPSystem* m_pParticle{nullptr};
};

