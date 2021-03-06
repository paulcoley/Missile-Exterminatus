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
	mBodyEntity = mSceneMgr->createEntity("player", "sinbad.mesh"); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node

	mBodyNode->translate(spawn_position); // make the Ogre stand on the plane (almost)
	mBodyNode->scale(scale,scale,scale); // Scale the figure

	// configure internal parameters
	timesHit = 0;
	dead = false;
	fire_speed = 35.0f;
	speed = 35.0f;
	baseSpeed = speed;
	mDirection = Ogre::Vector3::ZERO;
	shotActive = false;
	shotTime = 0.f;

	ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
	ps = mSceneMgr->createParticleSystem("Fountain1", "Examples/PurpleFountain");
	Ogre::SceneNode* mnode = mBodyNode->createChildSceneNode();
	mnode->roll(Degree(90));
	mnode->pitch(Degree(90));
	mnode->attachObject(ps);
	ps->setVisible(false);
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
	this->mBodyNode->yaw(Ogre::Degree(180));
}

Ogre::Quaternion Player::getRotation() {
	return this->mBodyNode->getOrientation();
}
    
void Player::update(Ogre::Real deltaTime) {      // update the Player
	if(this->shotActive)
	{
		this->shotTime += deltaTime;
		if(this->shotTime > 1.f)
		{
			this->shotActive = false;
			this->ps->setVisible(false);
			shotTime = 0.f;
		}
	}
	for(PowerUp power : this->currentPowerUps)
	{
		power.timeActive += deltaTime;
		speed = baseSpeed * power.speedModifier;
		fire_speed = baseSpeed * power.fire_speed_modifier;
		if(power.timeActive >= 10)
		{
			/*
			check which type it is via checking its fields and undo modifications corresponding to type then pop from list
			*/
			if(power.speedModifier > 1)
				speed = baseSpeed;
			else if(power.fire_speed_modifier > 1)
				fire_speed = baseSpeed;
			currentPowerUps.pop_front();
		}
	}
}

void Player::AddPowerUp(PowerUp power) {
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

void Player::setHeight(float val) {
	height = val;
	mBodyNode->setPosition(mBodyNode->getPosition().x, height, mBodyNode->getPosition().z);
}

void Player::setSpeed(float val) {
	baseSpeed = val;
	speed = val;
}

void Player::setFireSpeed(float val) {
	fire_speed = val;
}

void Player::moveForward(Ogre::Real deltaTime) {
	mBodyNode->translate(mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z * speed * deltaTime);
	mBodyNode->setPosition(mBodyNode->getPosition().x, height, mBodyNode->getPosition().z);
}

void Player::moveBackward(Ogre::Real deltaTime) {
	mBodyNode->translate(mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z * -speed * deltaTime);
	mBodyNode->setPosition(mBodyNode->getPosition().x, height, mBodyNode->getPosition().z);
}

void Player::strafeLeft(Ogre::Real deltaTime) {
	mBodyNode->translate(mBodyNode->getOrientation() * Ogre::Vector3::UNIT_X * speed * deltaTime);
	mBodyNode->setPosition(mBodyNode->getPosition().x, height, mBodyNode->getPosition().z);
}

void Player::strafeRight(Ogre::Real deltaTime) {
	mBodyNode->translate(mBodyNode->getOrientation() * Ogre::Vector3::UNIT_X * -speed * deltaTime);
	mBodyNode->setPosition(mBodyNode->getPosition().x, height, mBodyNode->getPosition().z);
}

Ogre::SceneNode* Player::getSceneNode()
{
	return this->mBodyNode;
}

Ogre::AxisAlignedBox Player::getBoundBox()
{
	return this->mBodyEntity->getWorldBoundingBox();
}

void Player::shoot()
{
	this->shotActive = true;
	ps->setVisible(true);
}

bool Player::getShotActive() {
	return shotActive;
}