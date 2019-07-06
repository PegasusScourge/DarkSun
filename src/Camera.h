#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <algorithm>

namespace darksun {
	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		NONE
	};

	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	public:
		// Camera Attributes
		glm::vec3 position; // This is manipulated for the tactical zoom
		glm::vec3 groundPosition; // This is the value that moves through space
		glm::vec3 Front = glm::vec3(1, 0, 0);;
		glm::vec3 Up = glm::vec3(0, 1, 0);
		glm::vec3 Right = glm::vec3(0, 0, 1);
		// Camera options
		float movementSpeed = 25.0f;
		float mouseSensitivity = 0.1f;
		float Zoom = 45.0f;

		float tacticalZPercent = 0.5f; // Between 0 and 1

		float tacticalMinHeight = 60.0f;
		float tacticalHeightDelta = 140.0f;
		float tacticalXDelta = 45.0f;

		// Camera values we store
		float lastMouseX = 0;
		float lastMouseY = 0;

		// Constructor
		Camera() {
			position = glm::vec3(0, 0, 0);
			groundPosition = glm::vec3(0, 0, 0);
			tacticalZoom(0, 0);
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix() {
			update();
			return glm::lookAt(position, groundPosition, Up);
		}

		// Handle events
		void handleEvent(sf::Event& event, float deltaTime, bool hasFocus) {
			if (!hasFocus) // Don't process if we aren't focused
				return;
			
			//if (event.type == sf::Event::MouseMoved) {
			//	int newX = event.mouseMove.x;
			//	int newY = event.mouseMove.y;
			//	float dX = newX - lastMouseX;
			//	float dY = newY - lastMouseY;
			//	dout.verbose("Mouse move event, dX=" + std::to_string(dX) + " dY=" + std::to_string(dY));
			//	ProcessMouseMovement(dX, dY);
			//	lastMouseX = newX; lastMouseY = newY;
			//}
			//else 
			if (event.type == sf::Event::MouseWheelScrolled) {
				//dout.verbose("Got mouse scroll event: " + std::to_string(event.mouseWheelScroll.delta));
				tacticalZoom(event.mouseWheelScroll.delta, deltaTime);
			}
		}
		void pollKeyboard(float deltaTime) {
			Camera_Movement mov = Camera_Movement::NONE;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				mov = Camera_Movement::FORWARD;
				ProcessKeyboard(mov, deltaTime);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				mov = Camera_Movement::LEFT;
				ProcessKeyboard(mov, deltaTime);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				mov = Camera_Movement::BACKWARD;
				ProcessKeyboard(mov, deltaTime);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				mov = Camera_Movement::RIGHT;
				ProcessKeyboard(mov, deltaTime);
			}
		}

		void tacticalZoom(float ticks, float deltaTime) {
			// Apply a zoom amount
			tacticalZPercent += ticks * 2.0f * sqrtf(0.05f + tacticalZPercent) * deltaTime;
			if (tacticalZPercent > 0.8f)
				tacticalZPercent = 0.8f;
			if (tacticalZPercent < 0.0f) 
				tacticalZPercent = 0.0f;

			position.z = groundPosition.z;
			position.x = groundPosition.x - ((1.0f - tacticalZPercent) * + tacticalXDelta);
			position.y = tacticalMinHeight + std::min(tacticalZPercent * tacticalHeightDelta, 2000.0f);
		}

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = movementSpeed * deltaTime;
			if (direction == FORWARD)
				groundPosition += Front * velocity;
			if (direction == BACKWARD)
				groundPosition -= Front * velocity;
			if (direction == LEFT)
				groundPosition -= Right * velocity;
			if (direction == RIGHT)
				groundPosition += Right * velocity;
		}

		void update(glm::vec3 nPos, float nZoom) {
			groundPosition = glm::vec3(nPos.x, 0.0f, nPos.y);
			if (nZoom >= 0.0f && nZoom <= 0.8f)
				tacticalZPercent = nZoom;
			tacticalZoom(0, 0);
		}

		void update() {
			tacticalZoom(0, 0);
		}

		void setTacticalZoomParams(float minHeight, float maxHeight, float xDelta) {
			tacticalMinHeight = minHeight;
			tacticalHeightDelta = maxHeight - minHeight;
			tacticalXDelta = xDelta;
			tacticalZoom(0, 0);
		}
	};
}

