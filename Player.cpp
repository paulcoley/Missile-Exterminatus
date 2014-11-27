#include "Player.h"

Player::Player(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position) {
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this Player will be

	if (mSceneMgr == NULL) {
		std::cout << "ERROR: No valid scene manager in Player constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;

	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, "sinbad.mesh"); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node

	mBodyNode->translate(spawn_position); // make the Ogre stand on the plane (almost)
	mBodyNode->scale(scale,scale,scale); // Scale the figure

	// configure internal parameters
	timesHit = 0;
	dead = false;
	fire_speed = 35.0f;
	speed = 35.0f;	
	mDirection = Ogre::Vector3::ZERO;
}

Player::~Player() {
}

void Player::setPosition(Ogre::Vector3 new_position) {
	this->mBodyNode->setPosition(new_position);
}

Ogre::Vector3 Player::getPosition() {
	return this->mBodyNode->getPosition();
}

void Player::setRotation(Ogre::Quaternion rotation) {
	this->mBodyNode->setOrientation(rotation);
}

Ogre::Quaternion Player::getRotation() {
	return this->mBodyNode->getOrientation();
}
    
void Player::update(Ogre::Real deltaTime, OIS::Keyboard *input) {      // update the Player
	for(PowerUp* power : this->currentPowerUps)
	{
		power->timeActive += deltaTime;
		if(power->timeActive >= 10)
		{
			/*
			check which type it is via checking its fields and undo modifications corresponding to type then pop from list
			*/
		}
	}
	if(input->isKeyDown(OIS::KC_W))
	{
		this->mBodyNode->translate(this->mBodyNode->getOrientation() * Ogre::Vector3(0, 0, this->speed * deltaTime));
	}
	else if(input->isKeyDown(OIS::KC_S))
	{
		this->mBodyNode->translate(this->mBodyNode->getOrientation() * Ogre::Vector3(0, 0, -(this->speed * deltaTime)));
	}
	else if(input->isKeyDown(OIS::KC_A))
	{
		this->mBodyNode->translate(this->mBodyNode->getOrientation() * Ogre::Vector3((this->speed * deltaTime), 0, 0));
	}
	else if(input->isKeyDown(OIS::KC_D))
	{
		this->mBodyNode->translate(this->mBodyNode->getOrientation() * Ogre::Vector3(-(this->speed * deltaTime), 0, 0));
	}
}

void Player::AddPowerUp(PowerUp* power) {
	this->currentPowerUps.push_back(power);
}

int Player::getTimesHit()
{
	return this->timesHit;
}

void Player::setTimesHit(int val)
{
	this->timesHit = val;
}