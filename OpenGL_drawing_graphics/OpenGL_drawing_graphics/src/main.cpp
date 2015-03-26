#include <iostream>
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Tutorial1.h"
#include "Tutorial2.h"
#include "Tutorial3.h"
#include "Tutorial4.h"
#include "Tutorial5.h"
#include "Tutorial6.h"
#include "Tutorial7.h"
#include "Tutorial8.h"
#include "Tutorial9.h"
#include "ParticleSystem.h"
#include "TextureLoader.h"
#include "MyObjLoader.h"
#include "MobileCamera.h"

using namespace std;

int main()
{
	vector<float> outVerts;

	Tutorial9* app = new Tutorial9();
	//ParticleSystem* particleSystem = new ParticleSystem();

	app->Run();


	/*Current Tutorials:
	6 = Shadows
	7 = Animation
	9 = Deffered Rendering
	10 = procedural generation
	Re-iterate through texture with independant loader
	*/
}