#include "pch.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_eCameraType = AIRCRAFT;

	m_vPos		= D3DXVECTOR3{ 0.f, 0.f, 0.f };
	m_vRight	= D3DXVECTOR3{ 1.f, 0.f, 0.f };
	m_vUp		= D3DXVECTOR3{ 0.f, 1.f, 0.f };
	m_vLook		= D3DXVECTOR3{ 0.f, 0.f, 1.f };
}

CCamera::CCamera(CameraType cameraType)
{
	m_eCameraType = cameraType;

	m_vPos = D3DXVECTOR3{ 0.f, 0.f, 0.f };
	m_vRight = D3DXVECTOR3{ 1.f, 0.f, 0.f };
	m_vUp = D3DXVECTOR3{ 0.f, 1.f, 0.f };
	m_vLook = D3DXVECTOR3{ 0.f, 0.f, 1.f };
}

CCamera::~CCamera()
{
}

void CCamera::Strafe(float units)
{
	// LANDOBJECT의 경우, xz 평면상 이동한다. 
	switch (m_eCameraType)
	{
	case LANDOBJECT:
		m_vPos += D3DXVECTOR3(m_vRight.x, 0.f, m_vRight.z) * units;
		break;
	case AIRCRAFT:
		m_vPos += m_vRight * units;
		break;
	default:
		break;
	}
}

void CCamera::Fly(float units)
{
	// LANDOBJECT의 경우, 월드 y좌표 기준으로 이동한다. 
	switch (m_eCameraType)
	{
	case LANDOBJECT:
		m_vPos.y += units;
		break;
	case AIRCRAFT:
		m_vPos += m_vUp * units;
		break;
	default:
		break;
	}
}

void CCamera::Walk(float units)
{
	// LANDOBJECT의 경우, xz 평면상 이동한다. 
	switch (m_eCameraType)
	{
	case LANDOBJECT:
		m_vPos += D3DXVECTOR3(m_vLook.x, 0.f, m_vLook.z) * units;
		break;
	case AIRCRAFT:
		m_vPos += m_vLook * units;
		break;
	default:
		break;
	}
}

void CCamera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vRight, angle);

	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &T);
	D3DXVec3TransformNormal(&m_vLook, &m_vLook, &T);
}

void CCamera::Yaw(float angle)
{
	D3DXMATRIX T;

	// LANDOBJ의 경우 회전은 월드 y축 기준
	switch (m_eCameraType)
	{
	case LANDOBJECT:
		D3DXMatrixRotationY(&T, angle);
		break;
	case AIRCRAFT:
		D3DXMatrixRotationAxis(&T, &m_vUp, angle);
		break;
	default:
		break;
	}

	D3DXVec3TransformNormal(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformNormal(&m_vLook, &m_vLook, &T);
}

void CCamera::Roll(float angle)
{
	// LANDOBJECT는 z축 회전 X
	if (m_eCameraType == LANDOBJECT)
		return;	

	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vLook, angle);

	D3DXVec3TransformNormal(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &T);
}

void CCamera::GetViewMatrix(D3DXMATRIX* V)
{
	// 카메라 좌표계 기저벡터 Orthonormal 관계 유지
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	// 뷰 행렬 만들기
	float x = -D3DXVec3Dot(&m_vRight, &m_vPos);
	float y = -D3DXVec3Dot(&m_vUp, &m_vPos);
	float z = -D3DXVec3Dot(&m_vLook, &m_vPos);

	(*V)(0, 0) = m_vRight.x; (*V)(0, 1) = m_vUp.x; (*V)(0, 2) = m_vLook.x; (*V)(0, 3) = 0.f;
	(*V)(1, 0) = m_vRight.y; (*V)(1, 1) = m_vUp.y; (*V)(1, 2) = m_vLook.y; (*V)(1, 3) = 0.f;
	(*V)(2, 0) = m_vRight.z; (*V)(2, 1) = m_vUp.z; (*V)(2, 2) = m_vLook.z; (*V)(2, 3) = 0.f;
	(*V)(3, 0) = x;			 (*V)(3, 1) =y;		   (*V)(3, 2) = z;		   (*V)(3, 3) = 1;


	// 이렇게하면 LookAtLH를 쓸 수 있다.
	// 그런데 이미 관리하고 있는 벡터를 다시 계산하는 과정을 거쳐서 자연스러운 건 아님
	// 근데 할 수는 있다는 걸 알고 있으면 된다.
	/*D3DXVECTOR3 m_vTarget = m_vPos + m_vLook;
	D3DXMatrixLookAtLH(
		V, &m_vPos, &m_vTarget, &m_vUp
	);*/
}

void CCamera::GetPosition(D3DXVECTOR3* vPos)
{
	*vPos = m_vPos;
}

void CCamera::GetRight(D3DXVECTOR3* vRight)
{
	*vRight = m_vRight;
}

void CCamera::GetUp(D3DXVECTOR3* vUp)
{
	*vUp = m_vUp;
}

void CCamera::GetLook(D3DXVECTOR3* vLook)
{
	*vLook = m_vLook;
}

void CCamera::SetCamaraType(CameraType eCameraType)
{
	m_eCameraType = eCameraType;
}

void CCamera::SetPosition(D3DXVECTOR3* vPos)
{
	m_vPos = *vPos;
}