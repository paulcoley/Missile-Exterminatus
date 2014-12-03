#pragma once
#include "Grid.h"
#include <deque>
#include "PowerUp.h"

class Player {
private:
    
    Ogre::SceneManager* mSceneMgr;      // pointer to scene graph
    Ogre::SceneNode* mBodyNode;         
    Ogre::Entity* mBodyEntity;
    int timesHit;
    
    std::list<PowerUp> currentPowerUps;
    Ogre::Real fire_speed;
    
    //Positioning and size data
    float height;                       // height the character should be moved up
    float scale;                        // scale of character from original model

    // for locomotion
    Ogre::Vector3 mDirection;               // The direction the object is moving
    Ogre::Real speed;                       // The speed at which the object is moving
    Ogre::Real baseSpeed;
	bool shotActive;
	Ogre::Real shotTime;
public:
    Player(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position);
    ~Player();
    
	bool dead;
    void setPosition(Ogre::Vector3 new_position);
    Ogre::Vector3 getPosition();
	void setRotation(Ogre::Quaternion rotation);
	Ogre::Quaternion getRotation();
    int getTimesHit();
	void setTimesHit(int val);
	void setHeight(float val);
	void setSpeed(float val);
	void setFireSpeed(float val);
	void moveForward(Ogre::Real deltaTime);
	void moveBackward(Ogre::Real deltaTime);
	void strafeLeft(Ogre::Real deltaTime);
	void strafeRight(Ogre::Real deltaTime);
	void shoot();
	Ogre::SceneNode* getSceneNode();
	Ogre::AxisAlignedBox getBoundBox();

	void update(Ogre::Real deltaTime);      // update the Player
    void AddPowerUp(PowerUp);
};

