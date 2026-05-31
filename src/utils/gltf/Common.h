#pragma once

#include <Windows.h>
#include <stdio.h>	
#include <stdlib.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>

#include <GL/glew.h>
#include <GL/wglew.h> 		
#include <gl/GL.h>

#include "../../includes/vmath.h"

#include "../common.h"
#pragma push_macro("lerp")
#undef lerp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#pragma pop_macro("lerp")
extern vmath::mat4 perspectiveProjectionMatrix;

namespace Core
{
	enum ATTRIBUTE
	{
		POSITION = 0,
		COLOR,
		NORMAL,
		TEXCOORD,
		TANGENT,
		BITANGENT
	};
}
