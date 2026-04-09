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
