#include <iostream>
using namespace std;

#include "SeasonalWindow.h"
class MyApp : public App { private: SeasonalWindow w; }; // boot GXBase
static MyApp seasonalGlobe;

/*
For shader class, inherit from "glex" and call glex::Load() in the constructor
- Need to do for everything using glex, VBOs, shaders etc
*/