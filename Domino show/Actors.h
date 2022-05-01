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

	class Crane : public DynamicActor
	{
	public:
		Crane(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{
			float scale = 1.f;
			std::vector<PxVec3> leg = { // legs should be tall and wedge shaped
				PxVec3(-1.f, 0.f, -1.f) * scale, // lower points (2mx2m)
				PxVec3(-1.f, 0.f, 1.f) * scale,
				PxVec3(1.f, 0.f, -1.f) * scale,
				PxVec3(1.f, 0.f, 1.f) * scale,
				PxVec3(2.f, 4.f, -1.f) * scale, // lower ridge
				PxVec3(2.f, 4.f, 1.f) * scale,
				PxVec3(-1.f, 40.f, -1.f) * scale, // upper points (40m high)
				PxVec3(-1.f, 40.f, 1.f) * scale,
				PxVec3(3.f, 40.f, -1.f) * scale,
				PxVec3(3.f, 40.f, 1.f) * scale,
			};

			PxConvexMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)leg.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &leg.front();
			mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			mesh_desc.vertexLimit = 256;

			PxShape* shape_placehold;

			// create legs
			CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(mesh_desc)), 1.f);
			CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(mesh_desc)), 1.f);
			CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(mesh_desc)), 1.f);
			CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(mesh_desc)), 1.f);

			// position legs
			shape_placehold = GetShape(0); // far left
			shape_placehold->setLocalPose(PxTransform(PxVec3(-20.f, 0.f, -10.f)));

			shape_placehold = GetShape(1); // close left
			shape_placehold->setLocalPose(PxTransform(PxVec3(-20.f, 0.f, 10.f)));

			shape_placehold = GetShape(2); // far right
			shape_placehold->setLocalPose(PxTransform(PxVec3(20.f, 0.f, -10.f)));

			shape_placehold = GetShape(3); // close right
			shape_placehold->setLocalPose(PxTransform(PxVec3(20.f, 0.f, 10.f)));

			// create top
			PxVec3 top = PxVec3(23.f, 2.f, 10.f);
			CreateShape(PxBoxGeometry(top), 1.f);
			shape_placehold = GetShape(4);
			shape_placehold->setLocalPose(PxTransform(PxVec3(1.f, 41.f, 0.f)));

			// make it industrial orange!
			this->Color(PxVec3(255.f / 255.f, 158.f / 255.f, 3.f / 255.f));
		}
	};

	class Stand : public StaticActor
	{
	public:
		Stand(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			PxShape* shape;
			std::vector<PxVec3> legs_pos = {
				PxVec3(-0.5f, 1.f, 0.5f),
				PxVec3(0.5f, 1.f, 0.5f),
				PxVec3(-0.5f, 1.f, -0.5f),
				PxVec3(0.5f, 1.f, -0.5f)
			};

			// create legs
			PxVec3 leg_shape = PxVec3(1.f, 0.01f, 0.025f);
			for (int i = 0; i < 4; i++) {
				CreateShape(PxBoxGeometry(leg_shape));
				shape = GetShape(i);
				shape->setLocalPose(PxTransform(legs_pos[i], PxQuat(-1.5708f, PxVec3(0.f, 0.f, 1.f))));
			}

			// connect legs
			PxVec3 connector = PxVec3(.275f, 0.01f, 0.025f);

			// connect front legs
			CreateShape(PxBoxGeometry(connector));
			shape = GetShape(4);
			shape->setLocalPose(PxTransform(PxVec3(0.f, 1.5f, 0.25f)));

			// connect rear legs
			CreateShape(PxBoxGeometry(connector));
			shape = GetShape(5);
			shape->setLocalPose(PxTransform(PxVec3(0.f, 1.5f, -0.25f)));
			
			// connect each set of legs
			//CreateShape(PxBoxGeometry(connector), 1.f);
			//shape = GetShape(6);
			//shape->setLocalPose(PxTransform(PxVec3(0.f, 3.f, 0.f), PxQuat(1.5708f, PxVec3(0.f, 1.f, 0.f))));

			// join fans to connector

			// apply drive
		}
	};

	class Fan : public DynamicActor
	{
	public:
		Fan(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{
			// create fans
			PxVec3 fan_shape = PxVec3(1.f, 0.02f, 0.2f);
			std::vector<PxVec3> fans_pos = {
				PxVec3(0.f, 3.5f, 0.f), // top blade
				PxVec3(0.5f, 3.f, 0.f), // right blade
				PxVec3(0.f, 2.5f, 0.f), // bottom blade
				PxVec3(-0.5f, 3.f, 0.f) // left blade
			};

			PxShape* shape;
			CreateShape(PxBoxGeometry(fan_shape), 1.f);
			shape = GetShape(0);
			shape->setLocalPose(PxTransform(fans_pos[0], PxQuat((1.5708f), PxVec3(0.f, 0.f, 1.f))));
		}
	};

	class Fans : public DynamicActor
	{
	public:
		Fans(const PxTransform& pose = PxTransform(PxIdentity))
			: DynamicActor(pose)
		{
			// create fans
			PxVec3 fan_shape = PxVec3(1.f, 0.02f, 0.2f);
			std::vector<PxVec3> fans_pos = {
				PxVec3(0.f, 3.5f, 0.f), // top blade
				PxVec3(0.5f, 3.f, 0.f), // right blade
				PxVec3(0.f, 2.5f, 0.f), // bottom blade
				PxVec3(-0.5f, 3.f, 0.f) // left blade
			};

			PxShape* shape;
			for (int i = 0; i < 4; i++)
			{
				CreateShape(PxBoxGeometry(fan_shape), 1.f);
				shape = GetShape(0 + i);
				shape->setLocalPose(PxTransform(fans_pos[i], PxQuat((1.5708f * (i + 1)), PxVec3(0.f, 0.f, 1.f))));
			}
		}
	};

}