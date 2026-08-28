#include "PhysicsEngine.h"
#include "Object.h"
#include "PhysicsData.h"

void PhysicsEngine::update() {
	forcesUpdater(deltaTime);

	float hitTime = earliestCollision(deltaTime);

	if (hitTime <= deltaTime && hitTime >= 0.0f) {
		positionUpdater(hitTime, deltaTime);
		resolveCollisions(deltaTime - hitTime);
		positionUpdater(deltaTime - hitTime, deltaTime);
	}
	else{
		positionUpdater(deltaTime, deltaTime);
	}

	if (hitTime < 0.0f || hitTime == 1000.0f) {
		resolveCollisions(deltaTime);
	}
}

// Updates all kinematic objects
// Currently only for straight, point-to-point, movements
// CURRENTLY NOT WORKING, WILL UPDATE
void PhysicsEngine::kinematicUpdater(float deltaTime) {
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || !object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		glm::vec3 oldPos = object->position;

		// Calculates how much to move
		float oldOffset = (std::sin(object->physicsProperties->accumulatedTime) * object->physicsProperties->moveDistance);
		object->physicsProperties->accumulatedTime += deltaTime * object->physicsProperties->moveSpeed;
		float newoffset = (std::sin(object->physicsProperties->accumulatedTime) * object->physicsProperties->moveDistance);

		object->position += (object->physicsProperties->moveDirection * (newoffset - oldOffset));

		object->physicsProperties->velocity = (object->position - oldPos) / deltaTime;
	}	
}

float PhysicsEngine::earliestCollision(float deltaTime) {
	float hitTime = 1000.0f;

	for (int i = 0; i < physObjects.size(); ++i) {
		for (int j = i + 1; j < physObjects.size(); ++j) {
			if (!physObjects[i] || !physObjects[j] || !physObjects[i]->physicsProperties || !physObjects[j]->physicsProperties || (physObjects[i]->physicsProperties->collider == ColliderType::Plane && physObjects[j]->physicsProperties->collider == ColliderType::Plane)) continue;
			Object* sphere = (physObjects[i]->physicsProperties->collider != ColliderType::Plane) ? physObjects[i] : physObjects[j];
			Object* plane = (physObjects[i]->physicsProperties->collider == ColliderType::Plane) ? physObjects[i] : physObjects[j];
			
			glm::vec3 planeToSphereVec = sphere->position - plane->position; // From center of plane to center of sphere

			glm::vec3 closestPointOnPlane = plane->position
				+ plane->left * glm::clamp(glm::dot(planeToSphereVec, plane->left), -plane->physicsProperties->planeWidth / 2, plane->physicsProperties->planeWidth / 2)
				+ plane->front * glm::clamp(glm::dot(planeToSphereVec, plane->front), -plane->physicsProperties->planeHeight / 2, plane->physicsProperties->planeHeight / 2);

			glm::vec3 planeToSphereClosest = sphere->position - closestPointOnPlane;

			float distToPlaneClosest = glm::length(planeToSphereClosest);			

			glm::vec3 dynamicNormal = (distToPlaneClosest > 0.00001f) ? glm::normalize(planeToSphereClosest) : plane->up;

			float projVelNormalTowardsPlane = glm::dot(sphere->physicsProperties->velocity, dynamicNormal); // Projected approach velocity towards plane

			if (projVelNormalTowardsPlane < -0.5f) {
				float avgApproachSpeed = glm::dot(sphere->physicsProperties->velocity - 0.5f * glm::vec3{ 0.0f, gravity, 0.0f } * deltaTime, dynamicNormal);

				float surfaceDistToPlaneClosest = distToPlaneClosest - sphere->physicsProperties->radius;

				float tempHitTime = (glm::length(sphere->physicsProperties->velocity) > 0.00001f) ? (-surfaceDistToPlaneClosest / avgApproachSpeed) : 1000.0f;

				if (hitTime > tempHitTime) hitTime = tempHitTime;
			}
		}
	}
	return hitTime;
}

void PhysicsEngine::positionUpdater(float hitTime, float deltaTime) {
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		glm::vec3 avgVel{ 0.0f };
		if (object->physicsProperties->enableGravity) {
			avgVel = object->physicsProperties->velocity - 0.5f * glm::vec3{ 0.0f, gravity, 0.0f } * deltaTime;
		}
		object->physicsProperties->isGrounded = false; // Makes the collision checker recheck object grounded state

		object->position += avgVel * hitTime;
		
		object->normal = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
		object->left = object->rotationQ * glm::vec3{ 1.0f, 0.0f, 0.0f };
		object->front = object->rotationQ * glm::vec3{ 0.0f, 0.0f, 1.0f };
		object->up = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
	}
}

void PhysicsEngine::forcesUpdater(float deltaTime) {
	// Updates all non kinematic objects
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		if (object->physicsProperties->enableGravity) {
			object->physicsProperties->addForce({ 0.0f, object->physicsProperties->mass * gravity, 0.0f });
		}
		glm::vec3 acceleration = object->physicsProperties->totalForces / object->physicsProperties->mass;
		object->physicsProperties->velocity += acceleration * deltaTime;
		object->physicsProperties->totalForces = glm::vec3{ 0.0f };
	}
}

// Continuous Collision Detection (CCD)
// Currently assumes one object is an infinite mass plane and one is not a plane
// Works for bounded planes and has resting state
void PhysicsEngine::planeCollision(Object* obj1, Object* obj2, const float& deltaTime){
	if (!obj1 || !obj2 || !obj1->physicsProperties || !obj2->physicsProperties || (obj1->physicsProperties->collider == ColliderType::Plane && obj2->physicsProperties->collider == ColliderType::Plane)) return;
	Object* sphere = (obj1->physicsProperties->collider != ColliderType::Plane) ? obj1 : obj2;
	Object* plane = (obj1->physicsProperties->collider == ColliderType::Plane) ? obj1 : obj2;

	glm::vec3 planeToSphereVec = sphere->position - plane->position; // From center of plane to center of sphere

	float combinedRestitution = (sphere->physicsProperties->restitution + plane->physicsProperties->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere->physicsProperties->restitution * plane->physicsProperties->restitution) / (sphere->physicsProperties->restitution + plane->physicsProperties->restitution);

	// Gets the point on the plane closest to sphere, bounded by the plane width and height
	glm::vec3 closestPointOnPlane = plane->position 
		+ plane->left * glm::clamp(glm::dot(planeToSphereVec, plane->left), -plane->physicsProperties->planeWidth / 2, plane->physicsProperties->planeWidth / 2)
		+ plane->front * glm::clamp(glm::dot(planeToSphereVec, plane->front), -plane->physicsProperties->planeHeight / 2, plane->physicsProperties->planeHeight / 2);

	glm::vec3 planeToSphereClosest = sphere->position - closestPointOnPlane;

	float distToPlaneClosest = glm::length(planeToSphereClosest);

	glm::vec3 dynamicNormal = (distToPlaneClosest > 0.00001f) ? glm::normalize(planeToSphereClosest) : plane->up; // Normal that follows the sphere's movements, used to bounce off edges mainly

	float surfaceDistToPlaneClosest = distToPlaneClosest - sphere->physicsProperties->radius;

	float projVelNormalTowardsPlane = glm::dot(sphere->physicsProperties->velocity, dynamicNormal); // Projected approach velocity towards plane

	if (projVelNormalTowardsPlane < -0.5f) { // Only if moving towards plane
		float avgApproachSpeed = glm::dot(sphere->physicsProperties->velocity - 0.5f * glm::vec3{ 0.0f, gravity, 0.0f } * deltaTime, dynamicNormal);
		
		float hitTime = (glm::length(sphere->physicsProperties->velocity) > 0.00001f) ? (-surfaceDistToPlaneClosest / avgApproachSpeed) : 1.0f;

		if (hitTime < 0.0f) hitTime = 0.0f;

		if (hitTime >= 0.0f && hitTime <= deltaTime) { // Check if the plane is being hit within the frame
			glm::vec3 velAtImpact = sphere->physicsProperties->velocity - glm::vec3{ 0.0f, gravity, 0.0f } * (deltaTime - hitTime);

			sphere->physicsProperties->velocity += -(1 + combinedRestitution) * glm::dot(velAtImpact, dynamicNormal) * dynamicNormal;
		}
	}
	else if (projVelNormalTowardsPlane >= -0.5f && projVelNormalTowardsPlane <= 0.5f && distToPlaneClosest <= sphere->physicsProperties->radius) { // Only if near plane and not moving too much away or towards plane
		sphere->physicsProperties->isGrounded = true;

		glm::vec3 velocityPerpendicular = glm::dot(dynamicNormal, sphere->physicsProperties->velocity) * dynamicNormal;

		sphere->physicsProperties->velocity -= velocityPerpendicular;
		if(glm::length(planeToSphereClosest) < sphere->physicsProperties->radius){
			sphere->position += (sphere->physicsProperties->radius - glm::length(planeToSphereClosest)) * dynamicNormal;
		}		
		else {
			sphere->position -= (glm::length(planeToSphereClosest) - sphere->physicsProperties->radius) * dynamicNormal;
		}
	}
}

// Checks all objects for collisions
// Will replace with a more efficient scan
void PhysicsEngine::resolveCollisions(const float& deltaTime) {
	for (int i = 0; i < physObjects.size(); ++i) {
		for (int j = i + 1; j < physObjects.size(); ++j) {
			//if(physObjects[i]->physicsProperties->collider != BoundingBox::Plane && physObjects[j])
			PhysicsEngine::planeCollision(physObjects[i], physObjects[j], deltaTime);
		}
	}
}