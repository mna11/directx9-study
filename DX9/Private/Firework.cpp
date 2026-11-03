#include "pch.h"
#include "Firework.h"

using namespace psys;

// 한 지점에서 여러 입자가 사방으로 퍼지고, 2초 뒤 사라지는 불꽃 효과

CFirework::CFirework(D3DXVECTOR3* origin, int numParticles)
{
	m_vOrigin = *origin;
	m_fSize = 0.9f;
	m_dwVbSize = 2048;
	m_dwVbOffset = 0;
	m_dwVbBatchSize = 512;

	for (int i = 0; i < numParticles; ++i)
		AddParticle();
}

void CFirework::ResetParticle(Attribute* attribute)
{
	// 파티클 초기화 -> 구체 내에서 임의의 속도를 만들며, 임의의 컬러를 부여함, 수명 2초
	attribute->_isAlive = true;
	attribute->_position = m_vOrigin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.f, -1.f, -1.f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.f, 1.f, 1.f);

	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max
	);

	// 구체를 만들기 위한 정규화
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);
	attribute->_velocity *= 10.f;
	
	// 결과적으로 방향은 다르지만, 속력은 같은 입자들이 발생 지점에서 퍼지게 된다.


	// 색깔
	attribute->_color = D3DXCOLOR(
		d3d::GetRandomFloat(0.f, 1.f),
		d3d::GetRandomFloat(0.f, 1.f),
		d3d::GetRandomFloat(0.f, 1.f),
		1.f
	);

	attribute->_age = 0.f;
	attribute->_lifeTime = 2.f;
}

void CFirework::Update(float timeDelta)
{
	// 파티클 위치 갱신, 수명이 초과한 파티클 죽음 처리
	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		if (i->_isAlive)
		{
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;
			if (i->_age >= i->_lifeTime)
				i->_isAlive = false;
		}
	}
}

void CFirework::PreRender()
{
	__super::PreRender();

	// 불꽃은 SRC와 DEST의 색을 더해서 계산한다. - 겹칠 수록 밝아짐
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 그런데 깊이 버퍼가 켜져있으면, 뒤쪽 불꽃이 더해지지 못할 수 있음
	// 깊이 기록만 끈거지 깊이 비교는 유지해서 불꽃과 다른 물체들끼리는 깊이 비교함
	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFirework::PostRender()
{
	__super::PostRender();

	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}
