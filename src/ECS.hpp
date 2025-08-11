#ifndef ECS_HPP
#define ECS_HPP

// inlcude standard libraries for memorory management, data structures, and type uitilities
#include <memory>
#include <concepts>
#include <vector>
#include <deque>
#include <iostream>
#include <ranges>
#include <bitset>
#include <span>
#include <variant>
#include <cassert>

// external variable to track global component count
extern size_t globalComponentCounter;
namespace cs381 {

	// generate a unique component ID for each component type using static variable
	template<typename T>
	size_t GetComponentID(T reference = {}) {
		static size_t id = globalComponentCounter++;
		return id;
	}

	
	using Entity = uint8_t; // basic componenet storage using a contiguous byte array

	// basic component storage using a contiguous byte array
	struct ComponentStorage {
		size_t elementSize = -1;			// size of a single component
		std::vector<std::byte> data;		// raw storage for component data

		// default constructor
		ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}

		// constructor with known element size
		ComponentStorage(size_t elementSize) : elementSize(elementSize) { 
			data.reserve(5 * elementSize); } // reserve space for 5 components
		
		// template constructor for type-based initialization
		template<typename Tcomponent>
		ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}
		
		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {										// returns a reference to the component of type Tcomponent at entity e
			assert(sizeof(Tcomponent) == elementSize); 					// ensures the size of the component matches the storage size
			assert(e < (data.size() / elementSize));			 		// ensures the entity index is within bounds; checks that entity index is less than the number of components in storage 

			// data.data() gives pointer to the byte array
			// e * elementSize moves that pointer forward to the e-th component
			// cast the bytes to that location into a pointer to Tcomponent
			return *(Tcomponent*)(data.data() + e * elementSize);		// deference pointer to get a reference to the component
		}

		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate(size_t count = 1) {					// allocates space for a component of type Tcomponent for count entities
			assert(sizeof(Tcomponent) == elementSize);								// ensures the size of the component matches the storage size
			assert(count < 100);													// ensures the count is within a reasonable limit, in this case, less than 100
			auto originalEnd = data.size();											// records the current end of the data vector in oringinalEnd
			data.insert(data.end(), elementSize * count, std::byte{0});				// adds space for new component in the data vector by inserting count * elementSize bytes intialized to zero
			for(size_t i = 0; i < count - 1; i++) // Skip the last one				// iterates over the newly allocated space, except the last one
				new(data.data() + originalEnd + i * elementSize) Tcomponent();		// creates Tcomponent objects in the newly allocated space
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(),			// returns a reference to the last component created
				data.size() / elementSize											// return the index of last component created
			};
		}

		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);								// ensures size of Tcomponent matches the elementSize (size of each component in data)
			size_t size = data.size() / elementSize;								// stores the number of components currently in data in size
			if (size <= e)															// checks if current number of components is less than or equal to index e (the requested entity)

				// if the requested entity does not exist, allocate space for it
				// calculates how many new components need to be allocated to ensure there is enough space for the entity at index e.
				Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size + 1, 1));	
			return Get<Tcomponent>(e);												// returns a reference to the component at index e
		}
	};


	template<typename Storage = ComponentStorage>					// template parameter w/ a default storage value of ComponentStorage 
	struct Scene {
		std::vector<std::vector<bool>> entityMasks;					// vectors of vectors of booleans; each vector represents an entity and boolean represents if a component is associated with said entity
		std::vector<Storage> storages = {Storage()};				// vector that stores storage objects for each component type 

		template<typename Tcomponent>																		// starts the template function to work with any type Tcomponent
		Storage& GetStorage() {																				// returns a reference to the storage object for the component type Tcomponent
			size_t id = GetComponentID<Tcomponent>();														// gets the component ID for the requested component type
			if(storages.size() <= id)																		// checks if current size of storages is less than equal to id
																											// if so, it means that the storages vector doesn't have enough storage for the requested component type
				storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), Storage());	// inserts a number of storage elements into storages to ensure there's enough room for the requested id 
																											// id - storages.size() gives the number of new storages to insert
																											// std::max<int64_t>(id - storages.size(), 1) ensures at least one new storage is inserted
			if (storages[id].elementSize == std::numeric_limits<size_t>::max())								// checks if elementSize of tht Storage position at position id is equal to the maximum possible value of size_t
																											// if so, it means that the Storage for this component type has not been initialized yet
				storages[id] = Storage(Tcomponent{});														// creates new Storage object and assigns it to storages[id] with an empty object of type Tcomponent
			return storages[id];																			// returns a reference to the Storage object for the component type Tcomponent at index id in the storages vector
		}

		Entity CreateEntity() {										// creates new entity in ECS system
			Entity e = entityMasks.size();							// number of entities in the system; index/ID of the new entity
			entityMasks.emplace_back(std::vector<bool>{false});		// adds a new element to the entityMasks vector
																	// std::vector<bool>{false} creates a new vector of booleans with one element initialized to false 
																	// (doesn't have component yet)
			return e;												// returns the index/ID of the new entity
		}

		template<typename Tcomponent>												
		Tcomponent& AddComponent(Entity e) {										// adds a component of type Tcomponent to the entity e
			size_t id = GetComponentID<Tcomponent>();								// gets the component ID for the requested component type stored in id
			auto& eMask = entityMasks[e];											// accessing the mask of entity e
			if(eMask.size() <= id)												 	// checks if the entity's mask has enough space to store new component's status (true/false)
																					// if so, the entity has not been assigned components with higher IDs yet
				eMask.resize(id + 1, false);										// resizes  the mask to make room for the new component
																					// it resizes the mask to have a least id + 1 elements, and it initializes the new elements to false
			eMask[id] = true;														// sets component's mask index id to true; that the entity e now has the component Tcomponent.
			return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);	// returns a reference to the component of type Tcomponent associated with entity e
		}

		template<typename Tcomponent>
		void RemoveComponent(Entity e) {				// removes a component of type Tcomponent from the entity e
			size_t id = GetComponentID<Tcomponent>();	// gets the component ID for the requested component type
			auto& eMask = entityMasks[e];				// accessing the mask of entity e
			if(eMask.size() > id)						// checks if the entity's mask has enough space to store new component's status (true/false)
				eMask[id] = false;						// sets component's mask index id to false; that the entity e no longer has the component Tcomponent
		}

		template<typename Tcomponent>
		Tcomponent& GetComponent(Entity e) {								
			size_t id = GetComponentID<Tcomponent>();						// gets the component ID for the requested component type
			assert(entityMasks[e][id]);										// checks if the entity e has the component Tcomponent
			return GetStorage<Tcomponent>().template Get<Tcomponent>(e);	// returns a reference to the component of type Tcomponent associated with entity e
		}

		template<typename Tcomponent>
		bool HasComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();					// gets the component ID for the requested component type
			return entityMasks[e].size() > id && entityMasks[e][id];	// checks if the entity e has the component Tcomponent by
																		// checking if the mask has enough space and if the component is set to true
		}
	};




	// Niceties!

	struct SkiplistComponentStorage {	// manages the storage of components in a skiplist-like structure
		size_t elementSize = -1;		// size of a single component in bytes; initialized to -1 which acts like an invalid or unintialized state value
		std::vector<size_t> indecies;	// vector that stores the index of each entity's component in the data vector
		std::vector<std::byte> data;	// vector that stores all component data for all entities

		// default constructor
		// initializes elementSize to -1, indecies to a vector of size 1 with value -1, and data to a vector of size 1 with value 0
		SkiplistComponentStorage() : elementSize(-1), indecies(1, -1), data(1, std::byte{0}) {}

		// constructor with known element size; reserves space for 5 components
		SkiplistComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }
		
		// template constructor for type-based initialization
		template<typename Tcomponent>
		SkiplistComponentStorage(Tcomponent reference = {}) : SkiplistComponentStorage(sizeof(Tcomponent)) {}

		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);					// ensures size of component matches the elementSize
			assert(e < indecies.size());								// ensures the entity index is within bounds 
			assert(indecies[e] != std::numeric_limits<size_t>::max());	// ensures the entity has a valid index in the data vector
			return *(Tcomponent*)(data.data() + indecies[e]);			// returns deference pointer to get a reference to the component
																		// 		data.data() gives pointer to the byte array
																		// 		indecies[e] moves that pointer forward to the e-th component
		}

		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate() {
			assert(sizeof(Tcomponent) == elementSize);						// ensures size of component matches the elementSize
			data.insert(data.end(), elementSize, std::byte{0});				// adds space for new component in the data vector by inserting elementSize bytes intialized to zero
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(), // constructs a new Tcomponent object in the newly allocated space
				(data.size() - 1) / elementSize								// returns the index of the newly allocated component
			};
		}

		template<typename Tcomponent>
		Tcomponent& Allocate(Entity e) {				// allocates new component of type Tcomponent for the entity e
			auto [ret, i] = Allocate<Tcomponent>(); 	// calls the previous Allocate<Tcomponent>() function that returns a pair: reference to new component (ret) and the index of the it (i)
			indecies[e] = i * elementSize;				// sets the index of the entity e in indecies to the index of the new component multiplied by elementSize to get the byte offset
			return ret;									// returns the reference to the new component
		}

		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);				// ensures size of component matches the elementSize
			if (indecies.size() <= e)								// checks if the indecies vector has enough space for the entity e

				// resize the indecies vector with enough space for the entity e while initializing the new elements to -1
				indecies.insert(indecies.end(), std::max<int64_t>(int64_t(e) - indecies.size() + 1, 1), -1);
			if (indecies[e] == std::numeric_limits<size_t>::max())	// checks if the entity e has a valid index in the data vector
				return Allocate<Tcomponent>(e);						// if not, allocate a new component of type Tcomponent for the entity e
			return Get<Tcomponent>(e);								// otherwise, return the reference to the component of type Tcomponent associated with entity e
		}
	};


	using post_increment_t = int;			// creates a type alias for int called post_increment_t

	template<typename... Tcomponents>				// template function that takes a variadic number of component types
	struct SceneView {
		Scene<SkiplistComponentStorage>& scene;		// reference to the scene object that contains the components

		struct Sentinel {};							// empty struct used to mark the end of the iteration range
		struct Iterator {							// struct that represents an interator over the entities in the scene
			Scene<SkiplistComponentStorage>* scene = nullptr;	// pointer to the scene object
			Entity e;											// current entity index

			// bool function that checks if the current entity e has ALL the components specified in Tcomponents
			bool valid() { return (scene->HasComponent<Tcomponents>(e) && ...); }

			// this operator checks if the iterator has reached the end of the scene
			// if the scene is null or the entity index e exceeds or equal to the # of entities, it means it has reached the end of the iteration
			bool operator==(Sentinel) { return scene == nullptr || e >= scene->entityMasks.size(); }

			Iterator& operator++(post_increment_t) { 					// post-increment operator
				do {
					e++;												// increments the entity index e
				} while(!valid() && e < scene->entityMasks.size());		// checks if the current entity e has all the components specified in Tcomponents
																		// checks if current entity index e is within the bounds of the entityMasks vector
				return *this;											// returns the iterator object
			}

			Iterator operator++() {			// pre-increment operator
				Iterator old;				// creates a temp iterator object named old
				operator++(0);				// calls the post-increment version of operator++ with 0
											// increments the entity index e and moves it into the next valid entity
				return old;					// returns the old iterator object
			}

			// Entity operator*() { return e; }

			// dereference returns a tuple of references to the components of the current entity e
			std::tuple<std::add_lvalue_reference_t<Tcomponents>...> operator*() { return { scene->GetComponent<Tcomponents>(e)... }; }
		};

		Iterator begin() { 
			Iterator out{&scene, 0}; 		// creates new iterator obeject out
											// &scene is the iterator pointer to the scene object
											// 0 is the starting entity index
			if(!out.valid()) ++out;			// checks if the current entity e has all the components specified in Tcomponents
											// if so, it increments the entity index e to the next valid entity
			return out;						// return the first valid iterator
		}
		Sentinel end() { return {}; }	// defines the end range for iteration
	};
}

#endif // ECS_HPP