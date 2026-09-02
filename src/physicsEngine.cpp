#include "PhysicsEngine.h"
#include "Object.h"
#include "PhysicsData.h"

#include <iostream>

void PhysicsEngine::update() {
	float timeLeft = deltaTime;
	for (int i = 0; i < 5 && timeLeft > 0.00001f; i++) {
		HitData nextHit = earliestCollision(timeLeft);
		float hitTime = nextHit.timeToHit;
		//std::cout << hitTime << std::endl;
		if (hitTime < 0.0f || timeLeft < hitTime) {
			forcesUpdater(timeLeft);
			positionUpdater(timeLeft);
			timeLeft = 0.0f;
		}
		else{
			forcesUpdater(hitTime);
			positionUpdater(hitTime);
			timeLeft -= hitTime;
			resolveCollisions(nextHit);
		}	
	}
	//std::cout << "Energy: " << totalEnergy() << std::endl;
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

// Returns the HitData of when the earliest collision happens within timeLeft passed into the function
HitData PhysicsEngine::earliestCollision(float timeLeft) {
	HitData collision;
	collision.timeToHit = 1.0f;

	for (int i = 0; i < physObjects.size(); ++i) {
		for (int j = i + 1; j < physObjects.size(); ++j) {
			if (!physObjects[i] || !physObjects[j] || !physObjects[i]->physicsProperties || !physObjects[j]->physicsProperties) continue;
			HitData hitData;

			if (physObjects[i]->physicsProperties->collider == ColliderType::Sphere) {
				if (physObjects[j]->physicsProperties->collider == ColliderType::Sphere) {
					hitData = PhysicsEngine::sphereCollisionData(*physObjects[i], *physObjects[j], timeLeft);
				}
				else if (physObjects[j]->physicsProperties->collider == ColliderType::Plane) {
					hitData = PhysicsEngine::planeCollisionData(*physObjects[i], *physObjects[j], timeLeft);
				}
			}
			else if (physObjects[i]->physicsProperties->collider == ColliderType::Plane) {
				if (physObjects[j]->physicsProperties->collider == ColliderType::Sphere) {
					hitData = PhysicsEngine::planeCollisionData(*physObjects[j], *physObjects[i], timeLeft);
				}
				else if (physObjects[j]->physicsProperties->collider == ColliderType::Plane) {
					continue;
				}
			}
			if (hitData.timeToHit >= 0.0f && hitData.timeToHit < collision.timeToHit) {
				collision = hitData;
			}
		}
	}
	return collision;
}

void PhysicsEngine::positionUpdater(float hitTime) {
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		glm::vec3 avgVel{ 0.0f };
		if (object->physicsProperties->enableGravity) {
			avgVel = object->physicsProperties->velocity - 0.5f * gravity * hitTime;
		}
		else {
			avgVel = object->physicsProperties->velocity;
		}

		object->position += avgVel * hitTime;
		
		object->normal = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
		object->left = object->rotationQ * glm::vec3{ 1.0f, 0.0f, 0.0f };
		object->front = object->rotationQ * glm::vec3{ 0.0f, 0.0f, 1.0f };
		object->up = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
	}
}

void PhysicsEngine::forcesUpdater(float timeLeft) {
	// Updates all non kinematic objects
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		if (object->physicsProperties->enableGravity) {
			object->physicsProperties->addForce(object->physicsProperties->mass * gravity);
		}
		glm::vec3 acceleration = object->physicsProperties->totalForces / object->physicsProperties->mass;
		object->physicsProperties->velocity += acceleration * timeLeft;
		object->physicsProperties->totalForces = glm::vec3{ 0.0f };
	}
}

void PhysicsEngine::forcesReset() {
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f) continue;
		object->physicsProperties->totalForces = glm::vec3{ 0.0f };
	}
}

float PhysicsEngine::totalEnergy() { // Used to check conservation of energy for debugging
	float energy = 0.0f;
	for (auto& obj : physObjects) {
		if (!obj || !obj->physicsProperties || obj->physicsProperties->collider == ColliderType::Plane) continue;
		float ke = 0.5f * obj->physicsProperties->mass * glm::dot(obj->physicsProperties->velocity, obj->physicsProperties->velocity);
		float pe = obj->physicsProperties->mass * glm::length(gravity) * obj->position.y;
		energy += ke + pe;
	}
	return energy;
}

// Continuous Collision Detection (CCD)
// Currently assumes one object is an infinite mass plane and one is a sphere
// Works for bounded planes
void PhysicsEngine::planeCollision(HitData& hitData){
	Object* sphere = hitData.objectHitter;
	Object* plane = hitData.objectHit;

	float combinedRestitution = (sphere->physicsProperties->restitution + plane->physicsProperties->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere->physicsProperties->restitution * plane->physicsProperties->restitution) / (sphere->physicsProperties->restitution + plane->physicsProperties->restitution);
	float normalSpeed = glm::dot(hitData.relativeVelocity, hitData.normal);

	if (normalSpeed < 0.0f) {
		sphere->physicsProperties->velocity += -(1 + combinedRestitution) * normalSpeed * hitData.normal;
	}
}

HitData PhysicsEngine::planeCollisionData(Object& sphere, Object& plane, float timeLeft) {
	HitData collision;
	collision.objectHit = &plane;
	collision.objectHitter = &sphere;

	float hitTime = 1.0f;
	float tolerance = 0.0001f;

	glm::vec3 planeToSphere = sphere.position - plane.position;

	glm::vec3 sphereAcceleration = sphere.physicsProperties->totalForces / sphere.physicsProperties->mass + gravity; // Currently cheating the acceleration, will update
	glm::vec3 planeAcceleration = plane.physicsProperties->totalForces / plane.physicsProperties->mass;

	glm::vec3 closestPointOnPlane = plane.position
		+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
		+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);
	glm::vec3 planeToSphereClosest = sphere.position - closestPointOnPlane;
	float distToSphereClosest = glm::length(planeToSphereClosest);
	glm::vec3 dynamicNormal = (distToSphereClosest > 0.00001f) ? glm::normalize(planeToSphereClosest) : plane.up;

	glm::vec3 relativeVelocity = sphere.physicsProperties->velocity - plane.physicsProperties->velocity;
	glm::vec3 relativeAcceleration = sphereAcceleration - planeAcceleration;

	if (distToSphereClosest - sphere.physicsProperties->radius <= 0.0f) { // Ensures infinite loop when contacting plane and moving away doesn't happen, important for sub stepping
		if (glm::dot(relativeVelocity, dynamicNormal) > 0.0f) {
			return collision;
		}
	}

	// When the accelerations are the same, it uses analytical time of impact solving
	// When different it uses conservative advancement since analytical solver breaks for different relative accelerations
	float timeAccumulator = 0.0f; // Accumulator for conservative advancement
	if (glm::length(relativeAcceleration) != 0.0f) { // For different relative accelerations, use conservative advancement
		while (timeAccumulator <= timeLeft) {
			glm::vec3 tempSpherePos = sphere.position + sphere.physicsProperties->velocity * timeAccumulator + 0.5f * sphereAcceleration * timeAccumulator * timeAccumulator;
			glm::vec3 tempPlanePos = plane.position + plane.physicsProperties->velocity * timeAccumulator + 0.5f * planeAcceleration * timeAccumulator * timeAccumulator;
			glm::vec3 tempSphereVel = sphere.physicsProperties->velocity + sphereAcceleration * timeAccumulator;
			glm::vec3 tempPlaneVel = plane.physicsProperties->velocity + planeAcceleration * timeAccumulator;
			
			planeToSphere = tempSpherePos - tempPlanePos;
			closestPointOnPlane = tempPlanePos
				+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
				+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);
			planeToSphereClosest = tempSpherePos - closestPointOnPlane;
			distToSphereClosest = glm::length(planeToSphereClosest);

			//std::cout << "Hit Before: " << hitTime << std::endl;
			if (distToSphereClosest - sphere.physicsProperties->radius <= 0.0f) {
				hitTime = (timeAccumulator >= 0.0f && timeAccumulator < hitTime) ? timeAccumulator : 1.0f;
				break;
			}

			glm::vec3 tempRelativeVel = tempSphereVel - tempPlaneVel;
			float velocityMax = glm::length(tempRelativeVel) + glm::length(relativeAcceleration) * (timeLeft - timeAccumulator);
			if (velocityMax < 0.0f) {
				break;
			}
			float safeTimeStep = (distToSphereClosest - sphere.physicsProperties->radius) / velocityMax;
			if (safeTimeStep < 0.0f) {
				hitTime = (timeAccumulator >= 0.0f && timeAccumulator < hitTime) ? timeAccumulator : 1.0f;
				break;
			}
			timeAccumulator += safeTimeStep;
		}	
	}
	else { // Analytical time solving
		// Quadratic equation used to solve for time
		// a, b, c are derived from projectile motion displacement equation
		float a = glm::dot(relativeVelocity, relativeVelocity);
		float b = 2.0f * glm::dot(planeToSphereClosest, relativeVelocity);
		float c = glm::dot(planeToSphereClosest, planeToSphereClosest) - glm::pow(sphere.physicsProperties->radius, 2.0f);

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant >= 0.0f) {
			float hitTimeTemp = (-b - glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a); // Grabbing the minus result to get earliest hit instead of the hit if the sphere phases through plane on other side
			hitTime = (hitTimeTemp >= 0.0f && hitTimeTemp < hitTime) ? hitTimeTemp : 1.0f;
		}
	}
	
	glm::vec3 sphereImpactVel = sphere.physicsProperties->velocity + sphereAcceleration * hitTime;
	glm::vec3 planeImpactVel = plane.physicsProperties->velocity + planeAcceleration * hitTime;

	glm::vec3 newSpherePos = sphere.position + sphere.physicsProperties->velocity * hitTime + 0.5f * sphereAcceleration * hitTime * hitTime; // Move to impact position to write new hit results into hitData
	planeToSphere = newSpherePos - plane.position;
	closestPointOnPlane = plane.position
		+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
		+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);

	planeToSphereClosest = newSpherePos - closestPointOnPlane;
	distToSphereClosest = glm::length(planeToSphereClosest);
	dynamicNormal = (distToSphereClosest > 0.00001f) ? glm::normalize(planeToSphereClosest) : plane.up;
	relativeVelocity = sphereImpactVel - planeImpactVel;

	//std::cout << "Hit: " << hitTime << std::endl;

	collision.normal = dynamicNormal;
	collision.distance = distToSphereClosest;
	collision.timeToHit = hitTime;
	collision.relativeVelocity = relativeVelocity;
	if (hitTime <= timeLeft && hitTime >= 0.0f) {
		collision.hitPoint = closestPointOnPlane;
	}
	return collision;
}

void PhysicsEngine::sphereCollision(HitData& hitData) {
	Object* sphere1 = hitData.objectHitter;
	Object* sphere2 = hitData.objectHit;

	float combinedRestitution = (sphere1->physicsProperties->restitution + sphere2->physicsProperties->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere1->physicsProperties->restitution * sphere2->physicsProperties->restitution) / (sphere1->physicsProperties->restitution + sphere2->physicsProperties->restitution);

	float normalSpeed = glm::dot(hitData.relativeVelocity, hitData.normal);

	if (normalSpeed <= 0.0f || hitData.distance <= sphere1->physicsProperties->radius + sphere2->physicsProperties->radius) {
		float sphere1InverseMass = 1.0f / sphere1->physicsProperties->mass;
		float sphere2InverseMass = 1.0f / sphere2->physicsProperties->mass;
		float impulse = -(1 + combinedRestitution) * normalSpeed / (sphere1InverseMass + sphere2InverseMass);

		sphere1->physicsProperties->velocity -= impulse * sphere1InverseMass * hitData.normal;
		sphere2->physicsProperties->velocity += impulse * sphere2InverseMass * hitData.normal;
	}
}

HitData PhysicsEngine::sphereCollisionData(Object& sphere1, Object& sphere2, float timeLeft) { // Assumes always same acceleration for both spheres
	HitData collision;
	float hitTime = 1.0f;

	glm::vec3 sphere1ToSphere2 = sphere2.position - sphere1.position;
	glm::vec3 sphere1Normal = glm::normalize(sphere1ToSphere2);

	glm::vec3 sphere1Acceleration = sphere1.physicsProperties->totalForces / sphere1.physicsProperties->mass + gravity;
	glm::vec3 sphere2Acceleration = sphere2.physicsProperties->totalForces / sphere2.physicsProperties->mass + gravity;

	glm::vec3 relativeVelocity = sphere2.physicsProperties->velocity - sphere1.physicsProperties->velocity;
	glm::vec3 relativeAcceleration = sphere2Acceleration - sphere1Acceleration; // Not used currently as its assuming same gravity for both spheres

	float a = glm::dot(relativeVelocity, relativeVelocity);
	float b = 2.0f * glm::dot(sphere1ToSphere2, relativeVelocity);
	float c = glm::dot(sphere1ToSphere2, sphere1ToSphere2) - glm::pow(sphere1.physicsProperties->radius + sphere2.physicsProperties->radius, 2.0f);

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f) {
		float hitTimeTemp = (-b - glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
		hitTime = (hitTimeTemp >= 0.0f && hitTimeTemp < hitTime) ? hitTimeTemp : 1.0f;
	}

	float distance = glm::length(sphere1ToSphere2);
	float surfaceDistance = distance - sphere1.physicsProperties->radius - sphere2.physicsProperties->radius;
	float closingNormVel = glm::dot(relativeVelocity, sphere1Normal);
	float closingNormAccel = glm::dot(relativeAcceleration, sphere1Normal);
	
	glm::vec3 sphere1ImpactVel = sphere1.physicsProperties->velocity + sphere1Acceleration * hitTime;
	glm::vec3 sphere2ImpactVel = sphere2.physicsProperties->velocity + sphere2Acceleration * hitTime;
	glm::vec3 newSphere1Pos = sphere1.position + sphere1.physicsProperties->velocity * hitTime + 0.5f * sphere1Acceleration * hitTime * hitTime;
	glm::vec3 newSphere2Pos = sphere2.position + sphere2.physicsProperties->velocity * hitTime + 0.5f * sphere2Acceleration * hitTime * hitTime;
	sphere1ToSphere2 = newSphere2Pos - newSphere1Pos;
	sphere1Normal = glm::normalize(sphere1ToSphere2);

	relativeVelocity = sphere2ImpactVel - sphere1ImpactVel;

	distance = glm::length(sphere1ToSphere2);
	surfaceDistance = distance - sphere1.physicsProperties->radius - sphere2.physicsProperties->radius;

	collision.objectHit = &sphere2;
	collision.objectHitter = &sphere1;
	collision.normal = sphere1Normal;
	collision.distance = distance;
	collision.timeToHit = hitTime;
	collision.relativeVelocity = relativeVelocity;
	if (hitTime <= timeLeft && hitTime >= 0.0f) {
		collision.hitPoint = sphere1.position + sphere1Normal * sphere1.physicsProperties->radius;
	}
	return collision;
}

// Resolves the collision after velocity and position is updated to exact impact
void PhysicsEngine::resolveCollisions(HitData& hitData) {
	Object* obj1 = hitData.objectHitter;
	Object* obj2 = hitData.objectHit;
	if (!obj1 || !obj2 || !obj1->physicsProperties || !obj2->physicsProperties) return;
	if (obj1->physicsProperties->collider == ColliderType::Sphere) {
		if (obj2->physicsProperties->collider == ColliderType::Sphere) {
			PhysicsEngine::sphereCollision(hitData);
		}
		else if (obj2->physicsProperties->collider == ColliderType::Plane) {
			PhysicsEngine::planeCollision(hitData);
		}
	}
}