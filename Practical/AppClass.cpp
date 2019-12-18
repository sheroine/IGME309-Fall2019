#include "AppClass.h"
#include "MyOctant.h"

using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 60.0f), //Position
		vector3(0.0f, 0.0f, 59.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	//make bounding box for gas volume
	boxSize = 25.0f;

	//add gas particles
	particleNum = 50;
	for (int j = 0; j < particleNum; j++)
	{
		m_pEntityMngr->AddEntity("Planets\\09_Pluto.obj");
		vector3 v3Position = vector3(glm::sphericalRand(boxSize));
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
	}

	//m_uOctantLevels = 1;
	//m_pRoot = new MyOctant(m_uOctantLevels, 5);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//move the particles
	for (uint i = 0; i < particleNum; i++)
	{
		matrix4 particleMatrix = m_pEntityMngr->GetModelMatrix(i);// get the model matrix of the particle
		particleMatrix *= glm::translate(IDENTITY_M4, m_pEntityMngr->GetEntity(i)->GetVelocity()); //translate it
		m_pEntityMngr->SetModelMatrix(particleMatrix, i); //return it to its owner
	}

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//if (m_bOctVisRep)
	//{
	//	if (m_uOctantID == -1)
	//	{
	//		//display octree
	//		m_pRoot->Display();
	//	}
	//	else
	//	{
	//		m_pRoot->Display(m_uOctantID);
	//	}	
	//}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the entity manager
	m_pEntityMngr->ReleaseInstance();

	//release GUI
	ShutdownGUI();

	SafeDelete(m_pRoot);
}