#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <Ogre.h>
#include <OgreApplicationContext.h>

class MainApplication : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
    MainApplication();
    ~MainApplication();

    bool keyPressed(const OgreBites::KeyboardEvent &evt);

    void setup();

protected:
};

#endif
