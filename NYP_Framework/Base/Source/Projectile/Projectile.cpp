#include "Projectile.h"

#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../Sound_Engine.h"
#include "../ParticleManager.h"

Projectile::Projectile(std::string _meshName) : GenericEntity(MeshList::GetInstance()->GetMesh(_meshName))
, m_bStatus(false)
, theDirection(0, 0, 0)
, m_fLifetime(-1.0f)
, m_fSpeed(40.0f)
, damage(0)
, size(0.5)
, pent(0)
, weap(nullptr)
{

}

Projectile::~Projectile()
{
}

void Projectile::SetStatus(const bool m_bStatus)
{
	if (m_bStatus == false)
		m_fLifetime = -1;
	this->m_bStatus = m_bStatus;
}

bool Projectile::GetStatus(void) const
{
	return m_bStatus;
}

void Projectile::Set(Vector3 theNewPosition, Vector3 theNewDirection, const float m_fLifetime, const float m_fSpeed, const int damage)
{
	position = theNewPosition;
	theDirection = theNewDirection;
	this->m_fLifetime = m_fLifetime;
	this->m_fSpeed = m_fSpeed;
	this->damage = damage;
}

void Projectile::SetDirection(Vector3 theNewDirection)
{
	theDirection = theNewDirection;
}

Vector3 Projectile::GetDirection(void)
{
	return theDirection;
}

void Projectile::SetLifetime(const float m_fLifetime)
{
	this->m_fLifetime = m_fLifetime;
}

float Projectile::GetLifetime(void) const
{
	return m_fLifetime;
}

void Projectile::SetSpeed(const float m_fSpeed)
{
	this->m_fSpeed = m_fSpeed;
}

float Projectile::GetSpeed(void) const
{
	return m_fSpeed;
}

void Projectile::SetDamage(const int m_damage)
{
	this->damage = m_damage;
}

float Projectile::GetDamage(void) const
{
	return damage;
}

void Projectile::Update(double dt)
{
	if (m_bStatus == false)
		return;

	// Update TimeLife of projectile. Set to inactive if too long
	m_fLifetime -= (float)dt;
	if (m_fLifetime < 0.0f)
	{
		SetStatus(false);
		SetIsDone(true);	// This method is to inform the EntityManager that it should remove this instance
		return;
	}

	SetAABB(Vector3(position.x + size / 2, position.y + size / 2, position.z + size / 2),
		Vector3(position.x - size / 2, position.y + size / 2, position.z - size / 2));

	// Update Position
	Vector3 d(theDirection.x, 0, -theDirection.y);
	position += d * (float)dt * m_fSpeed;
}

void Projectile::Render(void)
{
	if (m_bStatus == false)
		return;

	if (m_fLifetime < 0.0f)
		return;

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(size, size, size);
	RenderHelper::RenderMesh(modelMesh);
	modelStack.PopMatrix();
}

Projectile * Create::Bullet(std::string _meshName)
{
	Projectile* p = new Projectile(_meshName);
	p->objectType = GenericEntity::PROJECTILE;
	EntityManager::GetInstance()->AddEntity(p);
	return p;
}

void Projectile::CollisionResponse(GenericEntity * thatEntity)
{
	EnemyEntity* enemy = dynamic_cast<EnemyEntity*>(thatEntity);

	if (weap != nullptr) {
		if (weap->GetWeaponID() == 5) {
			if (thatEntity->objectType == ENEMY) {
				ParticleManager::GetInstance()->GenerateExplosion(position);
				this->SetIsDone(true);
				for (auto it : EntityManager::GetInstance()->GetEntityList()) {
					if (it->objectType == ENEMY) {
						enemy = dynamic_cast<EnemyEntity*>(it);
						if (enemy->GetActive()) {
							if ((it->GetPosition() - position).LengthSquared() < 400) {
								ParticleManager::GetInstance()->GenerateBlood(enemy->GetPosition());
								enemy->SetHealth(enemy->GetHealth() - damage);
								std::cout << "hito\n";
								if (enemy->GetHealth() <= 0)
								{
									enemy->SetActive(false);
									enemy->Reset();
									Player::GetInstance()->SetMaterial(Player::GetInstance()->GetMaterial() + 200);
								}
							}
						}
					}
				}
			}
		}
		else
			goto here;
	}
	else {
		here:
		switch (thatEntity->objectType) {
		case ENEMY:
			if (source != ENEMY_SOURCE) {
				this->SetIsDone(true);
				ParticleManager::GetInstance()->GenerateBlood(enemy->GetPosition());
				if (enemy->GetActive()) {
					enemy->SetHealth(enemy->GetHealth() - damage);
					std::cout << "hito\n";
					if (enemy->GetHealth() <= 0)
					{
						enemy->SetActive(false);
						enemy->Reset();
						Player::GetInstance()->SetMaterial(Player::GetInstance()->GetMaterial() + 200);
					}
				}
			}
			break;
		case BUILDING: {
			BuildingEntity* b = dynamic_cast<BuildingEntity*>(thatEntity);
			if (pent) {
				break;
			}
			if (b->type != BuildingEntity::BUILDING_COVER) {
				if (source == ENEMY_SOURCE) {
					b->SetHealth(b->GetHealth() - damage);
					if (b->GetHealth() <= 0) {
						b->SetIsDone(true);
					}
				}
				this->SetIsDone(true);
			}
		}
					   break;
		case EQUIPMENT:
			if (source == PLAYER_SOURCE) {
				break;
			}

			this->SetIsDone(true);
		default:
			return;
		}
	}
}
