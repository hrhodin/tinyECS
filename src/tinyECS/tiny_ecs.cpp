// internal
#include "tiny_ecs.hpp"

#include <cassert>
#include <iostream>
#include <typeinfo>

// We store a list of all Component containers to be able to inspect the number of components and entities in each and to remove entities across containers
using namespace ECS;

std::vector<ContainerInterface*>& ContainerInterface::registry_list_singleton() {
	// This is a Meyer's singleton, i.e., a function returning a static local variable by reference to solve SIOF
	static std::vector<ContainerInterface*> singleton; // constructed during first call
	return singleton;
}

void ContainerInterface::clear_all_components() {
	for (auto reg : registry_list_singleton()) {
		reg->clear();
    }
}
void ContainerInterface::list_all_components() {
	std::cout << "Debug info on all registry entries:\n";
	const auto& singleton = registry_list_singleton();
	for (auto reg : singleton) {
        assert(reg); // Must not be null
		if (reg->size() > 0) {
			std::cout
                << "  " << reg->size() << " components of type "
                << typeid(*reg).name() << "\n    ";
			for (auto entity : reg->entities) {
				std::cout << entity.id << ", ";
            }
			std::cout << '\n';
		}
	}
    std::cout.flush();
}
void ContainerInterface::list_all_components_of(Entity e) {
	std::cout << "Debug info on components of entity " << e.id << ":\n";
	for (auto reg : registry_list_singleton()) {
        assert(reg); // Must not be null
		if (reg->has(e)) {
			std::cout
                << "  type " << typeid(*reg).name() << ", stored at location "
                << reg->map_entity_component_index[e.id] << '\n';
        }
    }
}
void ContainerInterface::remove_all_components_of(Entity e) {
	for (auto reg : registry_list_singleton()) {
        assert(reg); // Must not be null
		reg->remove(e);
    }
}
