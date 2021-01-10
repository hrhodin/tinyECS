# tinyECS

The `tinyECS` library provides a minimal entity component system (ECS) implementation in modern C++14. It is designed to quickstart ECS game development. Its syntax and functioning are similar to [EnTT](https://github.com/skypjack/entt), which eases a transition once the basic ECS features are mastered and there is an appetite for improved performance at the expense of underlying complexity.

`tinyECS` uses a template class to associate components of an arbitrary type with an entity without having to duplicate code or forcing the user to specify in advance what components will be used. The central part in our `tinyECS`  implementation is the `registry` defined in `tiny_ecs.h`. It is merely a `std::map` that maps entities to components stored in tightly packed containers. Additional steps are taken to increase efficiency: a linear cache-friendly memory layout, constant-time component lookup with a hashmap, and move operations to avoid unnecessary copies.

The following code (that can be run by compiling `ecs_demo.cpp`, see below) gives an example use case and relates it to a game design using OOP-inheritance, showing that ECS nicely avoids the diamond problem of multiple inheritances.

### The tinyECS example
This example creates fish, horse, and turtle entities and specifies their swim and walk abilities with components.
```cpp
// Components
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

// Entry point
int main(int argc, char* argv[])
{
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

	// Print the names and abilities of all animals
	printf("----- ECS debug output -----\n");
	for (ECS::Entity& animal : ECS::registry<Animal>.entities)
		printf("%s: can swim %d and can walk %d\n",
			ECS::registry<Animal>.get(animal).name.c_str(),
			ECS::registry<WaterAnimal>.has(animal),
			ECS::registry<LandAnimal>.has(animal));

	// Inspect the ECS state
	ECS::list_all_components();
	ECS::list_all_components_of(turtle);

	// Clearing the ECS system before exit
	ECS::clear_all_components();
	return EXIT_SUCCESS;
}
```
The corresponding output is
```
Fish: can swim 1 and can walk 0
Horse: can swim 0 and can walk 1
Turtle: can swim 1 and can walk 1
Debug info on all regestry entries:
   3 components of type class ECS::ComponentContainer<struct Animal>
   2 components of type class ECS::ComponentContainer<struct WaterAnimal>
   2 components of type class ECS::ComponentContainer<struct LandAnimal>
Debug info on components of entity 3:
type class ECS::ComponentContainer<struct Animal>
type class ECS::ComponentContainer<struct WaterAnimal>
type class ECS::ComponentContainer<struct LandAnimal>
```

### OOP inheritance counter example
The turtle is both a land and water animal, which requires multiple-inheritance in a classical OOP design, creating ambiguities that can not be resolved easily (see below). The compositional nature of ECS is free of this problem (see above).
```cpp
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
	//animals_oop.push_back(&turtle_oop); 
	// ERROR: the base class is ambigious, 
	// see https://stackoverflow.com/questions/44878627/inheritance-causes-ambiguous-conversion
	
	animals_oop.push_back(static_cast<LandAnimalOOP*>(&turtle_oop)); 
	// WARNING: this compiles, but now the turtle is not able to swim!
	
	// Print the names and abilities of all animals
	printf("----- OOP inheritance debug output -----\n");
	for (AnimalOOP* animal : animals_oop)
		printf("%s: can swim %d and can walk %d\n", animal->name().c_str(), animal->canDive(), animal->canWalk());
		// This is not what we want, our OOP turtle can't swim :/
```
The corresponding output is
```
Fish: can swim 1 and can walk 0
Horse: can swim 0 and can walk 1
Turtle: can swim 0 and can walk 1
```
Note, the turtle is erroneously reported to not being a swimmer.

### Compilation

Use CMake 3.6 or later to generate platform-independent code or copy-paste the `tiny_ecs.hpp` and `tiny_ecs.cpp` directly to your project.
