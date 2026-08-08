#include "physicsEngine.h"
#include "object.h"
#include "physics.h"

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
		object->physics->totalForces = glm::vec3{ 0.0f };
		object->physics->isGrounded = false; // Makes the collision checker recheck object grounded status
	}
	checkCollisions();
}

// Currently assumes one object is a plane and one is not
// Also only for Y axis bounces and no plane rotation/translation beside up and down
void PhysicsEngine::planeCollision(Object* obj1, Object* obj2){
	if (!obj1 || !obj2 || !obj1->physics || !obj2->physics) return;
	Object* sphere;
	Object* plane;
	float sphereBottom = 0.0f;
	if (obj1->physics->collider != BoundingBox::Plane) {
		sphere = obj1;
		plane = obj2;
	}
	else
	{
		sphere = obj2;
		plane = obj1;
	}
	if (glm::distance(sphere->position, plane->position) > sphere->physics->radius + 0.5f) return;
	sphereBottom = sphere->position.y - sphere->physics->radius;

	float groundPenetration = plane->position.y - sphereBottom;
	float combinedRestitution = sphere->physics->restitution * plane->physics->restitution;
	float frameFraction = 0.0f;
	if (std::abs(sphere->physics->velocity.y) != 0.0f) {
		frameFraction = std::abs(groundPenetration / sphere->physics->velocity.y);
		frameFraction = std::min(frameFraction, 1.0f);
	}
	float gravityVelocityAdded = sphere->physics->gravity * (1.0f / 60.0f) * frameFraction;

	sphere->physics->velocity.y -= gravityVelocityAdded;

	// Check if grounded
	if (std::abs(sphere->physics->velocity.y - plane->physics->velocity.y) < 0.25f && groundPenetration > -0.05f) { 
		sphere->physics->isGrounded = true;
		sphere->position.y = plane->position.y + sphere->physics->radius;
		sphere->physics->velocity = plane->physics->velocity;
	}
	else if (groundPenetration > 0.0f && !sphere->physics->isGrounded) { // Calculate bounce
		sphere->physics->velocity.y = plane->physics->velocity.y + (plane->physics->velocity.y  - sphere->physics->velocity.y) * combinedRestitution;
		sphere->physics->velocity.y += gravityVelocityAdded;
		sphere->position.y = plane->position.y + sphere->physics->radius + (1.0f / 60.0f) * sphere->physics->velocity.y * frameFraction;
	}
}

// Checks all objects for collisions
// Will replace with a more efficient scan
void PhysicsEngine::checkCollisions() {
	for (int i = 0; i < physObjects.size(); ++i) {
		for (int j = i + 1; j < physObjects.size(); ++j) {
			//if(physObjects[i]->physics->collider != BoundingBox::Plane && physObjects[j])
			PhysicsEngine::planeCollision(physObjects[i], physObjects[j]);
		}
	}
}