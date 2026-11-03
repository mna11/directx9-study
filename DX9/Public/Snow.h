#pragma once
#include "PSystem.h"

namespace psys
{
    class CSnow :
        public CPSystem
    {
    public:
        CSnow(CBoundingVolume* bBox, int numParticles);
        void ResetParticle(Attribute* attribute) override;
        void Update(float timeDelta) override;
    };
}

