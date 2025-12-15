#include "types/scene.h"
#include "types/entity.h"
#include "types/light.h"
#include <cstddef>

Scene::~Scene()
{
    for (Entity *entity : entities) {
        if (entity)
            delete entity;
    }
}

EntityId Scene::addEntity(EntityType type, std::string name)
{
    auto it = nameToEntity.find(name);
    if (it == nameToEntity.end()) {
        name = getUniqueEntityName(name);
        EntityId id = (EntityId)entities.size();
        nameToEntity[name] = id;
        entities.push_back(new Entity(type, name));
        return id;
    } else {
        return it->second;
    }

    return EntityId::Invalid;
}

EntityId Scene::addLight()
{
    if (lightCount < MAX_LIGHTS) {
        EntityId id = (EntityId)entities.size();
        std::string name = getUniqueEntityName("Light");
        entities.push_back(new Light(name));
        nameToEntity[name] = id;
        lightCount++;
        return id;
    } else {
        printf("Failed to add light - limit exceeded!\n");
    }

    return EntityId::Invalid;
}

Entity *Scene::getEntityById(EntityId id)
{
    if (id == EntityId::Invalid)
        return nullptr;

    return entities[(size_t)id];
}

Entity *Scene::getEntityByName(std::string name)
{
    if (name.empty())
        return nullptr;

    auto it = nameToEntity.find(name);
    if (it != nameToEntity.end()) {
        return entities[(size_t)it->second];
    }

    return nullptr;
}

std::vector<Entity*> Scene::getEntitiesByType(EntityType type)
{
    std::vector<Entity*> outEntities;
    for (Entity *entity : entities) {
        if (entity->getEntityType() == type) {
            outEntities.push_back(entity);
        }
    }

    return outEntities;
}

std::string Scene::getUniqueEntityName(std::string baseName)
{
    size_t id = 0;
    for (; id <= entities.size(); id++) {
        std::string name = baseName + std::to_string(id);
        if (!getEntityByName(name)) {
            return name;
        }
    }

    return baseName + std::to_string(id);
}