#pragma once
#include "PSystem.h"

namespace psys
{
    class CFirework :
        public CPSystem
    {
    public:
        // 불꽃이 폭발할 장소 origin
        CFirework(D3DXVECTOR3* origin, int numParticles);
        void ResetParticle(Attribute* attribute) override;
        void Update(float timeDelta) override;
        void PreRender() override;
        void PostRender() override;
    };

}