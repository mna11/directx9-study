#include "pch.h"
#include "Snow.h"
#include "BoundingVolume.h"

using namespace psys;

CSnow::CSnow(CBoundingVolume* bBox, int numParticles)
{
	m_Bbox = bBox;
	m_fSize = 0.25f;
	m_dwVbSize = 2048;
	m_dwVbOffset = 0;
	m_dwVbBatchSize = 512;

	for (int i = 0; i < numParticles; ++i)
		AddParticle();
}

void CSnow::ResetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;

	d3d::GetRandomVector(
		&attribute->_position,
		&m_Bbox->GetMin(),
		&m_Bbox->GetMax()
	);

	attribute->_position.y = m_Bbox->GetMax().y;

	attribute->_velocity.x = d3d::GetRandomFloat(0.f, 1.f) * -3.f;
	attribute->_velocity.y = d3d::GetRandomFloat(0.f, 1.f) * -10.f;
	attribute->_velocity.z = 0.f;

	attribute->_color = d3d::WHITE;
}

void CSnow::Update(float timeDelta)
{
	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		i->_position += i->_velocity * timeDelta;

		if (m_Bbox->isPointInside(i->_position) == false)
		{
			ResetParticle(&(*i));
		}
	}
}
