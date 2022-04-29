#include "PhysicsEngine.h"
#include <iostream>

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;

	//default error and allocator callbacks
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	//PhysX objects
	PxFoundation* foundation = 0;
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	debugger::comm::PvdConnection* pvd = 0;
#else
	PxPvd*  pvd = 0;
#endif
	PxPhysics* physics = 0;
	PxCooking* cooking = 0;

	///PhysX functions
	void PxInit()
	{
		//foundation
		if (!foundation) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#else
			foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#endif
		}

		if (!foundation)
			throw new Exception("PhysicsEngine::PxInit, Could not create the PhysX SDK foundation.");

		//visual debugger
		if (!pvd) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			pvd = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100,
				PxVisualDebuggerExt::getAllConnectionFlags());
#else
			pvd = PxCreatePvd(*foundation);
			PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
		}

		//physics
		if (!physics)
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
#else
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
#endif

		if (!physics)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the PhysX SDK.");

		if (!cooking)
			cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));

		if(!cooking)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the cooking component.");

		//create a deafult material
		CreateMaterial();
	}

	void PxRelease()
	{
		if (cooking)
			cooking->release();
		if (physics)
			physics->release();
		if (pvd)
			pvd->release();
		if (foundation)
			foundation->release();
	}

	PxPhysics* GetPhysics() 
	{ 
		return physics; 
	}

	PxCooking* GetCooking()
	{
		return cooking;
	}

	PxMaterial* GetMaterial(PxU32 index)
	{
		std::vector<PxMaterial*> materials(physics->getNbMaterials());
		if (index < physics->getMaterials((PxMaterial**)&materials.front(), (PxU32)materials.size()))
			return materials[index];
		else
			return 0;
	}

	PxMaterial* CreateMaterial(PxReal sf, PxReal df, PxReal cr) 
	{
		return physics->createMaterial(sf, df, cr);
	}

	///Actor methods

	PxActor* Actor::Get()
	{
		return actor;
	}

	void Actor::Color(PxVec3 new_color, PxU32 shape_index)
	{
		//change color of all shapes
		if (shape_index == -1)
		{
			for (unsigned int i = 0; i < colors.size(); i++)
				colors[i] = new_color;
		}
		//or only the selected one
		else if (shape_index < colors.size())
		{
			colors[shape_index] = new_color;
		}
	}

	const PxVec3* Actor::Color(PxU32 shape_indx)
	{
		if (shape_indx < colors.size())
			return &colors[shape_indx];
		else 
			return 0;			
	}

	void Actor::Material(PxMaterial* new_material, PxU32 shape_index)
	{
		std::vector<PxShape*> shape_list = GetShapes(shape_index);
		for (PxU32 i = 0; i < shape_list.size(); i++)
		{
			std::vector<PxMaterial*> materials(shape_list[i]->getNbMaterials());
			for (unsigned int j = 0; j < materials.size(); j++)
				materials[j] = new_material;
			shape_list[i]->setMaterials(materials.data(), (PxU16)materials.size());
		}
	}

	PxShape* Actor::GetShape(PxU32 index)
	{
		std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
		if (index < ((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size()))
			return shapes[index];
		else
			return 0;
	}

	std::vector<PxShape*> Actor::GetShapes(PxU32 index)
	{
		std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
		((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());
		if (index == -1)
			return shapes;
		else if (index < shapes.size())
		{
			return std::vector<PxShape*>(1, shapes[index]);
		}
		else
			return std::vector<PxShape*>();
	}

	void Actor::Name(const string& new_name)
	{
		name = new_name;
		actor->setName(name.c_str());
	}

	string Actor::Name()
	{
		return name;
	}

	DynamicActor::DynamicActor(const PxTransform& pose) : Actor()
	{
		actor = (PxActor*)GetPhysics()->createRigidDynamic(pose);
		Name("");
	}

	DynamicActor::~DynamicActor()
	{
		for (unsigned int i = 0; i < colors.size(); i++)
			delete (UserData*)GetShape(i)->userData;
	}

	void DynamicActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry,*GetMaterial());
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)actor, density);
		colors.push_back(default_color);
		//pass the color pointers to the renderer
		shape->userData = new UserData();
		for (unsigned int i = 0; i < colors.size(); i++)
			((UserData*)GetShape(i)->userData)->color = &colors[i];
	}

	void DynamicActor::CreateShape(const PxGeometry& geometry, PxReal density, PxMaterial* material)
	{
		PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry, *material);
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)actor, density);
		colors.push_back(default_color);
		//pass the color pointers to the renderer
		shape->userData = new UserData();
		for (unsigned int i = 0; i < colors.size(); i++)
			((UserData*)GetShape(i)->userData)->color = &colors[i];
	}

	void DynamicActor::SetKinematic(bool value, PxU32 index)
	{
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
		((PxRigidDynamic*)actor)->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, value);
#else
		((PxRigidDynamic*)actor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
#endif
	}

	void DynamicActor::Wake()
	{
		((PxRigidDynamic*)actor)->wakeUp();
	}

	StaticActor::StaticActor(const PxTransform& pose)
	{
		actor = (PxActor*)GetPhysics()->createRigidStatic(pose);
		Name("");
	}

	StaticActor::~StaticActor()
	{
		for (unsigned int i = 0; i < colors.size(); i++)
			delete (UserData*)GetShape(i)->userData;
	}

	void StaticActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidStatic*)actor)->createShape(geometry,*GetMaterial());
		colors.push_back(default_color);
		//pass the color pointers to the renderer
		shape->userData = new UserData();
		for (unsigned int i = 0; i < colors.size(); i++)
			((UserData*)GetShape(i)->userData)->color = &colors[i];
	}

	///Scene methods
	void Scene::Init()
	{
		//scene
		PxSceneDesc sceneDesc(GetPhysics()->getTolerancesScale());

		if(!sceneDesc.cpuDispatcher)
		{
			PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		px_scene = GetPhysics()->createScene(sceneDesc);

		if (!px_scene)
			throw new Exception("PhysicsEngine::Scene::Init, Could not initialise the scene.");

		//default gravity
		px_scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));

		CustomInit();

		pause = false;

		selected_actor = 0;

		SelectNextActor();
	}

	void Scene::Update(PxReal dt)
	{
		if (pause)
			return;

		CustomUpdate();

		px_scene->simulate(dt);
		px_scene->fetchResults(true);
	}

	void Scene::Add(Actor* actor)
	{
		px_scene->addActor(*actor->Get());
	}

	void Scene::Remove(Actor* actor)
	{
		px_scene->removeActor(*actor->Get());
	}

	PxScene* Scene::Get() 
	{ 
		return px_scene; 
	}

	void Scene::Reset()
	{
		px_scene->release();
		Init();
	}

	void Scene::Pause(bool value)
	{
		pause = value;
	}

	bool Scene::Pause() 
	{ 
		return pause;
	}

	PxRigidDynamic* Scene::GetSelectedActor()
	{
		return selected_actor;
	}

	PxRigidDynamic* Scene::GetActor(int index)
	{
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC)); // make buffer with size num of actors
		return actors[index]; // get specific actor pointer from list of actor pointers
	}

	void Scene::SelectNextActor()
	{
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC));
		if (actors.size() && (px_scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC, (PxActor**)&actors.front(), (PxU32)actors.size())))
#else
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
		if (actors.size() && (px_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, (PxActor**)&actors.front(), (PxU32)actors.size())))
#endif
		{
			if (selected_actor)
			{
				for (unsigned int i = 0; i < actors.size(); i++)
					if (selected_actor == actors[i])
					{
						HighlightOff(selected_actor);
						//select the next actor
						selected_actor = actors[(i+1)%actors.size()];
						break;
					}
			}
			else
			{
				selected_actor = actors[0];
			}
			HighlightOn(selected_actor);
		}
		else
			selected_actor = 0;
	}

	std::vector<PxActor*> Scene::GetAllActors()
	{
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
		physx::PxActorTypeSelectionFlags selection_flag = PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC |
			PxActorTypeSelectionFlag::eCLOTH;
#else
		physx::PxActorTypeFlags selection_flag = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC |
			PxActorTypeFlag::eCLOTH;
#endif
		std::vector<PxActor*> actors(px_scene->getNbActors(selection_flag));
		px_scene->getActors(selection_flag, (PxActor**)&actors.front(), (PxU32)actors.size());
		return actors;
	}

	void Scene::HighlightOn(PxRigidDynamic* actor)
	{
		//store the original colour and adjust brightness of the selected actor
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());

		sactor_color_orig.clear();

		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			PxVec3* color = ((UserData*)shapes[i]->userData)->color;
			sactor_color_orig.push_back(*color);
			//*color += PxVec3(.2f,.2f,.2f); // TODO get rid of this
		}
	}

	void Scene::HighlightOff(PxRigidDynamic* actor)
	{
		//restore the original color
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());

		for (unsigned int i = 0; i < shapes.size(); i++)
			*((UserData*)shapes[i]->userData)->color = sactor_color_orig[i];
	}

	RevoluteJoint::RevoluteJoint(Actor* first_actor, const PxTransform& first_local_anchor, Actor* second_actor, const PxTransform& second_local_actor)
	{
		PxRigidActor* px_first_actor = 0;
		if (first_actor)
			px_first_actor = (PxRigidActor*)first_actor->Get(); // prepare var for func call

		// by not doing the above check for the second actor, we allow the second actor to simply be some point in the world.

		joint = PxRevoluteJointCreate(*GetPhysics(), px_first_actor, first_local_anchor, (PxRigidActor*)second_actor->Get(), second_local_actor);
		joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true); // enable visulisation
	};

	void RevoluteJoint::DriveVelocity(PxReal vel)
	{
		//wake up the attached actors
		PxRigidDynamic* first_actor, * second_actor;
		((PxRevoluteJoint*)joint)->getActors((PxRigidActor*&)first_actor, (PxRigidActor*&)second_actor);
		if (first_actor) // if first actor is valid
		{
			if (first_actor->isSleeping()) // and currently sleeping
				first_actor->wakeUp(); // re-activate physics for object
		}
		if (second_actor) // is second actor is valid
		{
			if (second_actor->isSleeping()) // and currently sleeping
				second_actor->wakeUp(); // re-activate physics for object
		}
		((PxRevoluteJoint*)joint)->setDriveVelocity(vel); // assign drive velocity
		((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true); // and enable drive capability
	};

	PxReal RevoluteJoint::DriveVelocity()
	{
		return ((PxRevoluteJoint*)joint)->getDriveVelocity();
	};

	void RevoluteJoint::SetLimits(PxReal low, PxReal high)
	{
		((PxRevoluteJoint*)joint)->setLimit(PxJointAngularLimitPair(low, high)); // set low and high bounds of mobility
		((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true); // and enable effects of the limits
	};



	
	DistanceJoint::DistanceJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
	{
		PxRigidActor* px_actor0 = 0;
		if (actor0)
			px_actor0 = (PxRigidActor*)actor0->Get();

		joint = (PxJoint*)PxDistanceJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
		joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
		((PxDistanceJoint*)joint)->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
		Damping(1.f);
		Stiffness(1.f);
	}

	void DistanceJoint::Stiffness(PxReal value)
	{
		((PxDistanceJoint*)joint)->setStiffness(value);
	}

	PxReal DistanceJoint::Stiffness()
	{
		return ((PxDistanceJoint*)joint)->getStiffness();
	}

	void DistanceJoint::Damping(PxReal value)
	{
		((PxDistanceJoint*)joint)->setDamping(value);
	}

	PxReal DistanceJoint::Damping()
	{
		return ((PxDistanceJoint*)joint)->getDamping();
	}

	void RevoluteJoint::Weakify()
	{
		// make rope so weak it breaks instantly
		PxJoint* ref = ((Joint*)this)->Get();
		ref->setBreakForce(0, 0);
	}
}
