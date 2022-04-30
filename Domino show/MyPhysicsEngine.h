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
		Box* plank;
		Box* cargo;
		PxVec3 cargo_shape;
		DistanceJoint* rope;
		Sphere* sphere1;
		Pallet* pallet1;
		Pallet* pallet2;
		Box* cargo3;
		Crane* crane;
		Box* crate;
		

	public:
		MySimulationEventCallback* my_callback;
		int num_domino = 50;

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

			// Initialise some standard vars
			cargo_shape = PxVec3(6.096f, 2.59f, 2.43f);

			PxVec3 col_cardboard = PxVec3(194.f / 255.f, 172.f / 255.f, 122.f / 255.f); // pale grey-brown

			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);

			crate = new Box(PxTransform(PxVec3(-7.f, 6.f, 0.f)), PxVec3(1.f, 0.8f, 1.f));
			crate->Color(col_cardboard);
			Add(crate);

			plank = new Box(PxTransform(PxVec3(-5.2f,7.f,.0f), PxQuat(-.6f, PxVec3(0.f, 0.f, 1.f))), PxVec3(2.f, .05f, 1.f));
			plank->Color(PxVec3(230.f / 255.f, 226.f / 255.f, 160.f / 255.f));
			Add(plank);

			cargo = new Box(PxTransform(PxVec3(-2.f, 2.6f, 0.f)), cargo_shape, 0.05);
			cargo->Color(PxVec3(199.f / 255.f, 73.f / 255.f, 0.f / 255.f));
			Add(cargo);

			PxVec3 new_pos = PxVec3(-3.2f, 5.42f, 0.f);
			new_pos = createDominos(new_pos, 0, num_domino);
			//new_pos = createDominos1(new_pos, 0, 10, 0.2f);

			crane = new Crane();
			Add(crane);

			// attach cargo box to crane
			//CargoContainer* cargo2 = new CargoContainer(PxTransform(PxVec3(0.f, 20.f, 0.f)));
			cargo3 = new Box(PxTransform(PxVec3(0.f, 30.f, 0.f)), cargo_shape, 0.05);
			cargo3->Color(PxVec3(0.f, 0.3f, 0.5f));
			this->rope = new DistanceJoint(crane, PxTransform(PxVec3(0.f, 40.f, 0.f)), cargo3, PxTransform(PxVec3(0.f, 10.f, 0.f)));
			Add(cargo3);

			pallet1 = new Pallet(PxTransform(PxVec3(1.f, 1.f, 4.f), PxQuat(.4f, PxVec3(0.f, 1.f, 0.f))));
			pallet2 = new Pallet(PxTransform(PxVec3(1.f, 2.f, 4.f), PxQuat(.1f, PxVec3(0.f, 1.f, 0.f))));

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
				domino->Color(PxVec3(.0f, 0.f, 1.f)); // colour the domino red
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

			float scale = .1f;
			std::vector<PxVec3> pebble_desc{
				PxVec3(.0f, .9f, .0f) * scale, // top most point
				PxVec3(.7f, .5f, .7f) * scale, // upper ring of points
				PxVec3(.7f, .5f, -.7f)* scale,
				PxVec3(-.7f, .5f, .7f)* scale,
				PxVec3(-.7f, .5f, -.7f)* scale,
				PxVec3(1.f, .1f, 1.f)* scale, // upper middle ring of points
				PxVec3(1.f, .1f, -1.f) * scale,
				PxVec3(-1.f, .1f, 1.f) * scale,
				PxVec3(-1.f, .1f, -1.f) * scale,
				PxVec3(1.f, -.1f, 1.f)* scale, // lower middle ring of points
				PxVec3(1.f, -.1f, -1.f) * scale,
				PxVec3(-1.f, -.1f, 1.f) * scale,
				PxVec3(-1.f, -.1f, -1.f) * scale,
				PxVec3(.7f, -.5f, .7f)* scale, //  lower ring of points
				PxVec3(.7f, -.5f, -.7f)* scale,
				PxVec3(-.7f, -.5f, .7f)* scale,
				PxVec3(-.7f, -.5f, -.7f)* scale,
				PxVec3(.0f, -.9f, .0f)* scale, // bottom most point
			};
			ConvexMesh* pebble = new ConvexMesh(pebble_desc, PxTransform(PxVec3(-6.5f, 8.f, 0.f)), 1.f);
			pebble->Color(PxVec3(.75f, .75f, .75f));
			Add(pebble);
		}

		void ruinShow() {
			rope->Weakify(); // weaken the rop so it snaps instantly
			cargo3->Wake(); // waken the cargo box and cause it to realise it needs to fall
		}
	};
}
