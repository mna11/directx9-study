#pragma once
#include "PSystem.h"

class CCamera;

namespace psys
{
    class CParticleGun :
        public CPSystem
    {
    public:
        CParticleGun(CCamera* pCamera);
        void ResetParticle(Attribute* attribute) override;
        void Update(float timeDelta) override;
    private:
        CCamera* m_pCamera;
    };
}

