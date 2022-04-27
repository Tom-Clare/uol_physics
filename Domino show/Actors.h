#pragma once

#include "PhysicsEngine.h"
#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	class Domino : public DynamicActor
	{
	public:
		Domino(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{
			// sf of hard polyeurethene, df of hard polyerethene, c.o.r est. based on hard plastic ball (0.688) - domino is not hollow.
			PxMaterial* polyeurethene = PhysicsEngine::CreateMaterial(0.3f, .1f, 0.4f);
			CreateShape(PxBoxGeometry(PxVec3(0.01f, 0.0508f, .0254f)), 1.f, polyeurethene);
		}
	};

	class Pallet : public DynamicActor
	{
	public:
		Pallet(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{
			// sf wood on concrete, df est. wood on concrete, c.o.r est. based on hard wooden ball (0.605) - pallet is weathered, less resilient, and flatter.
			PxMaterial* wood_on_concrete = PhysicsEngine::CreateMaterial(.62f, .35f, 0.2f);
			PxVec3 plank = PxVec3(.81f, 0.0254f, 0.1016f);
			PxShape* shape;
			int top = 6;
			int support = 3;

			for (int i = 0; i < top; i++) {
				CreateShape(PxBoxGeometry(plank), 1.f, wood_on_concrete);
				shape = GetShape(i);
				shape->setLocalPose(PxTransform(PxVec3(0.f, 0.f, i * .285f)));
			}

			for (int i = 0; i < support; i++) {
				CreateShape(PxBoxGeometry(plank), 1.f, wood_on_concrete);
				shape = GetShape(top + i);
				shape->setLocalPose(PxTransform(PxVec3(-.78f + (i * .78f), -.125f, .71f), PxQuat(1.5708f, PxVec3(.0f, 1.0f, .0f)) * PxQuat(1.5708f, PxVec3(1.f, .0f, .0f))));
			}

			for (int i = 0; i < 3; i++) {
				CreateShape(PxBoxGeometry(plank), 1.f, wood_on_concrete);
				shape = GetShape(top + support + i);
				shape->setLocalPose(PxTransform(PxVec3(.0f, -.25f, i * .70f)));
			}

			this->Color(PxVec3(230.f / 255.f, 226.f / 255.f, 160.f / 255.f));

		}
	};

	/*class Crane : public DynamicActor
	{
	public:
		Crane(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{

		}
	};*/
}