#include "BuildingTile.h"
#include "EntityManager.h"

#include "Sound_Engine.h"
#include "PlayerInfo\PlayerInfo.h"

//BuildingEntity* BuildingTile::leftWall = nullptr;
//BuildingEntity* BuildingTile::rightWall = nullptr;
//BuildingEntity* BuildingTile::bottomWall = nullptr;
//BuildingEntity* BuildingTile::topWall = nullptr;
//BuildingEntity* BuildingTile::floor = nullptr;
//EquipmentEntity* BuildingTile::equipment = nullptr;

BuildingTile::BuildingTile()
	: topWall(nullptr)
	, leftWall(nullptr)
	, rightWall(nullptr)
	, bottomWall(nullptr)
	, floor(nullptr)
	, equipment(nullptr)
{
}

BuildingTile::~BuildingTile()
{
}

void BuildingTile::AddWall(BuildingEntity * entity, TILE_SIDE direction)
{
	if (Player::GetInstance()->GetMaterial() >= 100)
	{
		//where 1 -> left, 2 -> top, 3 -> right, 4 ->bottom
		switch (direction) {
		case LEFT:
			if (leftWall) {
				std::cout << "Position occupied" << std::endl;
				delete entity;
				return;
			}
			leftWall = entity;
			entity->tile = this;
			break;
		case TOP:
			if (topWall) {
				std::cout << "Position occupied" << std::endl;
				delete entity;
				return;
			}
			topWall = entity;
			entity->tile = this;
			break;
		case RIGHT:
			if (rightWall) {
				std::cout << "Position occupied" << std::endl;
				delete entity;
				return;
			}
			rightWall = entity;
			entity->tile = this;
			break;
		case BOTTOM:
			if (bottomWall) {
				std::cout << "Position occupied" << std::endl;
				delete entity;
				return;
			}
			bottomWall = entity;
			entity->tile = this;
			break;
		default:
			return;
		}
		if (entity->type == BuildingEntity::BUILDING_COVER)
			std::cout << "Cover placed down" << std::endl;
		Player::GetInstance()->SetMaterial(Math::Max(0, Player::GetInstance()->GetMaterial() - 100));
		EntityManager::GetInstance()->AddEntity(entity);
		CSoundEngine::GetInstance()->playsinglesound("Build", 0.4f);
		std::cout << "Build sound Played" << std::endl;
	}
	else
	{
		delete entity;
		return;
	}
}

void BuildingTile::AddFloor(BuildingEntity * entity)
{
	if (Player::GetInstance()->GetMaterial() >= 50)
	{
		if (floor) {
			std::cout << "Floor position occupied\n";
			delete entity;
			return;
		}
		if (entity->type != BuildingEntity::BUILDING_FLOOR)
			return;
		floor = entity;
		entity->tile = this;
		Player::GetInstance()->SetMaterial(Math::Max(0, Player::GetInstance()->GetMaterial() - 50));
		EntityManager::GetInstance()->AddEntity(floor);
		CSoundEngine::GetInstance()->playsinglesound("Floor", 0.4f);
	}
	else
	{
		delete entity;
		return;
	}
}

void BuildingTile::AddCore(BuildingEntity * entity)
{
	floor = entity;
	EntityManager::GetInstance()->AddEntity(entity);
}

void BuildingTile::AddEquipment(EquipmentEntity * entity)
{
	if (Player::GetInstance()->GetMaterial() >= 200)
	{
		if (equipment) {
			std::cout << "Equipment slot is occupied\n";
			delete entity;
			return;
		}
		equipment = entity;
		entity->tile = this;
		Player::GetInstance()->SetMaterial(Math::Max(0, Player::GetInstance()->GetMaterial() - 200));
		EntityManager::GetInstance()->AddEntity(equipment);
	}
	else
	{
		delete entity;
		return;
	}
}

bool BuildingTile::GetEmpty()
{
	if (leftWall || rightWall || bottomWall || topWall) {
		if (floor || equipment)
			return false;
	}
	else
		return true;
}
