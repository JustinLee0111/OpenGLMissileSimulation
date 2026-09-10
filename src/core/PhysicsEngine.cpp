#include "core/PhysicsEngine.h"
#include "environment/Object.h"
#include "environment/PhysicsData.h"

#include <iostream>

const float PhysicsEngine::deltaTime = (1.0f / 60.0f);

// Order of update is: Position Update -> Forces Update -> Collision Resolving
// Collisions is last since objects need to be moved and updated right up to colliding in order to resolve collisions with CCD
void PhysicsEngine::update() {
	float timeLeft = deltaTime;
	for (int i = 0; i < 5 && timeLeft > threshold; i++) { // For loop to run through the time left for sub time stepping, max of 5 substeps
		std::vector<HitData> nextHits = earliestCollision(timeLeft); // Stores all collisions happening on this frame / time left
		float hitTime = 1.0f;
		if (!nextHits.empty()) {
			hitTime = nextHits[0].timeToHit;
		}
		//std::cout << hitTime << std::endl;
		if (hitTime < 0.0f || timeLeft < hitTime || nextHits.empty()) { // If no collisions are happening, run through the entire timeLeft
			kinematicUpdater(timeLeft);
			positionUpdater(timeLeft);
			forcesUpdater(timeLeft);
			timeLeft = 0.0f;
		}
		else{
			kinematicUpdater(hitTime);
			positionUpdater(hitTime);
			forcesUpdater(hitTime);
			for (auto& hit : nextHits) { // Resolves collisions that are happening in the same frame
				resolveCollisions(hit);
			}		
			timeLeft -= hitTime;
		}	
	}
	if (timeLeft > threshold) { // Runs through the rest of the timeLeft if substeps couldn't go through it
		kinematicUpdater(timeLeft);
		positionUpdater(timeLeft);
		forcesUpdater(timeLeft);
	}
	forcesReset();
	//std::cout << "Energy: " << totalEnergy() << std::endl;
}

// Updates all kinematic objects
// Currently only for straight, point-to-point movements
void PhysicsEngine::kinematicUpdater(float timeLeft) {
	if (timeLeft <= 0.0f) return;
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || !object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f || object->physicsProperties->isStatic) continue;

		// Calculates how much to move
		float oldOffset = (std::sin(object->physicsProperties->accumulatedTime) * object->physicsProperties->moveDistance);
		object->physicsProperties->accumulatedTime += timeLeft * object->physicsProperties->moveSpeed;
		float newOffset = (std::sin(object->physicsProperties->accumulatedTime) * object->physicsProperties->moveDistance);

		object->physicsProperties->velocity = (object->position + (object->physicsProperties->moveDirection * (newOffset - oldOffset)) - object->position) / timeLeft;
	}	
}

// Returns a vector of HitData of when the earliest collisions happens at that specific earliest time
std::vector<HitData> PhysicsEngine::earliestCollision(float timeLeft) {
	std::vector<HitData> collisions;
	float earliestTime = 1.0f;
	if (timeLeft <= 0.0f) return collisions;

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
			if (hitData.timeToHit >= 0.0f && hitData.timeToHit <= earliestTime + threshold) {				
				if (hitData.timeToHit < earliestTime - threshold) {
					collisions.clear();
					earliestTime = hitData.timeToHit;
				}
				collisions.push_back(hitData);
			}
		}
	}
	return collisions;
}

// Updates position based on position equation
void PhysicsEngine::positionUpdater(float hitTime) {
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->mass <= 0.0f || object->physicsProperties->isStatic) continue;

		if (object->physicsProperties->enableGravity) {
			object->position += object->physicsProperties->velocity * hitTime + 0.5f * gravity * hitTime * hitTime;
		}
		else {
			object->position += object->physicsProperties->velocity * hitTime;
		}

		glm::vec3 angVel = object->physicsProperties->angularVelocity;
		float rotateAngle = glm::length(angVel) * hitTime;
		if (rotateAngle > 0.0f) {
			object->rotate(glm::normalize(angVel), glm::degrees(rotateAngle));
		}
		
		object->normal = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
		object->left = object->rotationQ * glm::vec3{ 1.0f, 0.0f, 0.0f };
		object->front = object->rotationQ * glm::vec3{ 0.0f, 0.0f, 1.0f };
		object->up = object->rotationQ * glm::vec3{ 0.0f, 1.0f, 0.0f };
	}
}

// Updates the objects based on the total forces acted on the object in the frame
void PhysicsEngine::forcesUpdater(float timeLeft) {
	if (timeLeft <= 0.0f) return;
	for (auto& object : physObjects) {
		if (!object || !object->physicsProperties || object->physicsProperties->isKinematic || object->physicsProperties->mass <= 0.0f || object->physicsProperties->isStatic) continue;
		glm::vec3 acceleration = object->physicsProperties->totalForces / object->physicsProperties->mass;
		if (object->physicsProperties->enableGravity) {
			acceleration += gravity;
		}		
		object->physicsProperties->velocity += acceleration * timeLeft;
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

void PhysicsEngine::planeCollision(HitData& hitData){ // Resolves plane and sphere collision
	Object* sphere = hitData.objectHitter;
	Object* plane = hitData.objectHit;

	hitData.relativeVelocity = sphere->physicsProperties->velocity - plane->physicsProperties->velocity;

	float combinedRestitution = (sphere->physicsProperties->restitution + plane->physicsProperties->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere->physicsProperties->restitution * plane->physicsProperties->restitution) / (sphere->physicsProperties->restitution + plane->physicsProperties->restitution);
	float normalSpeed = glm::dot(hitData.relativeVelocity, hitData.normal);

	if (normalSpeed >= 0.0f) {
		return;
	}

	float planePenetration = sphere->physicsProperties->radius - hitData.distance;
	if (planePenetration > threshold) { // Incase of floating point error, corrects it
		sphere->position += hitData.normal * planePenetration;
	}
	
	if (normalSpeed < -restingThreshold) { // Resolve collision
		sphere->physicsProperties->velocity -= (1.0f + combinedRestitution) * normalSpeed * hitData.normal;
	}
	else{ // Grounded state
		sphere->physicsProperties->velocity -= normalSpeed * hitData.normal;
	}
}

// Continuous Collision Detection (CCD)
// Currently assumes one object is an infinite mass plane and one is a sphere
// Implemented with both analytical and conservative advancement TOI solving
// Only uses analytical TOI solving since planes don't have acceleration currently
// Works for bounded planes
HitData PhysicsEngine::planeCollisionData(Object& sphere, Object& plane, float timeLeft) {
	HitData collision;
	collision.objectHit = &plane;
	collision.objectHitter = &sphere;

	float hitTime = 1.0f;

	glm::vec3 planeToSphere = sphere.position - plane.position;

	glm::vec3 sphereAcceleration = sphere.physicsProperties->totalForces / sphere.physicsProperties->mass;
	if (sphere.physicsProperties->enableGravity) {
		sphereAcceleration += gravity;
	}
	glm::vec3 planeAcceleration = plane.physicsProperties->totalForces / plane.physicsProperties->mass;
	if (plane.physicsProperties->enableGravity) {
		planeAcceleration += gravity;
	}

	glm::vec3 closestPointOnPlane = plane.position // Gets closes point on the plane to sphere bounded by the size of the plane
		+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
		+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);
	glm::vec3 planeToSphereClosest = sphere.position - closestPointOnPlane;
	float distToSphereClosest = glm::length(planeToSphereClosest);
	float distToSurface = distToSphereClosest - sphere.physicsProperties->radius;
	glm::vec3 dynamicNormal = (distToSphereClosest > threshold) ? glm::normalize(planeToSphereClosest) : plane.up; // dynamicNormal allows for plane edge bounces, follows sphere direction

	glm::vec3 relativeVelocity = sphere.physicsProperties->velocity - plane.physicsProperties->velocity;
	glm::vec3 relativeAcceleration = sphereAcceleration - planeAcceleration;

	if (distToSphereClosest - sphere.physicsProperties->radius <= 0.0f) { // Ensures infinite loop when contacting plane and moving away doesn't happen, important for sub stepping
		if (glm::dot(relativeVelocity, dynamicNormal) > -threshold) {
			return collision;
		}
		hitTime = 0.0f;
	}
	else {
		// When the accelerations are the same, it uses analytical time of impact solving
		// When different, it uses conservative advancement since analytical solver breaks for different relative accelerations
		float timeAccumulator = 0.0f; // Accumulator for conservative advancement, shows how long since start of advancing it has been, once the sphere collides, this becomes hitTime

		if (glm::length(relativeAcceleration) != 0.0f) { // This is conservative advancement
			while (timeAccumulator <= timeLeft) { // Keep doing advancements until timeAccumulator > timeLeft which means no collisions were detected in the path of the sphere
				glm::vec3 tempSpherePos = sphere.position + sphere.physicsProperties->velocity * timeAccumulator + 0.5f * sphereAcceleration * timeAccumulator * timeAccumulator;
				glm::vec3 tempPlanePos = plane.position + plane.physicsProperties->velocity * timeAccumulator + 0.5f * planeAcceleration * timeAccumulator * timeAccumulator;
				glm::vec3 tempSphereVel = sphere.physicsProperties->velocity + sphereAcceleration * timeAccumulator;
				glm::vec3 tempPlaneVel = plane.physicsProperties->velocity + planeAcceleration * timeAccumulator;

				planeToSphere = tempSpherePos - tempPlanePos; // Vector from plane center to sphere center
				closestPointOnPlane = tempPlanePos // Vector from the closest point on the plane to sphere, bounded by the plane dimensions, important for dynamic normal
					+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
					+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);
				planeToSphereClosest = tempSpherePos - closestPointOnPlane; // Vector from the above vector to the sphere
				distToSphereClosest = glm::length(planeToSphereClosest);

				if (distToSphereClosest - sphere.physicsProperties->radius <= 0.0f) { // If the sphere is touching plane, get hitTime from timeAccumulator
					hitTime = (timeAccumulator >= 0.0f && timeAccumulator < hitTime) ? timeAccumulator : 1.0f;
					break;
				}

				glm::vec3 tempRelativeVel = tempSphereVel - tempPlaneVel;
				float velocityMax = glm::length(tempRelativeVel) + glm::length(relativeAcceleration) * (timeLeft - timeAccumulator); // Theoretical max velocity 
				if (velocityMax < 0.0f) {
					break;
				}
				float safeTimeStep = (distToSphereClosest - sphere.physicsProperties->radius) / velocityMax; // Calculates the guaranteed safe timestep so no collisions occur
				if (safeTimeStep < 0.0f) { // Checks if sphere is colliding with plane
					hitTime = (timeAccumulator >= 0.0f && timeAccumulator < hitTime) ? timeAccumulator : 1.0f;
					break;
				}
				timeAccumulator += safeTimeStep; // If the sphere doesn't collide with plane, it goes onto the next safe timestep
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
	}

	// Everything below is calculating the variables for HitData based on the new solved collision time
	glm::vec3 sphereImpactVel = sphere.physicsProperties->velocity + sphereAcceleration * hitTime;
	glm::vec3 planeImpactVel = plane.physicsProperties->velocity + planeAcceleration * hitTime;

	glm::vec3 newSpherePos = sphere.position + sphere.physicsProperties->velocity * hitTime + 0.5f * sphereAcceleration * hitTime * hitTime; // Move to impact position to write new hit results into hitData
	planeToSphere = newSpherePos - plane.position;
	closestPointOnPlane = plane.position
		+ plane.left * glm::clamp(glm::dot(planeToSphere, plane.left), -plane.physicsProperties->planeWidth / 2, plane.physicsProperties->planeWidth / 2)
		+ plane.front * glm::clamp(glm::dot(planeToSphere, plane.front), -plane.physicsProperties->planeHeight / 2, plane.physicsProperties->planeHeight / 2);

	planeToSphereClosest = newSpherePos - closestPointOnPlane;
	distToSphereClosest = glm::length(planeToSphereClosest);
	distToSurface = distToSphereClosest - sphere.physicsProperties->radius;
	dynamicNormal = (distToSphereClosest > threshold) ? glm::normalize(planeToSphereClosest) : plane.up;
	relativeVelocity = sphereImpactVel - planeImpactVel;

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

	hitData.relativeVelocity = sphere2->physicsProperties->velocity - sphere1->physicsProperties->velocity;

	float combinedRestitution = (sphere1->physicsProperties->restitution + sphere2->physicsProperties->restitution == 0.0f) ? 0.0f : // Uses harmonic combination to get a combined restitution value
		(2 * sphere1->physicsProperties->restitution * sphere2->physicsProperties->restitution) / (sphere1->physicsProperties->restitution + sphere2->physicsProperties->restitution);

	float normalSpeed = glm::dot(hitData.relativeVelocity, hitData.normal);
	float collisionPenetration = hitData.distance - sphere1->physicsProperties->radius - sphere2->physicsProperties->radius;

	float sphere1InverseMass = (sphere1->physicsProperties->isStatic) ? 0.0f : 1.0f / sphere1->physicsProperties->mass;
	float sphere2InverseMass = (sphere2->physicsProperties->isStatic) ? 0.0f : 1.0f / sphere2->physicsProperties->mass;
	float totalInverseMass = sphere1InverseMass + sphere2InverseMass;

	if (collisionPenetration < -threshold) { // Incase of floating point error, corrects it
		sphere1->position += hitData.normal * collisionPenetration * (sphere1InverseMass / totalInverseMass);
		sphere2->position -= hitData.normal * collisionPenetration * (sphere2InverseMass / totalInverseMass);
	}

	if (normalSpeed >= 0.0f) { return; }

	if (normalSpeed < -restingThreshold) { // If moving faster towards each other faster than resting threshold
		float impulse = -(1 + combinedRestitution) * normalSpeed / totalInverseMass;
		sphere1->physicsProperties->velocity -= impulse * sphere1InverseMass * hitData.normal;
		sphere2->physicsProperties->velocity += impulse * sphere2InverseMass * hitData.normal;
	}
	else { // Resting state
		float impulse = -normalSpeed / totalInverseMass; // Acts as perfectly inelastic collision to prevent jittering
		sphere1->physicsProperties->velocity -= impulse * sphere1InverseMass * hitData.normal; // Zero velocity towards sphere2
		sphere2->physicsProperties->velocity += impulse * sphere2InverseMass * hitData.normal; // Zero velocity towards sphere1
	}
}

HitData PhysicsEngine::sphereCollisionData(Object& sphere1, Object& sphere2, float timeLeft) { // Assumes always same acceleration for both spheres
	HitData collision;
	float hitTime = 1.0f;

	glm::vec3 sphere1ToSphere2 = sphere2.position - sphere1.position;
	glm::vec3 sphere1Normal = glm::normalize(sphere1ToSphere2);

	glm::vec3 sphere1Acceleration = sphere1.physicsProperties->totalForces / sphere1.physicsProperties->mass;
	if (sphere1.physicsProperties->enableGravity) {
		sphere1Acceleration += gravity;
	}
	glm::vec3 sphere2Acceleration = sphere2.physicsProperties->totalForces / sphere2.physicsProperties->mass;
	if (sphere2.physicsProperties->enableGravity) {
		sphere2Acceleration += gravity;
	}

	glm::vec3 relativeVelocity = sphere2.physicsProperties->velocity - sphere1.physicsProperties->velocity;
	glm::vec3 relativeAcceleration = sphere2Acceleration - sphere1Acceleration; // Not used currently as its assuming same gravity for both spheres

	float distance = glm::length(sphere1ToSphere2);
	float surfaceDistance = distance - sphere1.physicsProperties->radius - sphere2.physicsProperties->radius;
	float closingNormVel = glm::dot(relativeVelocity, sphere1Normal);
	float closingNormAccel = glm::dot(relativeAcceleration, sphere1Normal);

	if (surfaceDistance <= 0.0f) { // Ensures infinite loop when contacting plane and moving away doesn't happen, important for sub stepping
		if (closingNormVel > -threshold) {
			return collision;
		}
		hitTime = 0.0f;
	}
	else {
		float a = glm::dot(relativeVelocity, relativeVelocity);
		float b = 2.0f * glm::dot(sphere1ToSphere2, relativeVelocity);
		float c = glm::dot(sphere1ToSphere2, sphere1ToSphere2) - glm::pow(sphere1.physicsProperties->radius + sphere2.physicsProperties->radius, 2.0f);

		if (a == 0.0f) { return collision; }

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant >= 0.0f) {
			float hitTimeTemp = (-b - glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
			float hitTimeTempPlus = (-b + glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
			if (hitTimeTemp >= -threshold && hitTimeTemp < hitTime) {
				hitTime = std::max(0.0f, hitTimeTemp);
			}
		}
	}
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

// Resolves the collision after velocity and position is updated to exact impact time in update function above
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

float PhysicsEngine::getPhysicsRate(){
	return PhysicsEngine::deltaTime;
}