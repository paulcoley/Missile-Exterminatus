#pragma once 

#include "BaseApplication.h"
#include "Missile.h"
#include <fstream>
#include <sstream>
#include <map>

class GameApplication : public BaseApplication {
	private:
		Missile* missile; // store a pointer to the character
		std::list<Missile*> MissileList; // Lecture 5: now a list of Missiles
	public:
		GameApplication(void);
		virtual ~GameApplication(void);

		void loadEnv();			// Load the buildings or ground plane, etc.
		void setupEnv();		// Set up the lights, shadows, etc

		void addTime(Ogre::Real deltaTime);		// update the game state

		//////////////////////////////////////////////////////////////////////////
		// Lecture 4: keyboard interaction
		// moved from base application
		// OIS::KeyListener
		bool keyPressed( const OIS::KeyEvent &arg );
		bool keyReleased( const OIS::KeyEvent &arg );
		// OIS::MouseListener
		bool mouseMoved( const OIS::MouseEvent &arg );
		bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		////////////////////////////////////////////////////////////////////////////

	protected:
		virtual void createScene(void);
};