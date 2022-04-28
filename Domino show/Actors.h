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

	class Crane : public StaticActor
	{
	public:
		Crane(const PxTransform& pose = PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			float scale = 1.f;
			std::vector<PxVec3> leg = { // legs should be tall and wedge shaped
				PxVec3(-1.f, 0.f, -1.f) * scale, // lower points (2mx2m)
				PxVec3(-1.f, 0.f, 1.f) * scale,
				PxVec3(1.f, 0.f, -1.f) * scale,
				PxVec3(1.f, 0.f, 1.f) * scale,
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


}