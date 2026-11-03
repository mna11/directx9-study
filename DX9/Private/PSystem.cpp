#include "pch.h"
#include "PSystem.h"
#include "BoundingVolume.h"

using namespace psys;

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


CPSystem::CPSystem()
	: m_Bbox(nullptr),
	m_vOrigin{}, m_fEmitRate(0.f), m_fSize(0.f), m_Tex(nullptr), m_VB(nullptr),
	m_iMaxParticles(0.f), m_dwVbSize(0), m_dwVbBatchSize(0), m_dwVbOffset(0)
{
}

CPSystem::~CPSystem() 
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
	d3d::Release<IDirect3DTexture9*>(m_Tex);
	d3d::Delete<CBoundingVolume*>(m_Bbox);
}

// 그리기에 필요한 자원을 준비
bool CPSystem::Init(wstring filename)
{
	HRESULT hr = 0;
	hr = Device->CreateVertexBuffer(
		m_dwVbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT,
		&m_VB,
		0
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexBuffer() - FAILED", L"PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		Device,
		filename.c_str(),
		&m_Tex
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateTextureFromFile() - FAILED", L"PSystem", 0);
		return false;
	}

	return true;
}

void CPSystem::reset()
{
	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		ResetParticle(&(*i));
	}
}

void CPSystem::AddParticle()
{
	Attribute attribute;
	ResetParticle(&attribute);

	m_particleList.push_back(attribute);
}

// 점을 입자처럼 보이게 설정
void CPSystem::PreRender()
{
	// 기본 조명은 끄고, 포인트 스프라이트 활성
	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true); // 포인트 스프라이트 - 정점 하나로 텍스처가 입혀진 작은 사각형 표시 방법
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);  // 카메라와의 거리에 따라 크기가 달라지게 설정
	Device->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(m_fSize));
	Device->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.f));

	Device->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.f));
	Device->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.f));
	Device->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.f));

	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CPSystem::PostRender()
{
	Device->SetRenderState(D3DRS_LIGHTING, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
}

void CPSystem::Render() 
{
	PreRender();

	// 그리기 자원 연결
	Device->SetTexture(0, m_Tex);
	Device->SetFVF(Particle::FVF);
	Device->SetStreamSource(0, m_VB, 0, sizeof(Particle));

	if (m_dwVbOffset >= m_dwVbSize)
		m_dwVbOffset = 0;

	// 정점 버퍼에서 한 묶음을 기록할 공간을 확보
	// m_dwVbOffset은 버퍼에서 다음에 쓸 위치

	Particle* v = 0;
	m_VB->Lock(
		m_dwVbOffset * sizeof(Particle),	 // 어디부터 쓸지
		m_dwVbBatchSize * sizeof(Particle),  // 얼마나 쓸지
		(void**)&v,
		m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD // 기존 데이터의 처리 방식
	); // NOOVERWRITE는 GPU 그리기 영역에 있는거 내가 안건들테니깐 그리기 기다리지 마 선언이다.
	   // DISCARD는 만약 GPU가 읽고 있는 영역에 내가 기록할려고 하면 다른 메모리 영역을 받아 기록한다.
	   // - 버퍼의 이전 내용은 유지 필요 X -> 이번부터 사용할 버퍼 내용으로 예전 데이터를 보존할 필요가 없다.

	DWORD numParticlesInBatch = 0;

	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		if (i->_isAlive)
		{
			v->_position = i->_position;
			v->_color = (D3DCOLOR)i->_color;
			++v;
			++numParticlesInBatch;

			if (numParticlesInBatch == m_dwVbBatchSize)
			{
				// 묶음이 가득 차면 잠금을 풀고 그린다.
				m_VB->Unlock();

				Device->DrawPrimitive(
					D3DPT_POINTLIST,
					m_dwVbOffset,
					m_dwVbBatchSize
				);

				m_dwVbOffset += m_dwVbBatchSize;

				if (m_dwVbOffset >= m_dwVbSize)
					m_dwVbOffset = 0;

				m_VB->Lock(
					m_dwVbOffset * sizeof(Particle),
					m_dwVbBatchSize * sizeof(Particle),
					(void**)&v,
					m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
				);

				numParticlesInBatch = 0;
			}
		}
	}

	m_VB->Unlock();

	// 마지막 묶음은 다 차지 않아도 그려야하니깐 여기서 처리함
	if (numParticlesInBatch)
	{
		Device->DrawPrimitive(
			D3DPT_POINTLIST,
			m_dwVbOffset,
			numParticlesInBatch
		);
	}

	m_dwVbOffset += m_dwVbBatchSize;

	PostRender();
}

bool CPSystem::IsEmpty()
{
	return m_particleList.empty();
}

bool CPSystem::IsDead()
{
	for (auto i = m_particleList.begin(); i != m_particleList.end(); ++i)
	{
		// 하나라도 살아있으면, 시스템은 죽지 않음
		if (i->_isAlive)
			return false;
	}
	return true;
}

void CPSystem::removeDeadParticles()
{
	for (auto i = m_particleList.begin(); i != m_particleList.end();)
	{
		if (i->_isAlive == false)
			i = m_particleList.erase(i);
		else
			++i;
	}
}