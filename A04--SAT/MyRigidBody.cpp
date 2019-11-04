#include "MyRigidBody.h"
#include <vector>
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	//bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	bool bColliding = true;

	//plane of separation info
	glm::vec3 pos = (a_pOther->GetCenterGlobal() - GetCenterGlobal()) / 2;
	glm::vec3 color;
	glm::vec3 orientation;
	
	//if they are colliding check the SAT
	//if (bColliding)
	//{
	int result = SAT(a_pOther);
	if (result != eSATResults::SAT_NONE)
	{
		bColliding = false;// reset to false

		//find which axis checked to have no collision by result % 3
		if (result % 3 == 0)	//z axis
		{
			color = C_BLUE;
			orientation = vector3(0, 0, 1);
		}
		else if (result % 3 == 1)	//x axis
		{
			color = C_RED;
			orientation = vector3(1, 0, 0);
		}
		else if(result % 3 == 2)	//y axis
		{
			color = C_GREEN;
			orientation = vector3(0, 1, 0);
		}
	}
	//}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);

		//render a plane
		m_pMeshMngr->AddPlaneToRenderList(glm::translate(m_m4ToWorld, pos) * glm::scale(glm::vec3(5, 5, 5)) * glm::rotate((float)PI / 2, orientation), color);
		m_pMeshMngr->AddPlaneToRenderList(glm::translate(m_m4ToWorld, pos) * glm::scale(glm::vec3(5, 5, 5)) * glm::rotate(3 * (float)PI / 2, orientation), color);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	Your code goes here instead of this comment;

	For this method, if there is an axis that separates the two objects
	then the return will be different than 0; 1 for any separating axis
	is ok if you are not going for the extra credit, if you could not
	find a separating axis you need to return 0, there is an enum in
	Simplex that might help you [eSATResults] feel free to use it.
	(eSATResults::SAT_NONE has a value of 0)
	*/

	std::vector<glm::vec3> aAxisGlobal;
	aAxisGlobal.push_back(glm::vec3(m_m4ToWorld * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));	//x axis
	aAxisGlobal.push_back(glm::vec3(m_m4ToWorld * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));	//y axis
	aAxisGlobal.push_back(glm::vec3(m_m4ToWorld * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));	//z axis

	std::vector<glm::vec3> bAxisGlobal;
	bAxisGlobal.push_back(glm::vec3(a_pOther->m_m4ToWorld * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));	//x axis
	bAxisGlobal.push_back(glm::vec3(a_pOther->m_m4ToWorld * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));	//y axis
	bAxisGlobal.push_back(glm::vec3(a_pOther->m_m4ToWorld * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));	//z axis


	float halfA;
	float halfB;
	matrix3 m3Rotation;
	matrix3 m3AbsRot;

	//compute rotational matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m3Rotation[i][j] = glm::dot(aAxisGlobal[i], bAxisGlobal[j]);
		}
	}

	//compute translation vector
	vector3 v3Translation = a_pOther->GetCenterGlobal() - GetCenterGlobal();
	//put into a's coordinate frame
	v3Translation = vector3(glm::dot(v3Translation, aAxisGlobal[0]), glm::dot(v3Translation, aAxisGlobal[1]), glm::dot(v3Translation, aAxisGlobal[2]));

	// Compute common subexpressions. Add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is (near) null
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m3AbsRot[i][j] = glm::abs(m3Rotation[i][j]) + FLT_EPSILON;
		}
	}

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		halfA = GetHalfWidth()[i];
		halfB = (a_pOther->GetHalfWidth()[0] * m3AbsRot[i][0])
			+ (a_pOther->GetHalfWidth()[1] * m3AbsRot[i][1])
			+ (a_pOther->GetHalfWidth()[2] * m3AbsRot[i][2]);

		if (glm::abs(v3Translation[i]) > halfA + halfB)
		{
			return i + 1;
		}
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		halfA = (GetHalfWidth()[0] * m3AbsRot[0][i])
			+ (GetHalfWidth()[1] * m3AbsRot[1][i])
			+ (GetHalfWidth()[2] * m3AbsRot[2][i]);
		halfB = a_pOther->GetHalfWidth()[i];

		if (glm::abs((v3Translation[0] * m3Rotation[0][i]) + (v3Translation[1] * m3Rotation[1][i]) + (v3Translation[2] * m3Rotation[2][i])) > halfA + halfB)
		{
			return i + 1;
		}
	}

	// Test axis L = A0 x B0
	halfA = GetHalfWidth()[1] * m3AbsRot[2][0] + GetHalfWidth()[2] * m3AbsRot[1][0];
	halfB = a_pOther->GetHalfWidth()[1] * m3Rotation[0][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[0][1];

	if (glm::abs(v3Translation[2] * m3Rotation[1][0] - v3Translation[1] * m3Rotation[2][0]) > halfA + halfB)
	{
		return eSATResults::SAT_AXxBX;
	}

	// Test axis L = A0 x B1
	halfA = GetHalfWidth()[1] * m3AbsRot[2][1] + GetHalfWidth()[2] * m3AbsRot[1][1];
	halfB = a_pOther->GetHalfWidth()[1] * m3Rotation[0][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[0][0];

	if (glm::abs(v3Translation[2] * m3Rotation[1][1] - v3Translation[1] * m3Rotation[2][1]) > halfA + halfB)
	{
		return eSATResults::SAT_AXxBY;
	}

	// Test axis L = A0 x B2
	halfA = GetHalfWidth()[1] * m3AbsRot[2][2] + GetHalfWidth()[2] * m3AbsRot[1][2];
	halfB = a_pOther->GetHalfWidth()[0] * m3Rotation[0][1] + a_pOther->GetHalfWidth()[1] * m3AbsRot[0][0];

	if (glm::abs(v3Translation[2] * m3Rotation[1][2] - v3Translation[1] * m3Rotation[2][2]) > halfA + halfB)
	{
		return eSATResults::SAT_AXxBZ;
	}

	// Test axis L = A1 x B0
	halfA = GetHalfWidth()[0] * m3AbsRot[2][0] + GetHalfWidth()[2] * m3AbsRot[0][0];
	halfB = a_pOther->GetHalfWidth()[1] * m3Rotation[1][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[1][1];

	if (glm::abs(v3Translation[0] * m3Rotation[2][0] - v3Translation[2] * m3Rotation[0][0]) > halfA + halfB)
	{
		return eSATResults::SAT_AYxBX;
	}

	// Test axis L = A1 x B1
	halfA = GetHalfWidth()[0] * m3AbsRot[2][1] + GetHalfWidth()[2] * m3AbsRot[0][1];
	halfB = a_pOther->GetHalfWidth()[0] * m3Rotation[1][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[1][0];

	if (glm::abs(v3Translation[0] * m3Rotation[2][1] - v3Translation[2] * m3Rotation[0][1]) > halfA + halfB)
	{
		return eSATResults::SAT_AYxBY;
	}

	// Test axis L = A1 x B2
	halfA = GetHalfWidth()[0] * m3AbsRot[2][2] + GetHalfWidth()[2] * m3AbsRot[0][2];
	halfB = a_pOther->GetHalfWidth()[0] * m3Rotation[1][1] + a_pOther->GetHalfWidth()[1] * m3AbsRot[1][0];

	if (glm::abs(v3Translation[0] * m3Rotation[2][2] - v3Translation[2] * m3Rotation[0][2]) > halfA + halfB)
	{
		return eSATResults::SAT_AYxBZ;
	}

	// Test axis L = A2 x B0
	halfA = GetHalfWidth()[0] * m3AbsRot[1][0] + GetHalfWidth()[1] * m3AbsRot[0][0];
	halfB = a_pOther->GetHalfWidth()[1] * m3Rotation[2][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[2][1];

	if (glm::abs(v3Translation[1] * m3Rotation[0][0] - v3Translation[0] * m3Rotation[1][0]) > halfA + halfB)
	{
		return eSATResults::SAT_AZxBX;
	}

	// Test axis L = A2 x B1
	halfA = GetHalfWidth()[0] * m3AbsRot[1][1] + GetHalfWidth()[1] * m3AbsRot[0][1];
	halfB = a_pOther->GetHalfWidth()[0] * m3Rotation[2][2] + a_pOther->GetHalfWidth()[2] * m3AbsRot[2][0];

	if (glm::abs(v3Translation[1] * m3Rotation[0][1] - v3Translation[0] * m3Rotation[1][1]) > halfA + halfB)
	{
		return eSATResults::SAT_AZxBY;
	}

	// Test axis L = A2 x B2
	halfA = GetHalfWidth()[0] * m3AbsRot[1][2] + GetHalfWidth()[1] * m3AbsRot[0][2];
	halfB = a_pOther->GetHalfWidth()[0] * m3Rotation[2][1] + a_pOther->GetHalfWidth()[1] * m3AbsRot[2][0];

	if (glm::abs(v3Translation[1] * m3Rotation[0][2] - v3Translation[0] * m3Rotation[1][2]) > halfA + halfB)
	{
		return eSATResults::SAT_AZxBZ;
	}

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}