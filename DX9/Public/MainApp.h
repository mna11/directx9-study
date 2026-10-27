#pragma once

class CMainApp
{
public:
	CMainApp() = default;
	~CMainApp();
public: 
	bool Setup();
	void Cleanup();
	void Update(float timeDelta);
	bool Display();
};

