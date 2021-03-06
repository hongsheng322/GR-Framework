#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "SingletonTemplate.h"
#include <list>
#include "Vector3.h"

class EntityBase;

class EntityManager : public Singleton<EntityManager>
{
	friend Singleton<EntityManager>;
public:
	void Update(double _dt);
	void Render();
	void RenderUI();

	void AddEntity(EntityBase* _newEntity);
	bool RemoveEntity(EntityBase* _existingEntity);

	void ClearEntities();

	std::list<EntityBase*>GetEntityList();

private:
	EntityManager();
	virtual ~EntityManager();

	std::list<EntityBase*> entityList;
};

#endif // ENTITY_MANAGER_H