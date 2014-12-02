#include "MissileSpawner.h"

MissileSpawner::MissileSpawner(Ogre::SceneManager* SceneManager, std::string name, float height, float scale, Ogre::Vector3 spawn_position, GameApplication* instance) {
	using namespace Ogre;

	mSceneMgr = SceneManager;

	if (mSceneMgr == NULL) {
		std::cout << "ERROR: No valid scene manager in MissileSpawner constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mBodyEntity = mSceneMgr->createEntity(name, "fish.mesh");
	mBodyNode->attachObject(mBodyEntity);

	mBodyNode->translate(spawn_position);
	mBodyNode->scale(scale,scale,scale);
	owner = instance;
}

MissileSpawner::~MissileSpawner() {}

void MissileSpawner::setPosition(Ogre::Vector3 new_position) {
	mBodyNode->setPosition(new_position);
}

Ogre::Vector3 MissileSpawner::getPosition() {
	return mBodyNode->getPosition();
}

void MissileSpawner::setTracking(Ogre::SceneNode* target) {
	mBodyNode->setAutoTracking(true, target, Ogre::Vector3::NEGATIVE_UNIT_X);
}

void MissileSpawner::update(Ogre::Real deltaTime) {
}