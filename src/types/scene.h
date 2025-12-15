#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "types/entity.h"

const unsigned int MAX_LIGHTS = 5;

class Scene
{
public:
    Scene() = default;
    ~Scene();

    EntityId addEntity(EntityType type, std::string name);
    EntityId addLight();

    std::vector<Entity*> &getEntities() { return entities; }

    Entity *getEntityById(EntityId id);
    Entity *getEntityByName(std::string name);
    std::vector<Entity*> getEntitiesByType(EntityType type);

    std::string getUniqueEntityName(std::string baseName);

private:

    std::vector<Entity*> entities;
    std::unordered_map<std::string, EntityId> nameToEntity;

    uint32_t lightCount = 0;
};