#pragma once
#ifndef BUILDING_ENTITY_H
#define BUILDING_ENTITY_H

#include "GenericEntity.h"

class Mesh;

class BuildingEntity : public GenericEntity {
public:
	BuildingEntity(const std::string _meshName);
	virtual ~BuildingEntity();

	enum BUILDING_TYPE {
		BUILDING_WALL,
		BUILDING_DOOR,
		BUILDING_COVER,
		BUILDING_FLOOR,
		BUILDING_CORE
	}type;

	void Render();

	int GetHealth();
	void SetHealth(int _value);

	int GetLevel();
	void SetLevel(int _value);

	float GetRotation();
	void SetRotation(float _value);

	int GetGridX();
	int GetGridZ();
	void SetGrid(int _x, int _z);

private:
	int health;
	int level;
	float rotation;
	int gridX, gridZ;
};

#endif