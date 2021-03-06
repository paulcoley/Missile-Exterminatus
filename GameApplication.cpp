#include "GameApplication.h"
#include "Grid.h"

namespace {
	Player* player;
	Grid* grid;
	Missile* missile;
	std::vector<PowerUp> powerUps;

	int setupPlayer(lua_State* L) {
		int argc = lua_gettop(L);
		if(argc == 0 || player == NULL) { return 0; }
		player->setSpeed(lua_tonumber(L, 1));
		player->setFireSpeed(lua_tonumber(L, 2));
		player->setHeight(lua_tonumber(L, 3));
		return 3;
	}

	int setPowerUps(lua_State* L) {
		int argc = lua_gettop(L);
		if(argc == 0) { return 0; }
		PowerUp power;
		power.speedModifier = lua_tonumber(L, 1);
		power.fire_speed_modifier = lua_tonumber(L, 2);
		power.deliver_the_fish = (bool) lua_toboolean(L, 3);
		powerUps.push_back(power);
		return 3;
	}
}

//-------------------------------------------------------------------------------------
GameApplication::GameApplication(void) {
	missile = NULL; // Init member data
	grid = NULL;
	player = NULL;
	b = NULL;

	//MissileSpawner Scaling initialization
	timePassed = 0.f;
	spawnThreshold = 5;
	increaseThreshold = 10;
	missilesPerSpawn = 1;

	l = luaL_newstate();
	luaL_openlibs(l);
	lua_register(l, "setupPlayer", setupPlayer);
	lua_register(l, "setPowerUps", setPowerUps);
	srand(NULL);
	myManualObject = NULL;
}
//-------------------------------------------------------------------------------------
GameApplication::~GameApplication(void) {
	if (missile != NULL)  // clean up memory
		delete missile; 
	if (grid != NULL) //Clean up grid
		delete grid;

	lua_close(l);
}

//-------------------------------------------------------------------------------------
void GameApplication::createScene(void) {
    loadEnv();
	setupEnv();
	myManualObject =  mSceneMgr->createManualObject("manual1"); 
	Ogre::SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node");

	Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("manual1Material","General"); 
	myManualObjectMaterial->setReceiveShadows(false); 
	myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 
	Ogre::Vector3 player_position = Ogre::Vector3(player->getPosition().x, player->getPosition().y + 3.f, player->getPosition().z);
	myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST); 
	myManualObject->position(player_position); 
	myManualObject->position(player_position + player->getRotation() * Ogre::Vector3::UNIT_Z * 1000.f);
	myManualObject->end(); 
	myManualObjectNode->attachObject(myManualObject);
}

void GameApplication::createGUI(void)
{
	//////////////////////////////////////////////////////////////////////////////////
	// Lecture 16
	if (mTrayMgr == NULL) return;
	using namespace OgreBites;


	Ogre::StringVector items;
	this->timePassed = 0.f;
	this->score = 0;
	items.push_back("Time passed");
	items.push_back("Score");
	mParamsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_TOP,"SamplePanel",250,items);
	
	//mTrayMgr->create

	mParamsPanel->show();

	//////////////////////////////////////////////////////////////////////////////////
}

std::string getNewName() { // return a unique name 
	static int count = 0;	// keep counting the number of objects

	std::string s;
	std::stringstream out;	// a stream for outputing to a string
	out << count++;			// make the current count into a string
	s = out.str();

	return "object_" + s;	// append the current count onto the string
}

void GameApplication::loadEnv() {
	using namespace Ogre;	// use both namespaces
	using namespace std;

	class readEntity { // need a structure for holding entities
		public:
			string filename;
			float y;
			float scale;
			float orient;
			bool Missile;
	};

	ifstream inputfile;		// Holds a pointer into the file

	string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= "main_level.txt"; //if txt file is in the same directory as cpp file
	inputfile.open(path);

	//inputfile.open("D:/CS425-2012/Lecture 8/GameEngine-loadLevel/level001.txt"); // bad explicit path!!!
	if (!inputfile.is_open()) { // oops. there was a problem opening the file
		cout << "ERROR, FILE COULD NOT BE OPENED" << std::endl;	// Hmm. No output?
		return;
	}

	// the file is open
	int x,z;
	inputfile >> x >> z;	// read in the dimensions of the grid
	string matName;
	inputfile >> matName;	// read in the material name

	// create floor mesh using the dimension read
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Plane(Vector3::UNIT_Y, 0), x*NODESIZE, z*NODESIZE, x, z, true, 1, x, z, Vector3::UNIT_Z);
	
	//create a floor entity, give it material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName(matName);
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	grid = new Grid(mSceneMgr, z, x); // Set up the grid. z is rows, x is columns
	numColumns = x;
	numRows = z;

	string buf;
	inputfile >> buf;	// Start looking for the Objects section
	while  (buf != "Objects")
		inputfile >> buf;
	if (buf != "Objects") {	// Oops, the file must not be formated correctly
		cout << "ERROR: Level file error" << endl;
		return;
	}

	// read in the objects
	readEntity *rent = new readEntity();	// hold info for one object
	std::map<string,readEntity*> objs;		// hold all object and Missile types;
	while (!inputfile.eof() && buf != "Characters") // read through until you find the Characters section
	{ 
		inputfile >> buf;			// read in the char
		if (buf != "Characters")
		{
			inputfile >> rent->filename >> rent->y >> rent->orient >> rent->scale;  // read the rest of the line
			rent->Missile = false;		// these are objects
			objs[buf] = rent;			// store this object in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}

	while  (buf != "Characters")	// get through any junk
		inputfile >> buf;
	
	// Read in the characters
	while (!inputfile.eof() && buf != "World") // Read through until the world section
	{
		inputfile >> buf;		// read in the char
		if (buf != "World")
		{
			inputfile >> rent->filename >> rent->y >> rent->scale; // read the rest of the line
			rent->Missile = true;			// this is an Missile
			objs[buf] = rent;			// store the Missile in the map
			rent = new readEntity();	// create a new instance to store the next object
		}
	}
	delete rent; // we didn't need the last one

	// read through the placement map
	char c;
	for (int i = 0; i < z; i++)			// down (row)
		for (int j = 0; j < x; j++)		// across (column)
		{
			inputfile >> c;			// read one char at a time
			buf = c + '\0';			// convert char to string
			rent = objs[buf];		// find cooresponding object or Missile
			if (rent != NULL)		// it might not be an Missile or object
				if (rent->Missile)	// if it is an Missile...
				{
					// Use subclasses instead!
					missile = new Missile(this->mSceneMgr, getNewName(), rent->filename, rent->y, rent->scale, Ogre::Vector3(grid->getPosition(i,j).x, rent->y, grid->getPosition(i,j).z), this);
					MissileList.push_back(missile);
				}
				else	// Load objects
				{
					grid->loadObject(getNewName(), rent->filename, i, rent->y, j, rent->scale);
				}
			else // not an object or Missile
			{
				if (c == 'w') // create a wall
				{
					Entity* ent = mSceneMgr->createEntity(getNewName(), Ogre::SceneManager::PT_CUBE);
					ent->setMaterialName("Examples/RustySteel");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ent);
					mNode->scale(0.1f,0.2f,0.1f); // cube is 100 x 100
					grid->getNode(i,j)->setOccupied();  // indicate that Missiles can't pass through
					mNode->setPosition(grid->getPosition(i,j).x, 10.0f, grid->getPosition(i,j).z);
				}
				else if (c == 'e')
				{
					ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
					ParticleSystem* ps = mSceneMgr->createParticleSystem(getNewName(), "Examples/PurpleFountain");
					Ogre::SceneNode* mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					mNode->attachObject(ps);
					mNode->setPosition(grid->getPosition(i,j).x, 0.0f, grid->getPosition(i,j).z);
				}
				else if (c == 'p') {
					player = new Player(this->mSceneMgr, getNewName(), "sinbad.mesh", 0, 1, Ogre::Vector3(grid->getPosition(i,j).x, 1, grid->getPosition(i,j).z));
					path = __FILE__; //gets the current cpp file's path with the cpp file
					path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
					path+= "scripts\\player.lua"; //if txt file is in the same directory as cpp file
					luaL_dofile(l, path.c_str());
				}
				else if (c == 's') {
					SpawnerList.push_back(new MissileSpawner(this->mSceneMgr, getNewName(), 0.f, .5f, Ogre::Vector3(grid->getPosition(i,j).x, 1, grid->getPosition(i,j).z), this));
				}
			}
		}
	
	// delete all of the readEntities in the objs map
	rent = objs["s"]; // just so we can see what is going on in memory (delete this later)
	
	std::map<string,readEntity*>::iterator it;
	for (it = objs.begin(); it != objs.end(); it++) // iterate through the map
	{
		delete (*it).second; // delete each readEntity
	}
	objs.clear(); // calls their destructors if there are any. (not good enough)
	
	inputfile.close();
	grid->printToFile(); // see what the initial grid looks like.

	path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0,1+path.find_last_of('\\')); //removes filename to leave path
	path+= "scripts\\powerups.lua"; //if txt file is in the same directory as cpp file
	luaL_dofile(l, path.c_str());

	powerSphere = new PowerUpAgent(player, this, mSceneMgr, "PowerSphere", "sphere.mesh", 5.f, 0.1f, Ogre::Vector3::ZERO);
}

void GameApplication::setupEnv()
{
	using namespace Ogre;

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// disable default camera control so the character can do its own 
	mCameraMan->setStyle(OgreBites::CS_FREELOOK); // CS_FREELOOK, CS_ORBIT, CS_MANUAL

	// use small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 40, 20);
	light->setSpecularColour(ColourValue::White);

	//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8); // Lecture 4
}

void GameApplication::addTime(Ogre::Real deltaTime)
{
	player->update(deltaTime);
	if(player->dead == true) {
		if(b == NULL) { b = mTrayMgr->createButton(OgreBites::TL_CENTER, "GameOver", "Game Over man; Game Over!"); }
		b->show();
		mTrayMgr->showCursor();
		return;
	}

	timePassed += deltaTime;
	Ogre::Vector3 player_position = Ogre::Vector3(player->getPosition().x, player->getPosition().y + 3.f, player->getPosition().z);
	Ogre::Ray death_beam(player_position, player_position + player->getRotation() * Ogre::Vector3::UNIT_Z * 1000.f);
	myManualObject->beginUpdate(0);
	myManualObject->position(player_position); 
	myManualObject->position(player_position + player->getRotation() * Ogre::Vector3::UNIT_Z * 1000.f);
	myManualObject->end(); 
	for(Missile*& projectile : MissileList) {
		if (projectile != NULL) {
			projectile->setTracking(player->getSceneNode());
			projectile->update(deltaTime);
		}
		if(projectile != NULL && projectile->getBoundBox().intersects(player->getBoundBox()))
		{
			player->setTimesHit(player->getTimesHit() + 1);
			projectile->explode();
			delete projectile;
			projectile = NULL;
		}
		if(projectile != NULL && player->getShotActive())
		{
			auto detect = death_beam.intersects(projectile->getBoundBox());
			if(detect.first) {
				std::cout << "Here!" << std::endl;
				score++;
				projectile->explode();
				delete projectile;
				projectile = NULL;
			}
		}
	}

	//bounding box checks
	if(player->getBoundBox().intersects(powerSphere->getBoundBox()) && powerSphere->getVisibility() == true) {
		player->AddPowerUp(powerSphere->getBase());
		powerSphere->despawn();
	}

	//update scaling values
	if(timePassed >= increaseThreshold) {
		increaseThreshold += 10;
		missilesPerSpawn++;
		if(powerSphere->getVisibility() == false) {
			powerSphere->spawn( powerUps[rand() % powerUps.size()], grid->getPosition(rand() % numRows, rand() % numColumns));
		}
	}
	if(timePassed >= spawnThreshold) {
		spawnThreshold += 5;
		for(MissileSpawner*& spawner : SpawnerList) {
			Ogre::Vector3 position = spawner->getPosition();
			position.x  = position.x + static_cast<float>(rand() % 30) - 15.f;
			position.z  = position.z + static_cast<float>(rand() % 30) - 15.f;
			position.y = 5.f;
			if (spawner != NULL) {
				for(int i = 0; i < missilesPerSpawn; i++) {
					if(MissileList.size() < MAXIMUM_FISH) { MissileList.push_back(new Missile(this->mSceneMgr, getNewName(), "missile.mesh", 1.f, 1.f, position, this)); }
				}
			}
		}
	}

	if (mKeyboard->isKeyDown(OIS::KC_W)) {
		player->moveForward(deltaTime);
	}
	if (mKeyboard->isKeyDown(OIS::KC_A)) {
		player->strafeLeft(deltaTime);
	}
	if (mKeyboard->isKeyDown(OIS::KC_S)) {
		player->moveBackward(deltaTime);
	}
	if (mKeyboard->isKeyDown(OIS::KC_D)) {
		player->strafeRight(deltaTime);
	}

	player->setRotation(mCamera->getOrientation());
	Ogre::Vector3 camPos = player->getPosition() + player->getRotation() * Ogre::Vector3::UNIT_Z * -15.f + player->getRotation() * Ogre::Vector3::UNIT_Y * 5.f;
	Ogre::Ray ray(player->getPosition(), camPos);
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0.f);
	auto values = ray.intersects(plane);
	if(values.first == true) {
		camPos = player->getPosition() + player->getRotation() * Ogre::Vector3::UNIT_Z * -values.second + player->getRotation() * Ogre::Vector3::UNIT_Y * 5.f;
	}
	mCamera->setPosition(camPos);

	mParamsPanel->setParamValue(0, Ogre::StringConverter::toString(this->timePassed));
	mParamsPanel->setParamValue(1, Ogre::StringConverter::toString(this->score));

	MissileList.remove(NULL);
	if(player->getTimesHit() >= 10)
	{
		player->dead = true;
	}
}

bool GameApplication::keyPressed( const OIS::KeyEvent &arg ) // Moved from BaseApplication
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
	else if(arg.key == OIS::KC_SPACE) { //Shoots using a ray to detect hits.
		if(!player->getShotActive()) {
			player->shoot();
		}
	}
    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
   
    mCameraMan->injectKeyDown(arg);
    return true;
}

bool GameApplication::keyReleased( const OIS::KeyEvent &arg )
{
    mCameraMan->injectKeyUp(arg);
    return true;
}

bool GameApplication::mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;
    mCameraMan->injectMouseMove(arg);
    return true;
}

bool GameApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool GameApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    return true;
}

////////////////////////////////////////////////////////////////////
// Lecture 16
// Callback method for buttons
void GameApplication::buttonHit(OgreBites::Button* b)
{
	if (b->getName() == "GameOver")
	{
		exit(0);
	}
}

std::list<Missile*> GameApplication::getMissiles() {
	return MissileList;
}

