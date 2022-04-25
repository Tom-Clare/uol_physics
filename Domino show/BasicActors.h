#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Plane class
	class Plane : public StaticActor
	{
	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 normal=PxVec3(0.f, 1.f, 0.f), PxReal distance=0.f) 
			: StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
		{
			CreateShape(PxPlaneGeometry());
		}
	};

	///Sphere class
	class Sphere : public DynamicActor
	{
	public:
		//a sphere with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m
		// - denisty: 1kg/m^3
		Sphere(const PxTransform& pose=PxTransform(PxIdentity), PxReal radius=1.f, PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			CreateShape(PxSphereGeometry(radius), density);
		}
	};

	///Box (dynamic) class
	class Box : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(const PxTransform& pose=PxTransform(PxIdentity), PxVec3 dimensions=PxVec3(.5f,.5f,.5f), PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	///Box (dynamic) class
	class CargoContainer : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		CargoContainer(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			// sf steel, df steel, c.o.r est. based on hard wooden ball (0.605) - pallet is weathered, less resilient, and flatter.
			PxMaterial* painted_metal = PhysicsEngine::CreateMaterial(.5f, .42f, 0.85f);
			CreateShape(PxBoxGeometry(dimensions), density, painted_metal);
		}
	};

	///Box (static) class
	class BoxStatic : public StaticActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		BoxStatic(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
			: StaticActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	class Capsule : public DynamicActor
	{
	public:
		Capsule(const PxTransform& pose=PxTransform(PxIdentity), PxVec2 dimensions=PxVec2(1.f,1.f), PxReal density=1.f) 
			: DynamicActor(pose)
		{
			CreateShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
		}
	};

	///The ConvexMesh class
	class ConvexMesh : public DynamicActor
	{
	public:
		//constructor
		ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose=PxTransform(PxIdentity), PxReal density=1.f)
			: DynamicActor(pose)
		{
			PxConvexMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			mesh_desc.vertexLimit = 256;

			CreateShape(PxConvexMeshGeometry(CookMesh(mesh_desc)), density);
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh(const PxConvexMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookConvexMesh(mesh_desc, stream))
				throw new Exception("ConvexMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}
	};

	///The TriangleMesh class
	class TriangleMesh : public StaticActor
	{
	public:
		//constructor
		TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose=PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			PxTriangleMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.triangles.count = (PxU32)trigs.size()/3;
			mesh_desc.triangles.stride = 3*sizeof(PxU32);
			mesh_desc.triangles.data = &trigs.front();

			CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
		}

		//mesh cooking (preparation)
		PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookTriangleMesh(mesh_desc, stream))
				throw new Exception("TriangleMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createTriangleMesh(input);
		}
	};

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
				shape->setLocalPose(PxTransform(PxVec3(0.f, 0.f, i*.285f)));
			}

			for (int i = 0; i < support; i++) {
				CreateShape(PxBoxGeometry(plank), 1.f, wood_on_concrete);
				shape = GetShape(top + i);
				shape->setLocalPose(PxTransform(PxVec3(-.78f + (i * .78f), -.125f, .71f), PxQuat(1.5708f, PxVec3(.0f, 1.0f, .0f))* PxQuat(1.5708f, PxVec3(1.f, .0f, .0f))));
			}

			for (int i = 0; i < 3; i++) {
				CreateShape(PxBoxGeometry(plank), 1.f, wood_on_concrete);
				shape = GetShape(top + support + i);
				shape->setLocalPose(PxTransform(PxVec3(.0f, -.25f, i * .70f)));
			}

			this->Color(PxVec3(230.f / 255.f, 226.f / 255.f, 160.f / 255.f));

		}
	};
}