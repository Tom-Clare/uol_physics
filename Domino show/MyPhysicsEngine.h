#pragma once

#include "BasicActors.h"
#include "Actors.h"
#include "PhysicsEngine.h"
#include "PxSimulationEventCallback.h"
#include <iostream>
#include <iomanip>
#include <algorithm>


namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	///Custom scene class
	class MyScene : public Scene
	{
	private:
		Plane* plane;
		BoxStatic* box1;
		CargoContainer* cargo;
		PxVec3 cargo_shape;
		RevoluteJoint* rope;
		Sphere* sphere1;
		Pallet* pallet1;
		Pallet* pallet2;
		Crane* crane;
		Box* cargo3;
		

	public:
		MySimulationEventCallback* my_callback;

		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);


			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);

			box1 = new BoxStatic(PxTransform(PxVec3(-.7f,1.f,.0f), PxQuat(-.5f, PxVec3(0.f, 0.f, 1.f))), PxVec3(2.f, .05f, 1.f));
			box1->Color(PxVec3(.6f, .6f, .6f));
			Add(box1);

			PxVec3 new_pos = PxVec3(1.2f, .1f, 0.f);
			new_pos = createDominos(new_pos, 0, 50);
			//new_pos = createDominos1(new_pos, 0, 10, 0.2f);

			crane = new Crane();
			Add(crane);

			// size of cargo container
			cargo_shape = PxVec3(6.096f, 2.59f, 2.43f);

			// attach cargo box to crane
			//CargoContainer* cargo2 = new CargoContainer(PxTransform(PxVec3(0.f, 20.f, 0.f)));
			cargo3 = new Box(PxTransform(PxVec3(0.f, 30.f, 0.f)), cargo_shape);
			cargo3->Color(PxVec3(0.f, 0.3f, 0.5f));
			this->rope = new RevoluteJoint(crane, PxTransform(PxVec3(0.f, 30.f, 0.f)), cargo3, PxTransform(PxVec3(0.f, 0.f, 0.f)));
			Add(cargo3);

			//((PxJoint*)rope)->release();

			pallet1 = new Pallet(PxTransform(PxVec3(1.f, 3.f, -3.f), PxQuat(.4f, PxVec3(0.f, 1.f, 0.f))));
			pallet2 = new Pallet(PxTransform(PxVec3(1.f, 4.f, -3.f), PxQuat(.1f, PxVec3(0.f, 1.f, 0.f))));

			Add(pallet1);
			Add(pallet2);
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
		}

		PxVec3 createDominos(PxVec3 pos, float angle, int amount) {
			for (int i = 0; i <= amount; i++) {
				pos[0] = pos[0] + .1f; // modify new position
				Domino* domino = new Domino(PxTransform(PxVec3(pos[0], pos[1], pos[2]), PxQuat(angle, PxVec3(0.f, 1.f, 0.f))));
				//Domino* domino = new Domino(PxTransform(PxVec3(x, y, z)));
				domino->Color(PxVec3(1.f, 0.f, 0.f)); // colour the domino red
				Add(domino);
			}
			return PxVec3(pos[0], pos[1], pos[2]);
		}
		
		//PxVec3 createDominos(PxVec3 pos, int amount, string direction) {  // this function doesn't really work - can't work out how to calculate it
		//	if (direction == "left")
		//	{
		//		//PxVec3 position_modifier = PxVec3(.1f, 0.f, 0.f);
		//	}
		//	else if (direction == "foreward")
		//	{
		//		PxVec3 position_modifier = PxVec3(.1f, 0.f, 0.f);
		//	}
		//	else if (direction == "right")
		//	{
		//		//PxVec3 position_modifier = PxVec3(.1f, 0.f, 0.f);
		//	}
		//
		//	for (int i = 0; i <= amount; i++) {
		//		pos[0] = pos[0] + .1f; // modify new position
		//		//angle = angle + curve; // starting angle plus new curve
		//		//Domino* domino = new Domino(PxTransform(PxVec3(pos[0], pos[1], pos[2]), PxQuat(angle, PxVec3(0.f, 1.f, 0.f))));
		//		//Domino* domino = new Domino(PxTransform(PxVec3(x, y, z)));
		//		//domino->Color(PxVec3(1.f, 0.f, 0.f)); // colour the domino red
		//		//Add(domino);
		//	}
		//	return PxVec3(pos[0], pos[1], pos[2]);
		//} // TODO create bendy lines with dominos

		void beginShow() {
			// This begins the show by pushing a "pebble" (convex mesh) from atop a pile of grit.

			float scale = 0.5f;
			std::vector<PxVec3> pebble_desc{
				PxVec3(.0f, .07f, .0f) * scale, // top most point
				PxVec3(.07f, .05f, .07f) * scale, // upper ring of points
				PxVec3(.07f, .05f, -.07f)* scale,
				PxVec3(-.07f, .05f, .07f)* scale,
				PxVec3(-.07f, .05f, -.07f)* scale,
				PxVec3(.1f, .1f, .1f)* scale, // upper middle ring of points
				PxVec3(.1f, .1f, -.1f) * scale,
				PxVec3(-.1f, .1f, .1f) * scale,
				PxVec3(-.1f, .1f, -.1f) * scale,
				PxVec3(.1f, -.1f, .1f)* scale, // lower middle ring of points
				PxVec3(.1f, -.1f, -.1f) * scale,
				PxVec3(-.1f, -.1f, .1f) * scale,
				PxVec3(-.1f, -.1f, -.1f) * scale,
				PxVec3(.07f, -.05f, .07f)* scale, //  lower ring of points
				PxVec3(.07f, -.05f, -.07f)* scale,
				PxVec3(-.07f, -.05f, .07f)* scale,
				PxVec3(-.07f, -.05f, -.07f)* scale,
				PxVec3(.0f, -.07f, .0f)* scale, // bottom most point
			};
			ConvexMesh* pebble = new ConvexMesh(pebble_desc, PxTransform(PxVec3(-1.f, 2.f, 0.f)), 1.f);
			pebble->Color(PxVec3(.75f, .75f, .75f));
			Add(pebble);
		}

		void ruinShow() {
			//cargo = new CargoContainer(PxTransform(PxVec3(5.f, 10.f, 0.f))); // 20ft x 8ft6 x 8ft
			//cargo->Color(PxVec3(0.f, 0.3f, 0.5f)); // murky blue
			//Add(cargo);

			rope->Get();
			rope->Weakify();
			cargo3->Prod();
			/*PxReal min;
			PxReal max;
			rope->getBreakForce(min, max);
			cout << min;
			cout << max;

			bool broken = (rope->getConstraintFlags() & PxConstraintFlag::eBROKEN) != 0;*/

		}
	};
}
