#include "PowerUpAgent.h"

PowerUpAgent::PowerUpAgent(Player* player, GameApplication* game, Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position) {
using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this Player will be

	if (mSceneMgr == NULL) {
		std::cout << "ERROR: No valid scene manager in PowerUpAgent constructor" << std::endl;
		return;
	}

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, "sphere.mesh");//Sphere(spawn_position, 2)); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node

	mBodyNode->translate(spawn_position); // make the Ogre stand on the plane (almost)
	mBodyNode->scale(scale,scale,scale); // Scale the figure
	base = new PowerUp();
	death = false;
}

void PowerUpAgent::update() {
	//bounding box check pending player instantiation
	/*
	if (this->mBodyEntity->getBoundingBox().intersects(playerBox))
	{
		player->addPowerUp(base);
	}
	*/
}