#pragma once
#include "Grid.h"
#include <deque>

class Missile {
private:
    
    Ogre::SceneManager* mSceneMgr;      // pointer to scene graph
    Ogre::SceneNode* mBodyNode[2];         
    Ogre::Entity* mBodyEntity[2];
    
    //Animation block
    enum AnimID
    {
        ANIM_SWIM,
        ANIM_NONE
    };
    Ogre::AnimationState* mAnims[2];        // master animation list
    AnimID mBaseAnimID;                     // current base (full- or lower-body) animation
    AnimID mTopAnimID;                      // current top (upper-body) animation
    bool mFadingIn[1];                      // which animations are fading in
    bool mFadingOut[1];                     // which animations are fading out
    Ogre::Real mTimer;                      // general timer to see how long animations have been playing
    void setupAnimations();                 // load this character's animations
    void fadeAnimations(Ogre::Real deltaTime);              // blend from one animation to another
    void updateAnimations(Ogre::Real deltaTime);            // update the animation frame
    void setBaseAnimation(AnimID id, bool reset = false);   // choose animation to display
    void setTopAnimation(AnimID id, bool reset = false);
    
    //State change block
    bool fishOn;                          //has the fish powerup been grabbed
    int secSinceFish;                   //tracks the time since powerup has been active
    
    //Positioning and size data
    float height;                       // height the character should be moved up
    float scale;                        // scale of character from original model

    // for locomotion
    Ogre::Real mDistance;                   // The distance the Missile has left to travel
    Ogre::Vector3 mDirection;               // The direction the object is moving
    Ogre::Vector3 mDestination;             // The destination the object is moving towards
    Ogre::Real speed;                       // The speed at which the object is moving
	Ogre::Real baseSpeed;
    //Update methods/checks
    void explode();
public:
    Missile(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, Ogre::Vector3 spawn_position);
    ~Missile();
    
    void setPosition(Ogre::Vector3 new_position);
    Ogre::Vector3 getPosition();
	void setTracking(Ogre::SceneNode* target);
    void update(Ogre::Real deltaTime);      // update the Missile

};