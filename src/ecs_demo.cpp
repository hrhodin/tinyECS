#include "tinyECS/tiny_ecs.hpp"
#include <string>
#include <iostream>

///////////////////////////
// OOP inheritance pattern
struct AnimalOOP {
	virtual bool canDive() = 0;
	virtual bool canWalk() = 0;
	virtual std::string name() = 0;
};

struct WaterAnimalOOP : public AnimalOOP {
	float swim_speed = 3;
	bool canDive() { return true; };
	bool canWalk() { return false; };
};

struct LandAnimalOOP : public AnimalOOP {
	float walk_speed = 3;
	bool canDive() { return false; };
	bool canWalk() { return true; };
};

struct FishOOP : public WaterAnimalOOP
{
	virtual std::string name() {return "Fish";};
};

struct HorseOOP : public LandAnimalOOP
{
	virtual std::string name() { return "Horse"; };
};

struct TurtleOOP : public LandAnimalOOP, WaterAnimalOOP
{
	virtual std::string name() {return "Turtle";};
};

/////////////////////////////////////////
// Entity Component System (ECS) pattern
struct Animal {
	std::string name;
	Animal(const char* str) : name(str) {};
};

struct WaterAnimal {
	float swim_speed = 3;
};

struct LandAnimal {
	float walk_speed = 3;
};

/////////////////////////////////////////
// Entry point
int main(int argc, char* argv[])
{
	/////////////////////////
	// OOP pattern
	// Create a fish and horse
	FishOOP fish_oop;
	HorseOOP horse_oop;

	// Create a turtle
	TurtleOOP turtle_oop;

	// Group all animals (enabled by inheriting the common base class Animal)
	std::vector<AnimalOOP*> animals_oop;
	animals_oop.push_back(&fish_oop);
	animals_oop.push_back(&horse_oop);
	//animals_oop.push_back(&turtle_oop); // ERROR: the base class is ambigious, see mroe here: https://stackoverflow.com/questions/44878627/inheritance-causes-ambiguous-conversion
	animals_oop.push_back(static_cast<LandAnimalOOP*>(&turtle_oop)); // WARNING: this compiles, but now the turtle is not able to swim!
	
	// Print the names and abilities of all animals
	std::cout << "----- OOP inheritance debug output -----\n";
	for (AnimalOOP* animalPtr : animals_oop) {
        assert(animalPtr);
        auto& theAnimal = *animalPtr; // NOTE: reference is needed for virtual function calls
        std::cout
            << theAnimal.name() << ' '
            << (theAnimal.canDive() ? "can" : "can't") << " swim and "
            << (theAnimal.canWalk() ? "can" : "can't") << " walk" << std::endl;
		// This is not what we want, our OOP turtle can't swim :/
    }

	//////////////////////////
	// ECS pattern
	// Create a fish
	ECS::Entity fish;
	ECS::registry<Animal>.insert(fish, Animal("Fish"));
	ECS::registry<WaterAnimal>.insert(fish, WaterAnimal());

	// Create a horse
	ECS::Entity horse;
	ECS::registry<Animal>.emplace(horse, "Horse"); // Note, emplace() does the same as insert() but is shorter
	ECS::registry<LandAnimal>.emplace(horse);

	// Create a turtle
	ECS::Entity turtle;
	ECS::registry<Animal>.emplace(turtle, "Turtle");
	ECS::registry<LandAnimal>.emplace(turtle);
	ECS::registry<WaterAnimal>.emplace(turtle);

	// WARNING: Common mistake! The following code will not change the animal's name, because we copy animal before updating it
	// One has to work with references or pointers instead. Correct it to create a Big Fish.
	Animal animal = ECS::registry<Animal>.get(fish);
	animal.name = "Big " + animal.name;

	// Note, no need to group animals, its automatic in ECS!
	// Note, no need to define fish, horse, and turtle classed, they are formed by the equipped components!

	// Print the names and abilities of all animals
	std::cout << "----- ECS debug output -----\n";
	for (ECS::Entity& animal : ECS::registry<Animal>.entities) {
        std::cout
            << ECS::registry<Animal>.get(animal).name << ' '
            << (ECS::registry<WaterAnimal>.has(animal) ? "can" : "can't") << " swim and "
            << (ECS::registry<LandAnimal>.has(animal) ? "can" : "can't") << " walk" << std::endl;
    }

	// Inspect the ECS state
	ECS::ContainerInterface::list_all_components();
	ECS::ContainerInterface::list_all_components_of(turtle);

	// Clearing the ECS system before exit
	ECS::ContainerInterface::clear_all_components();
	return EXIT_SUCCESS;
}

