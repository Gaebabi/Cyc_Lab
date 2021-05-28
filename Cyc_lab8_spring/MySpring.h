#pragma once

#include "core.h"
#include "particle.h"
#include <pfgen.h>


namespace cyclone {
	class Myspring : public ParticleForceGenerator
	{
		cyclone::Particle *other;
		double springConstant;
		double restLength;

	public:
		Myspring(cyclone::Particle * p, double springConstant, double restLenght);
		virtual void updateForce(cyclone::Particle * p, double duration);

	};
}


