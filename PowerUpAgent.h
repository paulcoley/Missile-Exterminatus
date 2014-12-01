#pragma once
#include "GameApplication.h"
#include "Player.h"
#include <deque>

class GameApplication;

class PowerUpAgent {
private:
    Ogre::SceneManager* mSceneMgr;      // pointer to scene graph
    Ogre::SceneNode* mBodyNode;         
    Ogre::Entity* mBodyEntity;
    
    PowerUp* base;
    bool death;
    
public:
    PowerUpAgent(Player* player, GameApplication* game, Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position);
    void spawn(int type, Ogre::Vector3 location);
	void despawn();
	bool getVisibility();
	PowerUp* getBase();
	Ogre::AxisAlignedBox getBoundBox();
	void update();
};