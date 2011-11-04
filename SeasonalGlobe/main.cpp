#include <iostream>
using namespace std;

#include "MyApp.h"

static MyApp seasonalGlobe;

/*
For shader class, inherit from "glex" and call glex::Load() in the constructor
- Need to do for everything using glex, VBOs, shaders etc
*/