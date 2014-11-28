#include "Missile.h"

Missile::Missile(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position) {
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this Missile will be

	if (mSceneMgr == NULL) {
		std::cout << "ERROR: No valid scene manager in Missile constructor" << std::endl;
		return;
	}

	this->height = height;
	this->scale = scale;

	mBodyNode[0] = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity[0] = mSceneMgr->createEntity(name, "fish.mesh"); // load the model
	mBodyNode[0]->attachObject(mBodyEntity[0]);	// attach the model to the scene node

	mBodyNode[0]->translate(spawn_position); // make the Ogre stand on the plane (almost)
	mBodyNode[0]->scale(scale,scale,scale); // Scale the figure

	setupAnimations();  // load the animation for this character

	// configure walking parameters
	speed = 35.0f;	
	this->baseSpeed = speed;
	mDirection = Ogre::Vector3::ZERO;
	//this->mBodyNode[0]->setAutoTracking(true, mSceneMgr->getSceneNode("Player"), Ogre::Vector3(0,0,1), Ogre::Vector3(0,4,0));
	//this->mBodyNode[1]->setAutoTracking(true, mSceneMgr->getSceneNode("Player"), Ogre::Vector3(0,0,1), Ogre::Vector3(0,4,0));
}

Missile::~Missile(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

void Missile::setPosition(Ogre::Vector3 spawn_position)
{
	this->mBodyNode[0]->setPosition(spawn_position);
}

// update is called at every frame from GameApplication::addTime
void Missile::update(Ogre::Real deltaTime) 
{
	
	this->updateAnimations(deltaTime);	// Update animation playback
	

	//Ogre::Real move = speed * deltaTime;
	//mDistance -= move;


	//bounding box check pending player instantiation
	/*
	if (this->mBodyEntity[0]->getBoundingBox().intersects(playerBox) || this->mBodyEntity[1]->getBoundingBox().intersects(playerBox))
	{
		if(!(this->fishOn))
		{
			player->setTimesHit(player->getTimesHit() + 1);
		}
		this->explode();
	}
	if(this->fishOn)
	{
		this->mBodyNode[1]->setVisible(false);
		this->mBodyNode[0]->setVisible(true);
	}
	else
	{
		this->mBodyNode[1]->setVisible(true);
		this->mBodyNode[0]->setVisible(false);
	}
	this->mBodyNode[0]->translate(this->mBodyNode[0]->getOrientation() * Ogre::Vector3::UNIT_Z * this->speed * deltaTime);
	this->mBodyNode[1]->translate(this->mBodyNode[1]->getOrientation() * Ogre::Vector3::UNIT_Z * this->speed * deltaTime);
	*/
}


void Missile::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning

	// this is very important due to the nature of the exported animations
	mBodyEntity[0]->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] = {"swim"};

	// populate our animation list
	for (int i = 0; i < 1; i++)
	{
		mAnims[i] = mBodyEntity[0]->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_SWIM);
	setTopAnimation(ANIM_SWIM);
}

void Missile::setBaseAnimation(AnimID id, bool reset)
{
    if (mBaseAnimID >= 0 && mBaseAnimID < 1)
    {
        // if we have an old animation, fade it out
        mFadingIn[mBaseAnimID] = false;
        mFadingOut[mBaseAnimID] = true;
    }

    mBaseAnimID = id; 

    if (id != ANIM_NONE)
    {
        // if we have a new animation, enable it and fade it in
        mAnims[id]->setEnabled(true);
        mAnims[id]->setWeight(0);
        mFadingOut[id] = false;
        mFadingIn[id] = true;
        if (reset) mAnims[id]->setTimePosition(0);
    }
}
	
void Missile::setTopAnimation(AnimID id, bool reset)
{
    if (mTopAnimID >= 0 && mTopAnimID < 1)
    {
        // if we have an old animation, fade it out
        mFadingIn[mTopAnimID] = false;
        mFadingOut[mTopAnimID] = true;
    }

    mTopAnimID = id;

    if (id != ANIM_NONE)
    {
        // if we have a new animation, enable it and fade it in
        mAnims[id]->setEnabled(true);
        mAnims[id]->setWeight(0);
        mFadingOut[id] = false;
        mFadingIn[id] = true;
        if (reset) mAnims[id]->setTimePosition(0);
    }
}

void Missile::updateAnimations(Ogre::Real deltaTime)
{
    using namespace Ogre;

    Real baseAnimSpeed = 1;
    Real topAnimSpeed = 1;

    mTimer += deltaTime; // how much time has passed since the last update

    if (mTimer >= mAnims[mTopAnimID]->getLength())
        {
            setTopAnimation(ANIM_SWIM, true);
            setBaseAnimation(ANIM_SWIM, true);
            mTimer = 0;
        }
    
    // increment the current base and top animation times
    if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
    if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

    // apply smooth transitioning between our animations
    fadeAnimations(deltaTime);
}

void Missile::fadeAnimations(Ogre::Real deltaTime)
{
    using namespace Ogre;

    for (int i = 0; i < 1; i++)
    {
        if (mFadingIn[i])
        {
            // slowly fade this animation in until it has full weight
            Real newWeight = mAnims[i]->getWeight() + deltaTime * 7.5f; //ANIM_FADE_SPEED;
            mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
            if (newWeight >= 1) mFadingIn[i] = false;
        }
        else if (mFadingOut[i])
        {
            // slowly fade this animation out until it has no weight, and then disable it
            Real newWeight = mAnims[i]->getWeight() - deltaTime * 7.5f; //ANIM_FADE_SPEED;
            mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
            if (newWeight <= 0)
            {
                mAnims[i]->setEnabled(false);
                mFadingOut[i] = false;
            }
        }
    }
}

void Missile::setTracking(Ogre::SceneNode* target)
{
	mDestination = target->getPosition();
	mDirection = mDestination - mBodyNode[0]->getPosition();
	mDistance = mDirection.normalise();
	mBodyNode[0]->setAutoTracking(true, target, Ogre::Vector3::NEGATIVE_UNIT_X);
}