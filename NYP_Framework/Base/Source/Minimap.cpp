#include "Minimap.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "GL\glew.h"

CMinimap::CMinimap(void)
	: m_cMinimap_Background(NULL)
	, m_cMinimap_Avatar(NULL)
	, m_fAngle(-90.0f)
{
	Init();
}

CMinimap::~CMinimap(void)
{
	if (m_cMinimap_Background)
	{
		delete m_cMinimap_Background;
		m_cMinimap_Background = NULL;
	}
	if (m_cMinimap_Avatar)
	{
		delete m_cMinimap_Avatar;
		m_cMinimap_Avatar = NULL;
	}
}

// Initialise this class instance
bool CMinimap::Init(void)
{
	m_fAngle = -90.0f;
	position.Set(335.f, 235.f, 0.0f);
	scale.Set(100.0f, 100.0f, 100.0f);

	return true;
}

// Set the background mesh to this class
bool CMinimap::SetBackground(Mesh* aBackground)
{
	if (aBackground != NULL)
	{
		m_cMinimap_Background = aBackground;
		return true;
	}
	return false;
}

// Get the background mesh to this class
Mesh* CMinimap::GetBackground(void) const
{
	return m_cMinimap_Background;
}

// Set the Avatar mesh to this class
bool CMinimap::SetAvatar(Mesh* anAvatar)
{
	if (anAvatar != NULL)
	{
		m_cMinimap_Avatar = anAvatar;
		return true;
	}
	return false;
}

// Get the Avatar mesh to this class
Mesh* CMinimap::GetAvatar(void) const
{
	return m_cMinimap_Avatar;
}

// Set m_iAngle of avatar
bool CMinimap::SetAngle(const float m_fAngle)
{
	this->m_fAngle = m_fAngle;
	return true;
}

// Get m_iAngle
float CMinimap::GetAngle(void) const
{
	return m_fAngle;
}

// Set position of avatar in minimap
bool CMinimap::SetPosition(const float m_fPos_X, const float m_fPos_y)
{
	this->m_fPos_x = m_fPos_X;
	this->m_fPos_y = m_fPos_y;
	return true;
}

// Get position x of avatar in minimap
float CMinimap::GetPosition_x(void) const
{
	return m_fPos_x;
}

// Get position y of avatar in minimap
float CMinimap::GetPosition_y(void) const
{
	return m_fPos_y;
}

// Set size of minimap (for calculation of avatar in minimap)
bool CMinimap::SetSize(const float m_fSize_x, const float m_fSize_y)
{
	this->m_fSize_x = m_fSize_x;
	this->m_fSize_y = m_fSize_y;
	return true;
}

// Get size of minimap (for calculation of avatar in minimap)
float CMinimap::GetSize_x(void) const
{
	return m_fSize_x;
}

// Get size of minimap (for calculation of avatar in minimap)
float CMinimap::GetSize_y(void) const
{
	return m_fSize_y;
}

void CMinimap::RenderUI()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	// Push the current transformation into the modelStack
	modelStack.PushMatrix();
		// Translate the current transformation
		modelStack.Translate(position.x, position.y, position.z);
		// Scale the current transformation
		modelStack.Scale(scale.x, scale.y, scale.z);

		if (!isResizing)
		{
			SetSize(1.3f, 1.f);
			modelStack.PushMatrix();
			if (m_cMinimap_Background)
				RenderHelper::RenderMesh(m_cMinimap_Background);
			modelStack.PopMatrix();

			// Disable depth test
			glDisable(GL_DEPTH_TEST);

			// Display the Avatar
			modelStack.PushMatrix();
			modelStack.Translate(m_fPos_x - 0.4f, m_fPos_y - 0.6f, 0.f);
			modelStack.Rotate(m_fAngle, 0.0f, 0.0f, -1.0f);
			if (m_cMinimap_Avatar)
				RenderHelper::RenderMesh(m_cMinimap_Avatar);
			modelStack.PopMatrix();

			// Enable depth test
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			SetSize(3.6f, 3.f);
			modelStack.PushMatrix();
			modelStack.Translate(-3.35f, -2.35f, 0.f);
			modelStack.Scale(2.5f, 2.5f, 1.f);
			if (m_cMinimap_Background)
				RenderHelper::RenderMesh(m_cMinimap_Background);
			modelStack.PopMatrix();

			// Disable depth test
			glDisable(GL_DEPTH_TEST);

			// Display the Avatar
			modelStack.PushMatrix();
			modelStack.Translate(-3.3f, -2.35f, 0.f);
			modelStack.Translate(m_fPos_x - 1.2f, m_fPos_y - 1.8f, 0.f);
			modelStack.Rotate(m_fAngle, 0.0f, 0.0f, -1.0f);
			modelStack.Scale(3.f, 3.f, 1.f);
			if (m_cMinimap_Avatar)
				RenderHelper::RenderMesh(m_cMinimap_Avatar);
			modelStack.PopMatrix();

			// Enable depth test
			glEnable(GL_DEPTH_TEST);
		}
	modelStack.PopMatrix();
}

CMinimap* Create::Minimap()
{
	CMinimap* result = CMinimap::GetInstance();
	return result;
}