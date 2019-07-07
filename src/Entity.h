#pragma once
/**

File: Entity.h
Description:

Header file for Entity.h, a game engine actual entity, marrying together all aspects

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "Model.h"
#include "LuaEngine.h"
#include "Shader.h"
#include "Log.h"

using string = std::string;

namespace darksun {

	class Entity {

	private:
		std::shared_ptr<Model> model = NULL;
		LuaEngine engine;

		// Blueprint stuff
		string bpName;

		// My id
		long myId;

		// Internal name
		string internalName;

		// Position information
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		// Pathfinding information
		std::vector<glm::vec3> pathfindingWaypoints;
		int currentPathfindingWaypoint = 0;
		bool pathfinding = false;
		glm::vec3 targetPos = glm::vec3(0, 0, 0);

		// Rotation information
		glm::vec3 targetRot = glm::vec3(0, 0, 0);

		// bools
		bool valid = false;
		bool hasScript = false;

		// Does the init stuff, should only be called in the constructor
		void init(string blueprintn, long newId);
		void initLuaEngine();

		// Set a new target to move to in the world
		void setMoveTarget(float x, float y, float z) {
			targetPos = glm::vec3(x,y,z);
			float dist = glm::distance(targetPos, position);

			// Replace this value with the size of the entity, as if our new position is within us there's really no point in moving?
			if (dist >= 0.2) {
				recalculatePathfinding();
			}
		}

		// Calculate the pathfinding to the position we are targeting
		void recalculatePathfinding();

		// Returns if we are currently pathfinding somewhere
		bool isPathfinding() { return pathfinding; }

		// Execute pathfinding
		void moveOnTick(glm::vec3& p, float deltaTime);

		// Rotate ourselves to a orientation
		void faceDirection(float x, float y, float z) {
			targetRot = glm::vec3(x, y, z);;
		}

		// Execute rotation
		void rotateOnTick(glm::vec3& r, float deltaTime);

		// Static stuff
		static long LastEntityId;
	public:
		static long createNewId();

		Entity(string blueprintn, long newId = Entity::createNewId());

		// Tick the entity
		void tick(float deltaTime);

		// Draw the entity
		void draw(Shader* shader, bool drawReflection = false, bool reflectiveSurface = false);

		// Returns if the entity init correctly
		bool isValid() { return valid; }

		// Returns the entity id
		long getId() { return myId; }

		// Returns the position vector
		glm::vec3 getPosition() { return position; }

		// Sets the position of the entity
		void setPosition(glm::vec3 newp) { position = newp; }

	};

}