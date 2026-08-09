#include "physicsEngine.h"
#include "object.h"
#include "physics.h"

#include <iostream>

void PhysicsEngine::update(float deltaTime) {
	// Updates all kinematic objects
	// Currently only for straight movements
	for (auto& object : physObjects) {
		if (!object || !object->physics || !object->physics->isKinematic || object->physics->mass <= 0.0f) continue;
		glm::vec3 oldPos = object->position;

		// Calculates how much to move
		float oldOffset = (std::sin(object->physics->accumulatedTime) * object->physics->moveDistance);
		object->physics->accumulatedTime += deltaTime * object->physics->moveSpeed;
		float newoffset = (std::sin(object->physics->accumulatedTime) * object->physics->moveDistance);

		object->position += (object->physics->moveDirection * (newoffset - oldOffset));

		object->physics->velocity = (object->position - oldPos) / deltaTime;
	}

	// Updates all non kinematic objects
	for (auto &object : physObjects) {
		if (!object || !object->physics || object->physics->isKinematic || object->physics->mass <= 0.0f) continue;
		if (object->physics->enableGravity && !object->physics->isGrounded) {
			object->physics->addForce({ 0.0f, object->physics->mass * object->physics->gravity, 0.0f });
		}
		glm::vec3 acceleration = object->physics->totalForces / object->physics->mass;
		object->physics->velocity += acceleration * deltaTime;
		object->position += object->physics->velocity * deltaTime;
		object->physics->isGrounded = false; // Makes the collision checker recheck object grounded status
		object->physics->totalForces = glm::vec3{ 0.0f };
	}
	checkCollisions(deltaTime);
}

// Currently assumes one object is an infinite mass plane and one is not a plane
// Also only for Y axis bounces and no plane rotation/translation beside up and down
void PhysicsEngine::planeCollision(Object* obj1, Object* obj2, const float& deltaTime){
	if (!obj1 || !obj2 || !obj1->physics || !obj2->physics || (obj1->physics->collider == BoundingBox::Plane && obj2->physics->collider == BoundingBox::Plane)) return;
	Object* sphere = (obj1->physics->collider != BoundingBox::Plane) ? obj1 : obj2;
	Object* plane = (obj1->physics->collider == BoundingBox::Plane) ? obj1 : obj2;

	float sphereBottom = 0.0f;

	if (glm::distance(sphere->position, plane->position) > sphere->physics->radius + 0.5f) return; // Doesn't run through collision checker if too far away

	sphereBottom = sphere->position.y - sphere->physics->radius;

	float groundPenetration = plane->position.y - sphereBottom;
	float combinedRestitution = (sphere->physics->restitution + plane->physics->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere->physics->restitution * plane->physics->restitution) / (sphere->physics->restitution + plane->physics->restitution);
	float frameFraction = 0.0f; // Percentage of the frame that ball was penetrating ground

	// Check if grounded
	if (std::abs(sphere->physics->velocity.y - plane->physics->velocity.y) < 0.25f && groundPenetration > -0.05f) { 
		sphere->physics->isGrounded = true;
		sphere->position.y = plane->position.y + sphere->physics->radius;
		sphere->physics->velocity = plane->physics->velocity;
	}
	else if (groundPenetration > 0.0f && !sphere->physics->isGrounded) { // Calculate bounce if sphere penetrates ground
		if (glm::dot(sphere->physics->velocity, sphere->physics->velocity) > 0.00001f) { // Check if object isn't stationary
			frameFraction = std::abs(groundPenetration / (sphere->physics->velocity.y * deltaTime));
			frameFraction = std::min(frameFraction, 1.0f); // If object is travelling too fast, issues with reduced bounce, will be fixed
		}

		std::cout << "Penetration: " << groundPenetration << std::endl;
		std::cout << "Frame Fraction: " << frameFraction << std::endl;
		std::cout << "-Initial Y Velocity: " << sphere->physics->velocity.y << std::endl;
		std::cout << "-Initial Y Position: " << sphere->position.y << std::endl;

		// Inverts velocity of sphere, adds its bounced exit velocity based on restitution, and adds velocity of platform 
		sphere->physics->velocity.y = plane->physics->velocity.y + (plane->physics->velocity.y  - sphere->physics->velocity.y) * combinedRestitution;

		std::cout << "+Final Y Velocity: " << sphere->physics->velocity.y << std::endl;
		std::cout << "+Final Y Position: " << sphere->position.y << std::endl;
		std::cout << std::endl;

		sphere->position.y = plane->position.y + sphere->physics->radius + deltaTime * sphere->physics->velocity.y * frameFraction; // Sets the sphere to correct position after bounce
	}
}

// Checks all objects for collisions
// Will replace with a more efficient scan
void PhysicsEngine::checkCollisions(const float& deltaTime) {
	for (int i = 0; i < physObjects.size(); ++i) {
		for (int j = i + 1; j < physObjects.size(); ++j) {
			//if(physObjects[i]->physics->collider != BoundingBox::Plane && physObjects[j])
			PhysicsEngine::planeCollision(physObjects[i], physObjects[j], deltaTime);
		}
	}
}