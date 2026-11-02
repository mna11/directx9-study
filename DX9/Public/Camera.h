#pragma once
class CCamera
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };
public:
	CCamera();
	CCamera(CameraType cameraType);
	~CCamera();
public:
	// 이동
	void Strafe(float units);
	void Fly(float units);
	void Walk(float units);

	// 회전
	void Pitch(float angle); 
	void Yaw(float angle);
	void Roll(float angle);

	// getter-setter
	void GetViewMatrix(D3DXMATRIX* V);
	void GetPosition(D3DXVECTOR3* vPos);
	void GetRight(D3DXVECTOR3* vRight);
	void GetUp(D3DXVECTOR3* vUp);
	void GetLook(D3DXVECTOR3* vLook);

	void SetCamaraType(CameraType eCameraType);
	void SetPosition(D3DXVECTOR3* vPos);


private:
	CameraType m_eCameraType;

	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vPos;
};

