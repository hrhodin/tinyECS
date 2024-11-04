#pragma once

#include <vector>
#include <unordered_map>
#include <assert.h>

// Unique identifier for all entities
class Entity
{
    unsigned int id;
    static unsigned int id_count; // starts from 1, entity 0 is the default initialization
public:
    Entity()
    {
        id = id_count++;
        // Note, indices of already deleted entities aren't re-used in this simple implementation.
    }
    operator unsigned int() { return id; } // this enables automatic casting to int
};

// Common interface to refer to all containers in the ECS registry
struct ContainerInterface
{
    virtual void clear() = 0;
    virtual size_t size() = 0;
    virtual void remove(Entity e) = 0;
    virtual bool has(Entity entity) = 0;
};

// A container that stores components of type 'Component' and associated entities
template <typename Component> // A component can be any class
class ComponentContainer : public ContainerInterface
{
private:
    // The hash map from Entity -> array index.
    std::unordered_map<unsigned int, unsigned int> map_entity_componentID; // the entity is cast to uint to be hashable.
    bool registered = false;
public:
    // Container of all components of type 'Component'
    std::vector<Component> components;

    // The corresponding entities
    std::vector<Entity> entities;

    // Constructor that registers the type
    ComponentContainer()
    {
    }

    // Inserting a component c associated to entity e
    inline Component& insert(Entity e, Component c, bool check_for_duplicates = true)
    {
        // Usually, every entity should only have one instance of each component type
        assert(!(check_for_duplicates && has(e)) && "Entity already contained in ECS registry");


        map_entity_componentID[e] = (unsigned int)components.size();
        components.push_back(std::move(c)); // the move enforces move instead of copy constructor
        entities.push_back(e);
        return components.back();
    };

    // The emplace function takes the the provided arguments Args, creates a new object of type Component, and inserts it into the ECS system
    template<typename... Args>
    Component& emplace(Entity e, Args &&... args) {
        return insert(e, Component(std::forward<Args>(args)...)); // the forward ensures that arguments are moved not copied
    };
    template<typename... Args>
    Component& emplace_with_duplicates(Entity e, Args &&... args) {
        return insert(e, Component(std::forward<Args>(args)...), false); // the forward ensures that arguments are moved not copied
    };

    // A wrapper to return the component of an entity
    Component& get(Entity e) {
        assert(has(e) && "Entity not contained in ECS registry");
        return components[map_entity_componentID[e]];
    }

    // Check if entity has a component of type 'Component'
    bool has(Entity entity) {
        return map_entity_componentID.count(entity) > 0;
    }

    // Remove an component and pack the container to re-use the empty space
    void remove(Entity e)
    {
        if (has(e))
        {
            // Get the current position
            int cID = map_entity_componentID[e];

            // Move the last element to position cID using the move operator
            // Note, components[cID] = components.back() would trigger the copy instead of move operator
            components[cID] = std::move(components.back());
            entities[cID] = entities.back(); // the entity is only a single index, copy it.
            map_entity_componentID[entities.back()] = cID;

            // Erase the old component and free its memory
            map_entity_componentID.erase(e);
            components.pop_back();
            entities.pop_back();
            // Note, one could mark the id for re-use
        }
    };

    // Remove all components of type 'Component'
    void clear()
    {
        map_entity_componentID.clear();
        components.clear();
        entities.clear();
    }

    // Report the number of components of type 'Component'
    size_t size()
    {
        return components.size();
    }
};
