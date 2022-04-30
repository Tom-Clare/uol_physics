#pragma once

#include <vector>
#include "PxPhysicsAPI.h"
#include "Exception.h"
#include "Extras\UserData.h"
#include <string>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;
	
	///Initialise PhysX framework
	void PxInit();

	///Release PhysX resources
	void PxRelease();

	///Get the PxPhysics object
	PxPhysics* GetPhysics();

	///Get the cooking object
	PxCooking* GetCooking();

	///Get the specified material
	PxMaterial* GetMaterial(PxU32 index=0);

	///Create a new material
	PxMaterial* CreateMaterial(PxReal sf=.0f, PxReal df=.0f, PxReal cr=.0f);

	static const PxVec3 default_color(.8f,.8f,.8f);

	///Abstract Actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		PxActor* actor;
		std::vector<PxVec3> colors;
		std::string name;

	public:
		///Constructor
		Actor()
			: actor(0)
		{
		}

		PxActor* Get();

		void Color(PxVec3 new_color, PxU32 shape_index=-1);

		const PxVec3* Color(PxU32 shape_indx=0);

		void Actor::Name(const string& name);

		string Actor::Name();

		void Material(PxMaterial* new_material, PxU32 shape_index=-1);

		PxShape* GetShape(PxU32 index=0);

		std::vector<PxShape*> Actor::GetShapes(PxU32 index=-1);

		virtual void CreateShape(const PxGeometry& geometry, PxReal density) {}
	};

	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const PxTransform& pose);

		~DynamicActor();

		void CreateShape(const PxGeometry& geometry, PxReal density);
		void CreateShape(const PxGeometry& geometry, PxReal density, PxMaterial* material);

		void SetKinematic(bool value, PxU32 index=-1);

		void Wake();
	};

	class StaticActor : public Actor
	{
	public:
		StaticActor(const PxTransform& pose);

		~StaticActor();

		void CreateShape(const PxGeometry& geometry, PxReal density=0.f);
	};

	///Generic scene class
	class Scene
	{
	protected:
		//a PhysX scene object
		PxScene* px_scene;
		//pause simulation
		bool pause;
		//selected dynamic actor on the scene
		PxRigidDynamic* selected_actor;
		//original and modified colour of the selected actor
		std::vector<PxVec3> sactor_color_orig;

		void HighlightOn(PxRigidDynamic* actor);

		void HighlightOff(PxRigidDynamic* actor);

	public:
		///Init the scene
		void Init();

		///User defined initialisation
		virtual void CustomInit() {}

		///Perform a single simulation step
		void Update(PxReal dt);

		///User defined update step
		virtual void CustomUpdate() {}

		///Add and remove actors
		void Add(Actor* actor);
		void Remove(Actor* actor);

		///Get the PxScene object
		PxScene* Get();

		///Reset the scene
		void Reset();

		///Set pause
		void Pause(bool value);

		///Get pause
		bool Pause();

		///Get the selected dynamic actor on the scene
		PxRigidDynamic* GetSelectedActor();

		PxRigidDynamic* GetActor(int index);

		///Switch to the next dynamic actor
		void SelectNextActor();

		///a list with all actors
		std::vector<PxActor*> GetAllActors();
	};

	///Generic Joint class, simple wrapper for PxJoint
	class Joint
	{
	protected:
		PxJoint* joint;

	public:
		Joint() : joint(0) {}

		PxJoint* Get() { return joint; }
	};

	class RevoluteJoint : public Joint
	{
	public:
		RevoluteJoint(Actor*, const PxTransform&, Actor*, const PxTransform&);

		void DriveVelocity(PxReal);

		PxReal DriveVelocity();

		void SetLimits(PxReal, PxReal);

		void Weakify();
	};

	class DistanceJoint : public Joint
	{
	public:
		DistanceJoint(Actor*, const PxTransform&, Actor*, const PxTransform&);

		void Weakify();
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count)
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						//cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						//cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
		{
			//cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					//cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					//cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
		virtual void onWake(PxActor** actors, PxU32 count) {}
		virtual void onSleep(PxActor** actors, PxU32 count) {}
#if PX_PHYSICS_VERSION >= 0x304000
		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
#endif
	};
}
