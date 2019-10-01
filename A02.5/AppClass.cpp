#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Mandy Wong - mxw4175@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float radius = fSize;

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	//calculate the vertices and store them in a vertex
	for (int i = 0; i < m_uOrbits; i++)
	{//for circle number
		//temp vector to push into 2d vector
		std::vector<vector3> temp;

		//track no. of vertices circle
		int vertexNum = uSides + i;

		float angle = (360.0f / vertexNum) * (PI / 180);

		for (int j = 0; j < vertexNum; j++)
		{//for no. of vertices on the circle
			temp.push_back(vector3(radius * cos(angle * j), radius * sin(angle * j), 0));
		}

		//increment radius
		radius += 0.5f;

		//adds temp to vector then resets it
		circleVertices.push_back(temp);
		temp.clear();

		//initialise the path vector
		paths.push_back(0);
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//calculate the current position
	vector3 v3CurrentPos;

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{//per orbit
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//get start and end point
		vector3 startPoint = circleVertices[i][paths[i]];
		vector3 endPoint;
		if (paths[i] == (circleVertices[i].size() - 1))
		{
			endPoint = circleVertices[i][0];
		}
		else
		{
			endPoint = circleVertices[i][paths[i] + 1];
		}

		//get percentage
		float timeNeeded = 2.0f;
		float percentage = fTimer / timeNeeded;

		//use lerp to find position
		v3CurrentPos = glm::lerp(startPoint, endPoint, percentage);

		//check if reached end point
		if (fTimer >= timeNeeded)
		{
			//increment all paths
			for (int j = 0; j < m_uOrbits; j++)
			{
				paths[j] += 1;

				//reset path when reached the end
				if (paths[j] > (circleVertices[j].size() - 1))
				{
					paths[j] = 0;
				}
			}

			//reset timer
			fTimer = 0;
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);
		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	////track vertices
	//uint uSides = 3;
	////int vertexNum = uSides + i;

	////for (int i = 0; i < uSides; i++)
	//{
	//	//get start and end point
	//	static uint path = 0;
	//	vector3 startPoint = circleVertices[0][path];
	//	vector3 endPoint;
	//	if (path == (circleVertices[0].size() - 1))
	//	{
	//		endPoint = circleVertices[0][0];
	//	}
	//	else
	//	{
	//		endPoint = circleVertices[0][path + 1];
	//	}

	//	//get percentage
	//	float timeNeeded = 2.0f;
	//	float percentage = fTimer / timeNeeded;

	//	//use lerp to find position
	//	v3CurrentPos = glm::lerp(startPoint, endPoint, percentage);

	//	//check if reached end point
	//	if (fTimer >= timeNeeded)
	//	{
	//		//increment path
	//		path += 1;

	//		//reset path when reached the end
	//		if (path > (circleVertices[0].size() - 1))
	//		{
	//			path = 0;
	//		}

	//		//reset timer
	//		//fTimer = 0;
	//	}

	//}
	//	
	//matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);
	////draw spheres
	//m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);

	////for (int i = 0; i < uSides + 1; i++)
	//{
	//	//get start and end point
	//	static uint path = 0;
	//	vector3 startPoint = circleVertices[1][path];
	//	vector3 endPoint;
	//	if (path == (circleVertices[1].size() - 1))
	//	{
	//		endPoint = circleVertices[1][0];
	//	}
	//	else
	//	{
	//		endPoint = circleVertices[1][path + 1];
	//	}

	//	//get percentage
	//	float timeNeeded = 2.0f;
	//	float percentage = fTimer / timeNeeded;

	//	//use lerp to find position
	//	v3CurrentPos = glm::lerp(startPoint, endPoint, percentage);

	//	//check if reached end point
	//	if (fTimer >= timeNeeded)
	//	{
	//		//increment path
	//		path += 1;

	//		//reset path when reached the end
	//		if (path > (circleVertices[1].size() - 1))
	//		{
	//			path = 0;
	//		}

	//		//reset timer
	//		//fTimer = 0;
	//	}

	//}

	//m4Model = glm::translate(m4Offset, v3CurrentPos);
	////draw spheres
	//m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);

	//{
	//	//get start and end point
	//	static uint path = 0;
	//	vector3 startPoint = circleVertices[2][path];
	//	vector3 endPoint;
	//	if (path == (circleVertices[2].size() - 1))
	//	{
	//		endPoint = circleVertices[2][0];
	//	}
	//	else
	//	{
	//		endPoint = circleVertices[2][path + 1];
	//	}

	//	//get percentage
	//	float timeNeeded = 2.0f;
	//	float percentage = fTimer / timeNeeded;

	//	//use lerp to find position
	//	v3CurrentPos = glm::lerp(startPoint, endPoint, percentage);

	//	//check if reached end point
	//	if (fTimer >= timeNeeded)
	//	{
	//		//increment path
	//		path += 1;

	//		//reset path when reached the end
	//		if (path > (circleVertices[2].size() - 1))
	//		{
	//			path = 0;
	//		}

	//		//reset timer
	//		fTimer = 0;
	//	}

	//}

	//m4Model = glm::translate(m4Offset, v3CurrentPos);
	////draw spheres
	//m_pMeshMngr->AddSphereToRenderList(m4Model* glm::scale(vector3(0.1)), C_WHITE);
	/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

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
	//release GUI
	ShutdownGUI();
}