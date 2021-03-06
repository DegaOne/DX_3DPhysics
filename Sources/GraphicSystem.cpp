#include "pch.h"
#include "GraphicSystem.h"
#include "ObjectSystem.h"
#include "PhysicSystem.h"
#include "Sphere.h"
#include "OrientedBoundingBox.h"
#include "Capsule.h"

#include <stdio.h>

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

//Instance
GraphicSystem * GraphicSystem::m_instance = NULL;
GraphicSystem * GraphicSystem::GetInstance()
{
	//Singleton
	if (m_instance == NULL)
	{
		m_instance = new GraphicSystem();
	}
	return m_instance;
}

//Constructor
GraphicSystem::GraphicSystem() 
	: m_pitch(0), m_yaw(3.14), m_cam(Vector3(0,0,5.f)), m_look(Vector3::Forward)
{
	m_device = NULL;
	m_deviceContext = NULL;
}
//Destructor (Singleton so..?)
GraphicSystem::~GraphicSystem() 
{

}
///Initialize components and shapes
void GraphicSystem::Initialize(ID3D11Device1* device, ID3D11DeviceContext1 * deviceContext) {

	m_device = device;
	m_deviceContext = deviceContext;
	ObjectSystem * os = ObjectSystem::GetInstance();
	vector<RigidbodyComponent*> rigidbodies = os->GetRigidbodyComponentList();
	
	m_spriteBatch = std::make_unique<SpriteBatch>(deviceContext);
	m_font = std::make_unique<SpriteFont>(device, L"Textures/myfile.spritefont");

	//@Debug volumes
	m_AABBCullingPrimitive = GeometricPrimitive::CreateBox(deviceContext, Vector3::One);
	m_sphereCullingPrimitive = GeometricPrimitive::CreateSphere(deviceContext, 0.1f);//@Taking into account we dont do sphere culling, this primitive is designed for visualizing contact points

}
//@Init device dependent components
void GraphicSystem::OnSceneLoad() {
	ObjectSystem * os = ObjectSystem::GetInstance();
	vector<RigidbodyComponent*> rigidbodies = os->GetRigidbodyComponentList();
	//@Init shapes
	for (unsigned int i = 0; i < rigidbodies.size(); i++) {
		//Downcast to create the right shape
		switch (rigidbodies[i]->m_shape->m_type) {
		case ShapeType::SPHERE:
		{
			Sphere * theSphere = static_cast<Sphere*>(rigidbodies[i]->m_shape);
			theSphere->m_primitive = GeometricPrimitive::CreateSphere(m_deviceContext, theSphere->m_radius*2.0f);
		}
		break;
		case ShapeType::OBB:
		{
			OrientedBoundingBox * theOBB = static_cast<OrientedBoundingBox*>(rigidbodies[i]->m_shape);
			theOBB->m_primitive = GeometricPrimitive::CreateBox(m_deviceContext, theOBB->m_halfExtents*2.0f);
		}
		break;
		}
	}
}
///Window size dependent resources
void GraphicSystem::InitWindow(D3D11_VIEWPORT screenViewport)
{
	//@Resize projection
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		screenViewport.Width/screenViewport.Height, 0.1f, 100.f);
}
///Draw all shapes and fonts
void GraphicSystem::Update(float dt) {
	ObjectSystem * os = ObjectSystem::GetInstance();
	PhysicSystem * ps = PhysicSystem::GetInstance();
	vector<RigidbodyComponent*> rigidbodies = os->GetRigidbodyComponentList();

	//@Euler to direction vector
	float y = sinf(m_pitch);
	float r = cosf(m_pitch);
	float z = r * cosf(m_yaw);
	float x = r * sinf(m_yaw);
	m_look = m_cam + Vector3(x, y, z);
	Matrix view = Matrix::CreateLookAt(m_cam, m_look, Vector3::Up);

	//@Render
	for (unsigned int i = 0; i < rigidbodies.size(); i++) {
		//Init each
		RigidbodyComponent * currentRb = rigidbodies[i];
		TransformComponent currentTransform = currentRb->m_owner->m_transform;
		Matrix translation = Matrix::CreateTranslation(currentTransform.m_position);
		Matrix rotation = Matrix::CreateFromQuaternion(currentTransform.m_rotation);
		Matrix world = rotation*translation;

		//@Scale set to one, for now.
		currentRb->m_shape->m_primitive->Draw( world, view, m_proj, currentRb->m_shape->m_color, nullptr, ps->m_visualizeContacts.isEnabled);
		//@WE CAN DRAW CAPSULES AS CYLINDER,SPHERE COMBINATIONS

		//@Debug draw (Wireframes) (Only take into account position and shape properties, nor rotation nor scale)
		if (ps->m_AABBCulling.isEnabled) {
			//Create local primitive
			Matrix scale = Matrix::CreateScale(currentRb->m_shape->m_AABB.m_halfExtents*2);
			//If its enabled, the PhysicsSystem, should of have computed, and updated, the AABB
			m_AABBCullingPrimitive->Draw( scale*translation, view, m_proj, currentRb->m_shape->m_AABBColor, nullptr, true);
		}
		if (ps->m_sphereCulling.isEnabled) {
			//@Similar process
		}
	}
	//@Render grid bins
	vector<AABBNode*> AABBbins = ps->m_broadPhase.GetFinalNodes(&ps->m_broadPhase.m_AABBTreeRoot);
	for (unsigned int i = 0; i < AABBbins.size(); i++) {
		
		Matrix translation = Matrix::CreateTranslation(AABBbins[i]->m_centre);
		Matrix scale = Matrix::CreateScale(AABBbins[i]->m_AABB.m_halfExtents*2);
		m_AABBCullingPrimitive->Draw(scale*translation, view, m_proj, (AABBbins[i]->m_containing.empty()) ? Colors::Red : Colors::Yellow, nullptr, true);
	}
	//@Render contacts
	if (ps->m_visualizeContacts.isEnabled) {
		
		for (unsigned int i = 0; i < ps->m_narrowPhase.m_solver.m_contactManifolds.size(); i++) {
			for (unsigned int j = 0; j < ps->m_narrowPhase.m_solver.m_contactManifolds[i].m_points.size(); j++) {
				Matrix pointMatrix = Matrix::CreateTranslation(ps->m_narrowPhase.m_solver.m_contactManifolds[i].m_points[j]);
				m_sphereCullingPrimitive->Draw(pointMatrix, view, m_proj, Colors::Green, nullptr, true);
			}
		}
	}
	//@Render fonts
	m_spriteBatch->Begin();
	const wchar_t * fps = ps->m_fps.c_str();
	const wchar_t * visualizeContacts = ps->m_visualizeContacts.log.c_str();
	const wchar_t * uniformGrid = ps->m_uniformGrid.log.c_str();
	const wchar_t * hierarchicalGrid = ps->m_hierarchicalGrid.log.c_str();
	const wchar_t * AABBCulling = ps->m_AABBCulling.log.c_str();
	m_font->DrawString(m_spriteBatch.get(), fps, Vector2(10, 10 + 0*15 ), Colors::Black);
	m_font->DrawString(m_spriteBatch.get(), L"0-2(NumPad)-load scenes, R-reload, ESC-close", Vector2(10, 10 + 1 * 15), Colors::Black);
	m_font->DrawString(m_spriteBatch.get(), visualizeContacts, Vector2(10, 10 + 2 * 15), Colors::Black);
	m_font->DrawString(m_spriteBatch.get(), uniformGrid, Vector2(10, 10 + 3 * 15), Colors::Black);
	m_font->DrawString(m_spriteBatch.get(), hierarchicalGrid, Vector2(10, 10 + 4 * 15), Colors::Black);
	m_font->DrawString(m_spriteBatch.get(), AABBCulling, Vector2(10, 10 + 5 * 15), Colors::Black);
	m_spriteBatch->End();

}
///Reset shapes and components
void GraphicSystem::Reset()
{
	//@What to do avoid device, and deviceContext pointers?
	ObjectSystem * os = ObjectSystem::GetInstance();
	vector<RigidbodyComponent*> rigidbodies = os->GetRigidbodyComponentList();

	//@Reset shapes
	for (unsigned int i = 0; i < rigidbodies.size(); i++) {
		rigidbodies[i]->m_shape->m_primitive.reset();
	}

	//@Reset spriteBatch
	m_AABBCullingPrimitive.reset();
	m_sphereCullingPrimitive.reset();
	m_spriteBatch.reset();
}
//Utility
