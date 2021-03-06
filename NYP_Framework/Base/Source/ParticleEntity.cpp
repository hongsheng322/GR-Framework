#include "ParticleEntity.h"

#include "RenderHelper.h"
#include "GraphicsManager.h"

ParticleEntity::ParticleEntity()
	:elaspedTime(0)
	, duration(0)
{
	active = false;
	modelMesh = nullptr;
}

ParticleEntity::~ParticleEntity()
{
}

void ParticleEntity::Update(double dt)
{
	if (active) {
		switch (particleType) {
		case BLOOD:
			position += direction * velocity * dt;
			if (velocity > 0)
				velocity -= 2.5 * dt;
			elaspedTime += dt;
			if (elaspedTime >= duration)
				active = false;
			break;
		case EXPLOSION:
			position += direction * velocity * dt;
			if (velocity > 0)
				velocity -= 1 * dt;
			elaspedTime += dt;
			if (elaspedTime >= duration)
				active = false;
			break;
		case FEEDBACK_AMMO:
			position += direction * velocity * dt;
			if (velocity > 0)
				velocity -= 1 * dt;
			elaspedTime += dt;
			if (elaspedTime >= duration)
				active = false;
			break;
		case FEEDBACK_UNLOCK:
			position += direction * velocity * dt;
			if (velocity > 0)
				velocity -= 1 * dt;
			elaspedTime += dt;
			if (elaspedTime >= duration)
				active = false;
			break;
		default:
			return;
		}
	}
}

void ParticleEntity::Render()
{
	if (active) {
		if (modelMesh) {
			MS&ms = GraphicsManager::GetInstance()->GetModelStack();
			if (particleType == BLOOD) {
				ms.PushMatrix();
				ms.Translate(position.x, position.y, position.z);
				ms.Scale(scale.x, scale.y, scale.z);
				RenderHelper::RenderMeshWithLight(modelMesh);
				ms.PopMatrix();
			}
			else {
				ms.PushMatrix();
				ms.Translate(position.x, position.y, position.z);
				ms.Scale(scale.x, scale.y, scale.z);
				RenderHelper::RenderMesh(modelMesh);
				ms.PopMatrix();
			}
		}
	}
}

void ParticleEntity::SetMesh(Mesh * _mesh)
{
	modelMesh = _mesh;
}
