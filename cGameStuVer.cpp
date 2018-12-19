/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	SDL_Init(SDL_INIT_GAMECONTROLLER); //initialization for the Game Controller API
	SDL_GameController *controller = nullptr; //pointer
		//
	for (int i = 0; i < SDL_NumJoysticks(); i++) // checks if there are controllers
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			break;
		}
	}
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theScore = 0;

	// Store the textures
	textureName = { "asteroid1", "asteroid2", "asteroid3", "asteroid4", "photon","theRocket","theBackground", "explosion" };
	texturesToUse = { "Images\\Sprites\\asteroid1.png", "Images\\Sprites\\asteroid2.png", "Images\\Sprites\\asteroid3.png", "Images\\Sprites\\asteroid4.png", "Images\\Sprites\\Photon64x32.png", "Images\\Sprites\\rocketSprite.png", "Images\\Bkg\\starscape1024x768.png", "Images\\Sprites\\explosion.png" }; //changed the textures to set the medieval theme
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "scribish" }; // changed the font to fit the theme
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/scribish.ttf" }; // changed the font to fit the theme
	for (int fonts = 0; fonts < (int)fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Adam Al-Bsoul", "Score : ", "Controls:", "Rotate Cannon . Arrow Keys or Dpad", "Fire . SPACEBAR or A" }; // changed the text to say my name and to state the instructions
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();
	//	SDL_Texture* createTextTexture(SDL_Renderer* theRenderer, LPCSTR text, textType txtType, SDL_Color txtColour, SDL_Color txtBkgd);

	theTextureMgr->addTexture("Title", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, gameTextList[0], textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // changed the font to fit the theme; changed the font colour from green to white so that it can be easily read since the background is green
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // changed the font to fit the theme
	theTextureMgr->addTexture("theControls1", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, gameTextList[2], textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // added text for controls display
	theTextureMgr->addTexture("theControls2", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, gameTextList[3], textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // added text for rotation controls
	theTextureMgr->addTexture("theControls3", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, gameTextList[4], textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // added text for firing controls

	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { soundType::music, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theRocket.setSpritePos({ 350, 500 }); // position altered to place the player position in the bottom centre of the window
	theRocket.setTexture(theTextureMgr->getTexture("theRocket"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight());
	//theRocket.setRocketVelocity(100); //commented out because the player doesn't move
	theRocket.setSpriteTranslation({ 50,50 });

	// Create vector array of textures



}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each asteroid in the vector array
	for (int draw = 0; draw < (int)theAsteroids.size(); draw++)
	{
		theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
	}
	// Render each bullet in the vector array
	for (int draw = 0; draw < (int)theBullets.size(); draw++)
	{
		theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
	}
	// Render each explosion in the vector array
	for (int draw = 0; draw < (int)theExplosions.size(); draw++)
	{
		theExplosions[draw]->render(theRenderer, &theExplosions[draw]->getSourceRect(), &theExplosions[draw]->getSpritePos(), theExplosions[draw]->getSpriteScale());
	}
	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	// Render updated score value

	if (updateScore) {
		//if statement that checks the boolean updateScore value
		theTextureMgr->deleteTexture("theScore"); //deletes old score text
		theTextureMgr->addTexture("theScore", theFontMgr->getFont("scribish")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 255, 255, 255, 255 }, { 255, 255, 255, 255 })); // updates the score with the new value by getting the variable value, font, rendering and setting the type and colour of the text
		updateScore = false; //when the if statement is fullfiled, this bool is set to false so that the if value stops looping
	}
	tempTextTexture = theTextureMgr->getTexture("theScore");
	pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	// Render the control text
	tempTextTexture = theTextureMgr->getTexture("theControls1");
	pos = { 50, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	tempTextTexture = theTextureMgr->getTexture("theControls2");
	pos = { 10, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	tempTextTexture = theTextureMgr->getTexture("theControls3");
	pos = { 10, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	// Lab 7 code goes here

	// render the rocket
	theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{/*  // bounce the asteroids off the screen edges
	if (theAsteroid.getSpritePos().x <= 0 || theAsteroid.getSpritePos().x > (WINDOW_WIDTH - theAsteroid.getSpriteDimensions().w))
	{
		theAsteroid.setSpritePos({ theAsteroid.getSpritePos().x, theAsteroid.getSpritePos().y });
		theAsteroid.setAsteroidVelocity(theAsteroid.getAsteroidVelocity()*(-10));
	}
	else if (theAsteroid.getSpritePos().y <= 0 || theAsteroid.getSpritePos().y > (WINDOW_HEIGHT - theAsteroid.getSpriteDimensions().h))
	{
		theAsteroid.setSpritePos({ theAsteroid.getSpritePos().x, theAsteroid.getSpritePos().y });
		theAsteroid.setAsteroidVelocity(theAsteroid.getAsteroidVelocity()*(-10));
	}
	*/

	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
		}
	}
	// Update the visibility and position of each bullet
	vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	while (bulletIterartor != theBullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = theBullets.erase(bulletIterartor);
		}
		else
		{
			(*bulletIterartor)->update(deltaTime);
			++bulletIterartor;
		}
	}
	// Update the visibility and position of each explosion
	vector<cSprite*>::iterator expIterartor = theExplosions.begin();
	while (expIterartor != theExplosions.end())
	{
		if ((*expIterartor)->isActive() == false)
		{
			expIterartor = theExplosions.erase(expIterartor);
		}
		else
		{
			(*expIterartor)->animate(deltaTime);
			++expIterartor;
		}
	}

	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	{
		//(*bulletIterartor)->update(deltaTime);
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*asteroidIterator)->setActive(false);
				(*bulletIterartor)->setActive(false);
				theExplosions.push_back(new cSprite);
				int index = theExplosions.size() - 1;
				theExplosions[index]->setSpriteTranslation({ 0, 0 });
				theExplosions[index]->setActive(true);
				theExplosions[index]->setNoFrames(5); // change it to fit the skull death animation which has less frames than the original one
				theExplosions[index]->setTexture(theTextureMgr->getTexture("explosion"));
				theExplosions[index]->setSpriteDimensions(theTextureMgr->getTexture("explosion")->getTWidth() / theExplosions[index]->getNoFrames(), theTextureMgr->getTexture("explosion")->getTHeight());
				theExplosions[index]->setSpritePos({ (*asteroidIterator)->getSpritePos().x + (int)((*asteroidIterator)->getSpritePos().w / 2), (*asteroidIterator)->getSpritePos().y + (int)((*asteroidIterator)->getSpritePos().h / 2) });

				theSoundMgr->getSnd("explosion")->play(0);

				//increments the score when the enemy gets hit and destroyed with the bullet
				theScore += 10;
				strScore = gameTextList[1];
				strScore += to_string(theScore).c_str();
				updateScore = true; //bool is true when the score has to be updated

				// Lab 7 code goes here

			}
		}
	}


	// Update the Rockets position
	theRocket.update(deltaTime);
	if (frames % 360 == 0)
	{
		int currentSize = theAsteroids.size();
		int end = currentSize + 5;
		int count = 1;
		for (int astro = currentSize; astro < end; astro++)
		{
			theAsteroids.push_back(new cAsteroid);
			theAsteroids[astro]->setSpritePos({ (150 * count + (50 * (count - 1))), -75 });// changed the values to better fit the enemies
			theAsteroids[astro]->setSpriteTranslation({ 0, -100 }); //changed the values to change the direction of the enemies
			int randAsteroid = AsteroidTextDis(gen);
			theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
			theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
			theAsteroids[astro]->setAsteroidVelocity(100);
			theAsteroids[astro]->setActive(true);
			count++;
		}
	}
	frames++;
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDL_CONTROLLER_BUTTON_START:
			case SDLK_ESCAPE:
				theLoop = false;
				break;
				/*case SDLK_DOWN:
				{
					theRocket.setRocketMove(1);
				}
				break;

				case SDLK_UP:
				{
					theRocket.setRocketMove(-1);
				}
				break;*/
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			case SDLK_RIGHT:
			{
				theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() + 10); //changed from 5 to 10 so that the aiming can be done quicker
			}
			break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			case SDLK_LEFT:
			{
				theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() - 10); //changed from 5 to 10 so that the aiming can be done quicker
			}
			break;
			case SDL_CONTROLLER_BUTTON_A:
			case SDLK_SPACE:
			{
				theBullets.push_back(new cBullet);
				int numBullets = theBullets.size() - 1;
				theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x + 50, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
				theBullets[numBullets]->setSpriteTranslation({ 900, 30 }); //changed the first value from 50 to 900 so that the bullet spawns in the direction of where the player is pointing at; changed the second value from 50 to 30 because a cannon ball should move slower
				theBullets[numBullets]->setTexture(theTextureMgr->getTexture("photon"));
				theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("photon")->getTWidth(), theTextureMgr->getTexture("photon")->getTHeight());
				theBullets[numBullets]->setBulletVelocity(50);
				theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle() - 1800); // subtract 1800 to make it spawn up
				theBullets[numBullets]->setActive(true);
				cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
			}
			theSoundMgr->getSnd("shot")->play(0);
			break;
			default:
				break;
			}

		default:
			break;
		}

	}

	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	//Quit FONT system
	TTF_Quit();

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}