#include "AppClass.h"
void Application::InitVariables(void)
{
	//make an aray that stores the cubes
	//MyMesh* cubeArray[46];

	//make the cubes
	for (int i = 0; i < 46; i++)
	{
		cubeArray[i] = new MyMesh();
		cubeArray[i]->GenerateCube(1.0f, C_BLACK);
	}

	////Make MyMesh object
	//m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(2.0f, C_BROWN);

	////Make MyMesh object
	//m_pMesh1 = new MyMesh();
	//m_pMesh1->GenerateCube(1.0f, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//make variable that wont be remade every frame and keeps value form prev frame
	static float xInc = 0.0f;

	//renders the cubes
	//1st row
	cubeArray[0]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-3.0f + xInc, 5.0f, 0.0f)));
	cubeArray[1]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(3.0f + xInc, 5.0f, 0.0f)));

	//2nd row
	cubeArray[2]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-2.0f + xInc, 4.0f, 0.0f)));
	cubeArray[3]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(2.0f + xInc, 4.0f, 0.0f)));

	//3rd row
	for (int i = 4; i < 11; i++)
	{
		cubeArray[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-3.0f + (i - 4) + xInc, 3.0f, 0.0f)));
	}

	//4th row
	for (int i = 11; i < 18; i++)
	{
		float pos = -4.0f + (i - 11);
		if (i > 15)
		{
			pos += 2.0f;
		}
		else if (i > 12)
		{
			pos += 1.0f;
		}
		
		cubeArray[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(pos + xInc, 2.0f, 0.0f)));
	}

	//5th row
	for (int i = 18; i < 29; i++)
	{
		cubeArray[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-5.0f + (i - 18) + xInc, 1.0f, 0.0f)));
	}

	//6th row
	cubeArray[29]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-5.0f + xInc, 0.0f, 0.0f)));
	for (int i = 30; i < 37; i++)
	{
		cubeArray[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-3.0f + (i - 30) + xInc, 0.0f, 0.0f)));
	}
	cubeArray[37]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(5.0f + xInc, 0.0f, 0.0f)));

	//7th row
	cubeArray[38]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-5.0f + xInc, -1.0f, 0.0f)));
	cubeArray[39]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(-3.0f + xInc, -1.0f, 0.0f)));
	cubeArray[40]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(3.0f + xInc, -1.0f, 0.0f)));
	cubeArray[41]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(5.0f + xInc, -1.0f, 0.0f)));

	//8th row
	for (int i = 42; i < 46; i++)
	{
		float pos = -2.0f + (i - 42);
		if (i > 43)
		{
			pos += 1.0f;
		}

		cubeArray[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(pos + xInc, -2.0f, 0.0f)));
	}

	//increase the x
	xInc += 0.1f;

	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}