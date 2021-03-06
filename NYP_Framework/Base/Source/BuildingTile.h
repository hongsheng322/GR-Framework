#pragma once
#ifndef BUILDING_TILE_H
#define BUILDING_TILE_H

#include <vector>

#include "BuildingEntity.h"
#include "EquipmentEntity.h"
#include "Collider\Collider.h"

class BuildingTile {
public:
	BuildingTile();
	virtual ~BuildingTile();

	enum TILE_SIDE {
		NONE = 0,
		LEFT,
		TOP, 
		RIGHT,
		BOTTOM
	}side;

	void AddWall(BuildingEntity* entity, TILE_SIDE direction);
	void AddFloor(BuildingEntity* entity);
	void AddCore(BuildingEntity* entity);
	
	void AddEquipment(EquipmentEntity* entity);

	bool GetEmpty();

	BuildingEntity* leftWall;
	BuildingEntity* rightWall;
	BuildingEntity* bottomWall;
	BuildingEntity* topWall;
	BuildingEntity* floor;
	EquipmentEntity* equipment;

	Collision hitbox;

};

#endif