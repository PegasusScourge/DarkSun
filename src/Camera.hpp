#pragma once
/**

File: Camera.hpp
Description:

Camera

Thread safe

*/

//#include <GL/glew.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include <SFML/Window.hpppp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <algorithm>
#include <mutex>
#include <atomic>

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
	private:
		
		std::mutex event_mutex;
		std::mutex poll_mutex;

		// Camera Attributes
		std::atomic<glm::vec3> position; // This is manipulated for the tactical zoom
		std::atomic<glm::vec3> groundPosition; // This is the value that moves through space
		std::atomic<glm::vec3> Front = glm::vec3(1, 0, 0);;
		std::atomic<glm::vec3> Up = glm::vec3(0, 1, 0);
		std::atomic<glm::vec3> Right = glm::vec3(0, 0, 1);
		// Camera options
		std::atomic<float> movementSpeed = 50.0f;
		std::atomic<float> mouseSensitivity = 0.1f;
		std::atomic<float> Zoom = 45.0f;

		std::atomic<float> tacticalZPercent = 0.5f; // Between 0 and 1

		std::atomic<float> tacticalMinHeight = 60.0f;
		std::atomic<float> tacticalHeightDelta = 140.0f;
		std::atomic<float> tacticalXDelta = 45.0f;

		// Camera values we store
		std::atomic<float> lastMouseX = 0;
		std::atomic<float> lastMouseY = 0;

	public:

		// Constructor
		Camera() {
			position = glm::vec3(0, 0, 0);
			groundPosition = glm::vec3(0, 0, 0);
			tacticalZoom(0, 0);
		}

		glm::vec3 getGroundPosition() {
			return groundPosition.load();
		}

		glm::vec3 getPosition() {
			return position.load();
		}

		float getZoom() {
			return Zoom.load();
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix() {
			update();
			glm::vec3 tempPos = position.load();
			glm::vec3 tempGPos = groundPosition.load();
			glm::vec3 tempUp = Up.load();
			return glm::lookAt(tempPos, tempGPos, tempUp);
		}

		// Handle events
		void handleEvent(sf::Event& event, float deltaTime) {
			std::lock_guard lock(event_mutex);

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
			std::lock_guard lock(poll_mutex);

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
			tacticalZPercent = tacticalZPercent + (ticks* 2.0f * sqrtf(0.05f + tacticalZPercent) * deltaTime);
			if (tacticalZPercent > 0.8f)
				tacticalZPercent = 0.8f;
			if (tacticalZPercent < 0.0f) 
				tacticalZPercent = 0.0f;

			glm::vec3 tempPos;
			tempPos.z = groundPosition.load().z;
			tempPos.x = groundPosition.load().x - ((1.0f - tacticalZPercent) * + tacticalXDelta);
			tempPos.y = tacticalMinHeight + std::min(tacticalZPercent * tacticalHeightDelta, 2000.0f);
			position.store(tempPos);
		}

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = movementSpeed * deltaTime * (1.0f + (tacticalZPercent*3.0f));
			if (direction == FORWARD)
				groundPosition = groundPosition.load() + (Front.load() * velocity);
			if (direction == BACKWARD)
				groundPosition = groundPosition.load() - (Front.load() * velocity);
			if (direction == LEFT)
				groundPosition = groundPosition.load() + (Right.load() * -velocity);
			if (direction == RIGHT)
				groundPosition = groundPosition.load() + (Right.load() * velocity);
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

