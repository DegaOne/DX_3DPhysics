#include "pch.h"
#include "ObjectSystem.h"
#include "GraphicSystem.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

//Instance
ObjectSystem * ObjectSystem::m_instance = NULL;
ObjectSystem * ObjectSystem::GetInstance()
{
	//Singleton
	if (m_instance == NULL)
	{
		m_instance = new ObjectSystem();
	}
	return m_instance;
}

//Constructor
ObjectSystem::ObjectSystem() {
	//m_objectList.reserve(10);
	m_sceneIndex = -1;
}
//Destructor @Singleton so?
ObjectSystem::~ObjectSystem() {

}
//Init
void ObjectSystem::Initialize(ID3D11Device1* device, ID3D11DeviceContext1 * deviceContext)
{
	//@What do here? LoadSceneOne
	LoadScene(1);
}
void ObjectSystem::InitWindow(D3D11_VIEWPORT screenViewport)
{
	//@What do here?
}
//Update
void ObjectSystem::Update(float dt)
{
	//@What do here?
}
void ObjectSystem::Reset()
{
	//@What do here?
}
//Add Obj
int ObjectSystem::AddObject(GameObject * obj) {
	//@Not cache coherent at all
	m_objectList.push_back(obj);
	return 0;
}
//Remove Objs
void ObjectSystem::RemoveAllObjects() {
	//Safety check
	while (!m_objectList.empty()) {
		GameObject * curObj = m_objectList.back();
		delete curObj;
		m_objectList.pop_back();
	}
}
void ObjectSystem::RemoveObject(GameObject * object)
{
	//Remove specific object. ideally called from Component itself.
	//i.e. Sphere collider requests removing attached gameObject when it is put on a hole.
	//We give an index? a pointer to the gameObject?
	for (unsigned int i = 0; i < m_objectList.size(); i++) {
		//Compare addresses
		if (object == (m_objectList[i])) {
			//Remove
			//Swap and pop approach (NOT Needed)
			if (i < m_objectList.size() - 1) {
				swap(m_objectList[i], m_objectList.back());
			}
			GameObject * curObj = m_objectList.back();
			delete curObj;
			m_objectList.pop_back();
		}
	}
}
//Get component lists
vector< RigidbodyComponent * > ObjectSystem::GetRigidbodyComponentList() {
	//Iterate through gameobjects, find components through i.e dynamic_casts
	//Add pointers to the vector, return such vector
	vector< RigidbodyComponent * > RbCompList;
	//@Beware of vectors dynamically moving instances in memory
	for (unsigned int i = 0; i < m_objectList.size(); i++) {
		GameObject * curObj = m_objectList[i];
		//Find mesh components
		for (unsigned int j = 0; j < curObj->m_components.size(); j++) {
			//Dynamic casting to identify type;
			RigidbodyComponent * RbComp = dynamic_cast<RigidbodyComponent *>(curObj->m_components[j]);
			if (RbComp) {
				RbCompList.push_back(RbComp);
			}
		}
	}
	return RbCompList;
}

void ObjectSystem::LoadScene( unsigned int index)
{
	RemoveAllObjects();
	m_sceneIndex = index;
	switch (index) {
		case 0:
		{
			//Create some spheres
			GameObject * mySphere = new GameObject("Sphere", Vector3(-1.5, 2.0f, -3.f));
			RigidbodyComponent * rb1 = new RigidbodyComponent(0.5f, 1.0f, false, Colors::Red);
			rb1->m_force = Vector3(100.f, 0.f, 0.f);
			mySphere->AddComponent(rb1);
			AddObject(mySphere);

			GameObject * mySphere2 = new GameObject("Sphere2", Vector3(1.5, 2.0f, -3.f));
			RigidbodyComponent * rb2 = new RigidbodyComponent(0.5f, 1.0f, false, Colors::Red);
			rb2->m_force = Vector3(-100.f, 0.f, 0.f);
			mySphere2->AddComponent(rb2);
			AddObject(mySphere2);


			GameObject * mySphere3 = new GameObject("Sphere3", Vector3(-1.5, 10.0f, -7.f));
			mySphere3->AddComponent(new RigidbodyComponent(0.5f, 1.0f, false, Colors::Red));
			AddObject(mySphere3);

			GameObject * mySphere4 = new GameObject("Sphere4", Vector3(1.5, 10.0f, -7.f));
			mySphere4->AddComponent(new RigidbodyComponent(0.5f, 1.f, false, Colors::Red));
			AddObject(mySphere4);

			GameObject * mySphere5 = new GameObject("Sphere5", Vector3(-1.5, 10.0f, -12.f));
			mySphere5->AddComponent(new RigidbodyComponent(0.5f, 1.0f, false, Colors::Red));
			AddObject(mySphere5);

			GameObject * mySphere6 = new GameObject("Sphere6", Vector3(1.5, 10.0f, -12.f));
			mySphere6->AddComponent(new RigidbodyComponent(0.5f, 1.f, false, Colors::Red));
			AddObject(mySphere6);

			GameObject * mySphere7 = new GameObject("Sphere7", Vector3(-1.5, 10.0f, 3.f));
			mySphere7->AddComponent(new RigidbodyComponent(0.5f, 1.0f, false, Colors::Red));
			AddObject(mySphere7);

			GameObject * mySphere8 = new GameObject("Sphere8", Vector3(1.5, 10.0f, 3.f));
			mySphere8->AddComponent(new RigidbodyComponent(0.5f, 1.f, false, Colors::Red));
			AddObject(mySphere8);

			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					GameObject * mySphere = new GameObject("Sphere" + i + j, Vector3(0.5, 17.0f + 2 * i, 5.0f -2*j));
					mySphere->AddComponent(new RigidbodyComponent(0.5f, 1.f, false, Colors::Red));
					AddObject(mySphere);
				}
			}

			///SPHERE BOUNDS
			//Mid bottom
			GameObject * mySphereBound = new GameObject("SphereBound", Vector3(0, -11.f, -2.f));
			mySphereBound->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound);

			GameObject * mySphereBound2 = new GameObject("SphereBound2", Vector3(-15.5f, 1.f, -2.f));
			mySphereBound2->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound2);

			GameObject * mySphereBound3 = new GameObject("SphereBound3", Vector3(15.5f, 1.f, -2.f));
			mySphereBound3->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound3);

			//Front bottom
			GameObject * mySphereBound4 = new GameObject("SphereBound4", Vector3(0, -11.f, 8.f));
			mySphereBound4->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound4);

			GameObject * mySphereBound5 = new GameObject("SphereBound5", Vector3(-15.5f, 1.f, 8.f));
			mySphereBound5->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound5);

			GameObject * mySphereBound6 = new GameObject("SphereBound6", Vector3(15.5f, 1.f, 8.f));
			mySphereBound6->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound6);

			//Back bottom
			GameObject * mySphereBound7 = new GameObject("SphereBound7", Vector3(0, -11.f, -12.f));
			mySphereBound7->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound7);

			GameObject * mySphereBound8 = new GameObject("SphereBound8", Vector3(-15.5f, 1.f, -12.f));
			mySphereBound8->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound8);

			GameObject * mySphereBound9 = new GameObject("SphereBound9", Vector3(15.5f, 1.f, -12.f));
			mySphereBound9->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound9);

			//Back and front
			GameObject * mySphereBound10 = new GameObject("SphereBound10", Vector3(0, 1.f, -22.f));
			mySphereBound10->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound10);

			GameObject * mySphereBound11 = new GameObject("SphereBound11", Vector3(0, 1.f, 18.f));
			mySphereBound11->AddComponent(new RigidbodyComponent(10.f, 100.f, true));
			AddObject(mySphereBound11);

		}
		break;
		case 1:
		{

			GameObject * sphere3 = new GameObject("Sphere3", Vector3(0, -10, 0.f));
			RigidbodyComponent * rb3 = new RigidbodyComponent(10.0f, 10.f, true, DirectX::Colors::Green);
			sphere3->AddComponent(rb3);
			AddObject(sphere3);

			//Set of boxes
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					GameObject * myBox = new GameObject("Box" + i + j, Vector3(5 - 2 * i, 5.f,  -5 + 1.5 * j));
					myBox->AddComponent(new RigidbodyComponent(Vector3(.7, .7, .7)));
					AddObject(myBox);

					GameObject * myBox2 = new GameObject("Box2" + i + j, Vector3(5 - 2 * i, 7.f, -5 + 1.5 * j));
					myBox2->AddComponent(new RigidbodyComponent(Vector3(.7, .7, .7)));
					AddObject(myBox2);
				}
			}
		}
		break;
		case 2:
		{
			GameObject * sphere1 = new GameObject("Sphere1", Vector3(5.f, 1.5f, -5.f));
			RigidbodyComponent * rb1 = new RigidbodyComponent(0.5f, 20.f, false);
			sphere1->AddComponent(rb1);
			AddObject(sphere1);

			GameObject * tiltedPlane = new GameObject("OBB1", Vector3(5.f, -0.2f, -5.f), Quaternion::CreateFromYawPitchRoll(0,0,0.12f));
			RigidbodyComponent * rb2 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 10.f, true, Colors::Red);
			tiltedPlane->AddComponent(rb2);
			AddObject(tiltedPlane);

			GameObject * tiltedPlane2 = new GameObject("OBB2", Vector3(-1.f, -.7f, -5.f), Quaternion::CreateFromYawPitchRoll(0, 0, 0.05f));
			RigidbodyComponent * rb3 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 20.f, true, Colors::Red);
			tiltedPlane2->AddComponent(rb3);
			AddObject(tiltedPlane2);

			GameObject * tiltedPlane3 = new GameObject("OBB3", Vector3(-7.f, -.7f, -5.f), Quaternion::CreateFromYawPitchRoll(0, 0, -0.05f));
			RigidbodyComponent * rb4 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 20.f, true, Colors::Red);
			tiltedPlane3->AddComponent(rb4);
			AddObject(tiltedPlane3);

			GameObject * tiltedPlane4 = new GameObject("OBB4", Vector3(-10.f, -.7f, -5.f), Quaternion::CreateFromYawPitchRoll(0, 0, -0.03f));
			RigidbodyComponent * rb5 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 20.f, true, Colors::Red);
			tiltedPlane4->AddComponent(rb5);
			AddObject(tiltedPlane4);

			GameObject * tiltedPlane5 = new GameObject("OBB5", Vector3(-13.f, -2.7f, -5.f), Quaternion::CreateFromYawPitchRoll(0, 0, -0.03f));
			RigidbodyComponent * rb6 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 20.f, true, Colors::Red);
			tiltedPlane5->AddComponent(rb6);
			AddObject(tiltedPlane5);

			GameObject * tiltedPlane6 = new GameObject("OBB5", Vector3(-7.f, -2.9f, -5.f), Quaternion::CreateFromYawPitchRoll(0, 0, 0.004f));
			RigidbodyComponent * rb7 = new RigidbodyComponent(Vector3(3.f, 0.5f, 5.f), 20.f, true, Colors::Red);
			tiltedPlane6->AddComponent(rb7);
			AddObject(tiltedPlane6);
		}
	}
	//Need to reinitialize graphical components
	GraphicSystem::GetInstance()->OnSceneLoad();
	

}
