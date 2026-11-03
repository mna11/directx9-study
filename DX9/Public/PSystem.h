#pragma once

class CBoundingVolume;

// 눈, 불꽃, 연기 같은 효과를 만들기 위한 파티클 시스템의 기본 클래스

namespace psys
{
	// GPU - 정점 데이터
	struct Particle
	{
		D3DXVECTOR3 _position;
		D3DCOLOR	_color;
		static const DWORD FVF;
	};

	// CPU - 입자를 움직이고 관리할 때, 필요한 데이터
	// Attribute에서 필요한 것만 Particle에 복사해서 그릴 것이다.
	struct Attribute
	{
		Attribute()
		{
			_lifeTime = 0.f;
			_age = 0.f;
			_isAlive = true;
		}

		D3DXVECTOR3 _position;		// 현재 위치
		D3DXVECTOR3 _velocity;		// 속도
		D3DXVECTOR3 _acceleration;	// 가속도 
		float		_lifeTime;		// 수명
		float		_age;			// 지난 시간
		D3DXCOLOR   _color;			// 현재 색
		D3DXCOLOR	_colorFade;		// 색상 변화에 사용할 값
		bool		_isAlive;		// 살아 있는지 여부
	};

	class CPSystem
	{
	public:
		CPSystem();
		virtual ~CPSystem();

		virtual bool Init(wstring filename);
		virtual void reset();

		virtual void ResetParticle(Attribute* attribute) = 0;
		virtual void AddParticle();

		virtual void Update(float timeDelta) = 0;

		virtual void PreRender();
		virtual void Render();
		virtual void PostRender();

		bool IsEmpty();
		bool IsDead();

	protected:
		virtual void removeDeadParticles();

	protected:
		D3DXVECTOR3				m_vOrigin;			// 입자 발생 기준 위치
		CBoundingVolume*		m_Bbox;				// 경계 볼륨
		float					m_fEmitRate;		// 생성률
		float					m_fSize;			// 입자 크기
		IDirect3DTexture9*		m_Tex;				// 텍스처
		IDirect3DVertexBuffer9* m_VB;				// 정점 버퍼
		list<Attribute>			m_particleList;		// CPU에서 관리하는 입자 목록
		int						m_iMaxParticles;	// 최대 입자 수 설정값

		DWORD m_dwVbSize;		// 버퍼 전체에 들어가는 Particle 개수
		DWORD m_dwVbOffset;		// 데이터를 쓸 시작 위치
		DWORD m_dwVbBatchSize;	// 한번에 복사하고 그릴 Particle 개수
	};
}

