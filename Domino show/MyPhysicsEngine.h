#pragma once

#include "BasicActors.h"
#include "Actors.h"
#include "PhysicsEngine.h"
#include "PxSimulationEventCallback.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>


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
		Box* plank2;
		Box* cargo;
		PxVec3 cargo_shape;
		DistanceJoint* rope;
		Sphere* sphere1;
		Pallet* pallet1;
		Pallet* pallet2;
		Box* cargo3;
		Crane* crane;
		Box* cardboard_box;
		Stand* windmill_stand;
		Fans* windmill_fans;
		RevoluteJoint* windmill_connection;
		RevoluteJoint* windmill_connection2;
		std::vector<PxVec3> cargo_colours;
		std::vector<Cloth*> papers;
		std::vector<float> scales;

	public:
		MySimulationEventCallback* my_callback;
		int num_domino = 93;

		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit()
		{
			SetVisualisation();

			GetMaterial()->setDynamicFriction(.2f);

			// Initialise some standard vars
			cargo_shape = PxVec3(3.048f, 1.295f, 1.215f);
			cargo_colours = {
				PxVec3(199.f / 255.f, 73.f / 255.f, 0.f / 255.f), // maroon
				PxVec3(0.f, 0.3f, 0.5f), // murky blue
				PxVec3(211.f / 255.f, 222.f / 255.f, 222.f / 255.f), // silver-blue
				PxVec3(250.f / 255.f, 237.f / 255.f, 125.f / 255.f), // weak yellow
				PxVec3(96.f / 255.f, 192.f / 255.f, 224.f / 255.f) // 96, 192, 224
			};

			PxVec3 col_cardboard = PxVec3(194.f / 255.f, 172.f / 255.f, 122.f / 255.f); // pale grey-brown

			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			plane = new Plane();
			plane->Color(PxVec3(140.f / 255.f, 140.f / 255.f, 140.f / 255.f));
			Add(plane);

			cardboard_box = new Box(PxTransform(PxVec3(-4.5f, 3.f, 0.f)), PxVec3(.5f, 0.4f, .5f));
			cardboard_box->Color(col_cardboard);
			Add(cardboard_box);

			plank = new Box(PxTransform(PxVec3(-3.7f, 4.f, .0f), PxQuat(-.8f, PxVec3(0.f, 0.f, 1.f))), PxVec3(1.f, .025f, .5f));
			plank->Color(PxVec3(230.f / 255.f, 226.f / 255.f, 160.f / 255.f));
			Add(plank);

			cargo = new Box(PxTransform(PxVec3(-2.f, 1.29f, 0.f)), cargo_shape, 0.05);
			cargo->Color(cargo_colours[0]);
			Add(cargo);

			plank2 = new Box(PxTransform(PxVec3(-1.f, 2.595f, .0f)), PxVec3(3.f, .01f, 1.f));
			plank2->Color(PxVec3(235.f / 255.f, 216.f / 255.f, 140.f / 255.f));
			Add(plank2);

			PxVec3 new_pos = PxVec3(-2.72f, 2.65f, 0.f);
			new_pos = createDominos(new_pos, 0, num_domino);
			createDominos(PxVec3(3.6f, 0.01f, 0.f), 0, 40);

			crane = new Crane();
			Add(crane);

			// attach cargo box to crane
			//CargoContainer* cargo2 = new CargoContainer(PxTransform(PxVec3(0.f, 20.f, 0.f)));
			cargo3 = new Box(PxTransform(PxVec3(4.f, 30.f, 0.f)), cargo_shape, 0.05);
			cargo3->Color(cargo_colours[1]);
			this->rope = new DistanceJoint(nullptr, PxTransform(PxVec3(4.f, 40.f, 0.f)), cargo3, PxTransform(PxVec3(0.f, 0.f, 0.f)));
			Add(cargo3);
			rope->setDistance(10.f);

			pallet1 = new Pallet(PxTransform(PxVec3(1.f, 1.f, 4.f), PxQuat(.4f, PxVec3(0.f, 1.f, 0.f))));
			pallet2 = new Pallet(PxTransform(PxVec3(1.f, 2.f, 4.f), PxQuat(.1f, PxVec3(0.f, 1.f, 0.f))));

			Add(pallet1);
			Add(pallet2);

			windmill_stand = new Stand(PxTransform(PxVec3(2.5f, .0f, 0.f)));
			Add(windmill_stand);
			windmill_fans = new Fans(PxTransform(PxVec3(2.5f, 0.f, 0.f), PxQuat(1.5807f, PxVec3(1.f, 0.f, 0.f))));
			Add(windmill_fans);

			PxQuat rot = PxQuat(1.5807f, PxVec3(0.f, 1.f, 0.f)) * PxQuat(1.5807f, PxVec3(1.f, 0.f, 0.f));
			windmill_connection = new RevoluteJoint(nullptr, PxTransform(PxVec3(2.5f, 1.5f, 0.f), PxQuat(1.5807f, PxVec3(0.f, 1.f, 0.f))), windmill_fans, (PxTransform(PxVec3(0.f, 0.f, 0.f), rot)));
			windmill_connection->DriveVelocity(.1f);
			// make rope so weak it breaks instantly
			PxJoint* ref = ((Joint*)windmill_connection)->Get();
			ref->setBreakForce(10.f, 10.f);

			// bavkground cargo boxes
			BackgroundCargo();

			//Capsule* covered_box = new Capsule(PxTransform(PxVec3(7.f, .75f, -10.f)), PxVec2(0.5f, .4f));
			//covered_box->Color(cargo_colours[4]); // 96, 192, 224
			//Add(covered_box);

			Box* cargo2 = new Box(PxTransform(PxVec3(-9.5f, 1.29f, 2.5f)), cargo_shape, 0.05);
			cargo2->Color(cargo_colours[1]);
			Add(cargo2);
			PxTransform noticeboard_pos = PxTransform(PxVec3(-6.449825f, 1.5f, 2.5f), PxQuat(1.5807f, PxVec3(0.f, 1.f, 0.f)));
			BoxStatic* noticeboard = new BoxStatic(noticeboard_pos, PxVec3(1.f, .75f, 0.01275f), 0.05);
			noticeboard->Color(PxVec3(84.f / 255.f, 43.f / 255.f, 0.f / 255.f)); // 84, 43, 0
			Add(noticeboard);
			PxTransform noticeboard_surface = PxTransform(PxVec3(noticeboard_pos.p[0], noticeboard_pos.p[1], noticeboard_pos.p[2] + 0.01275f), noticeboard_pos.q);
			PopulateNoticeboard(noticeboard_surface);

		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			//// vary wind applied to flag
			float max = 1; // divide random number by upper bound to find random float
			float force = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));

			if (papers.size() == scales.size()) { // iteration between the two is valid
				for (std::vector<Cloth*>::size_type i = 0; i != papers.size(); i++) { // for every paper
					papers[i]->setExternalAcceleration(PxVec3((3 * scales[i]) + force, 0.f, force));
				}
			}
		}

		PxVec3 createDominos(PxVec3 pos, float angle, int amount) {
			for (int i = 0; i <= amount; i++) {
				pos[0] = pos[0] + .05f; // modify new position
				Domino* domino = new Domino(PxTransform(PxVec3(pos[0], pos[1], pos[2]), PxQuat(angle, PxVec3(0.f, 1.f, 0.f))));
				//Domino* domino = new Domino(PxTransform(PxVec3(x, y, z)));
				domino->Color(PxVec3(.0f, 0.f, 1.f)); // colour the domino red
				Add(domino);
			}
			return PxVec3(pos[0], pos[1], pos[2]);
		}

		void BackgroundCargo()
		{
			float height = 1.29f;
			float depth = 2.43f;
			float gap = 1.f;
			std::vector<PxVec3> positions = {
				PxVec3(-10.f, height * 1, -25.f), // right lower row
				PxVec3(-10.f, height * 1, -25.f + depth + gap),
				PxVec3(-10.f, height * 1, -25.f + (depth + gap) * 2),
				PxVec3(-10.f, height * 1, -25.f + (depth + gap) * 3),
				PxVec3(-17.f, height * 1, -25.f), // left lower row
				PxVec3(-17.f, height * 1, -25.f + depth + gap),
				PxVec3(-17.f, height * 1, -25.f + (depth + gap) * 2),
				PxVec3(-17.f, height * 1, -25.f + (depth + gap) * 3),
			};

			Box* back_cargo;
			for (int i = 0; i < 8; i++)
			{
				back_cargo = new Box(PxTransform(positions[i]), cargo_shape, 0.05f);
				back_cargo->Color(cargo_colours[std::rand() % 5]);
				Add(back_cargo);
			}
			
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

		void PopulateNoticeboard(PxTransform pos)
		{
			// placeholder cloth object
			Cloth* paper;
			PxVec3 sticky_colour = PxVec3(247.f/255.f, 255.f/255.f, 23.f/255.f); //247, 255, 23

			//notepad
			PxTransform notepad_pos = PxTransform(PxVec3(pos.p[0]+0.01f, pos.p[1] + .1f, pos.p[2] + .2f),pos.q);
			paper = new Cloth(notepad_pos, PxVec2(.15f, .3f), 2, 10, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(PxVec3(1.f, 1.f, 1.f));
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(1.f);

			paper = new Cloth(notepad_pos, PxVec2(.15f, .3f), 2, 10, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(PxVec3(1.f, 1.f, 1.f));
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(0.5f);

			//notepad 2
			notepad_pos = PxTransform(PxVec3(pos.p[0] + 0.01f, pos.p[1] + .3f, pos.p[2] + .5f), pos.q);
			paper = new Cloth(notepad_pos, PxVec2(.15f, .3f), 2, 10, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(PxVec3(252.f / 255.f, 255.f / 255.f, 168.f / 255.f)); // soft yellow
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(1.f);

			paper = new Cloth(notepad_pos, PxVec2(.15f, .3f), 2, 10, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(PxVec3(252.f / 255.f, 255.f / 255.f, 168.f / 255.f)); // soft yellow
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(0.5f);

			// sticky note
			paper = new Cloth(PxTransform(PxVec3(pos.p[0] + 0.01f, pos.p[1] + .5f, pos.p[2] - .4f), pos.q), PxVec2(.1f, .1f), 3, 3, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(sticky_colour);
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(1.f);

			// sticky note
			paper = new Cloth(PxTransform(PxVec3(pos.p[0] + 0.01f, pos.p[1] + .52f, pos.p[2] - .55f), pos.q), PxVec2(.1f, .1f), 3, 3, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(sticky_colour);
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(1.f);
			
			// sticky note
			paper = new Cloth(PxTransform(PxVec3(pos.p[0] + 0.01f, pos.p[1] + .44f, pos.p[2] - .63f), pos.q), PxVec2(.1f, .1f), 3, 3, true);
			paper->setFrictionCoefficient(0.6);
			paper->Color(sticky_colour);
			Add(paper);
			paper->setClothFlag(PxClothFlag::eSCENE_COLLISION, false);
			papers.push_back(paper);
			scales.push_back(1.f);
		};

		void beginShow() {
			// This begins the show by creating a "pebble" (convex mesh) atop a plank.

			float scale = .05f;
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
			ConvexMesh* pebble = new ConvexMesh(pebble_desc, PxTransform(PxVec3(-3.5f, 4.f, 0.f), PxQuat(1.5807f, PxVec3(0.f, 1.f, 0.f))), 1.f);
			pebble->Color(PxVec3(.75f, .75f, .75f));
			Add(pebble);
		}

		void ruinShow() {
			rope->Weakify(); // weaken the rop so it snaps instantly
			cargo3->Wake(); // waken the cargo box and cause it to realise it needs to fall
		}
	};
}
