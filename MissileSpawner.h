#pragma once
#include "GameApplication.h"
#include <deque>

class GameApplication;

class MissileSpawner {
private:
	Ogre::SceneManager* mSceneMgr;      // pointer to scene graph
    Ogre::SceneNode* mBodyNode;
    Ogre::Entity* mBodyEntity;
	GameApplication* owner;
	
    float height;                       // height the character should be moved up
    float scale;                        // scale of character from original model
public:
	MissileSpawner(Ogre::SceneManager* SceneManager, std::string name, float height, float scale, Ogre::Vector3 spawn_position, GameApplication* instance);
    ~MissileSpawner();
	
    void setPosition(Ogre::Vector3 new_position);
    Ogre::Vector3 getPosition();
	void setTracking(Ogre::SceneNode* target);
    void update(Ogre::Real deltaTime);      // update the Missile
};