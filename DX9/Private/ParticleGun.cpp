#include "pch.h"
#include "ParticleGun.h"
#include "Camera.h"

using namespace psys;

CParticleGun::CParticleGun(CCamera* pCamera)
{
	m_pCamera = pCamera;
	m_fSize = 0.8f;
	m_dwVbSize = 2048;
	m_dwVbOffset = 0;
	m_dwVbBatchSize = 512;
}

void CParticleGun::ResetParticle(Attribute* attribute)
{
	// 재활용
	attribute->_isAlive = true;
	
	D3DXVECTOR3 vCameraPos;
	m_pCamera->GetPosition(&vCameraPos);

	D3DXVECTOR3 vCameraDir;
	m_pCamera->GetLook(&vCameraDir);

	// 카메라의 위치( y는 약간 아래 )로 시작점을 잡고 카메라가 바라보는 방향으로 속도를 가짐 
	attribute->_position = vCameraPos;
	attribute->_position.y -= 1.f;
	attribute->_velocity = vCameraDir * 100.f;

	// 색깔은 녹색, 수명은 1초
	attribute->_color = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
	attribute->_age = 0.f;
	attribute->_lifeTime = 1.f;
}

void CParticleGun::Update(float timeDelta)
{
	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		i->_position += i->_velocity * timeDelta;
		i->_age += timeDelta;

		if (i->_age >= i->_lifeTime)
			i->_isAlive = false;
	}
	// 수명이 다한 경우, 지운다.
	removeDeadParticles();
}
