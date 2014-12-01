#pragma once 

#include "BaseApplication.h"
#include "Missile.h"
#include "Player.h"
#include "PowerUpAgent.h"
#include <fstream>
#include <sstream>
#include <map>
#include <lua.hpp>

class Missile;

class GameApplication : public BaseApplication {
	private:
		std::list<Missile*> MissileList; //List of all active missiles

		//MissileSpawner Scaling
		Ogre::Real timePassed;
		int spawnThreshold;
		int increaseThreshold;
		int missilesPerSpawn;
		//grid dimensions
		int numRows;
		int numColumns;

		lua_State* l;
	public:
		GameApplication(void);
		virtual ~GameApplication(void);

		void loadEnv();			// Load the buildings or ground plane, etc.
		void setupEnv();		// Set up the lights, shadows, etc

		void addTime(Ogre::Real deltaTime);		// update the game state

		//////////////////////////////////////////////////////////////////////////
		// OIS::KeyListener
		bool keyPressed( const OIS::KeyEvent &arg );
		bool keyReleased( const OIS::KeyEvent &arg );
		// OIS::MouseListener
		bool mouseMoved( const OIS::MouseEvent &arg );
		bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		////////////////////////////////////////////////////////////////////////////

		std::list<Missile*> getMissiles();

	protected:
		virtual void createScene(void);
};