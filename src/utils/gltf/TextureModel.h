#pragma once

#include "Common.h"

namespace Core
{
	class TextureModel
	{
	public:
		TextureModel();
		~TextureModel();

		static GLuint LoadTextureModel(const std::string&, bool flip = true);
		static GLuint LoadCubeMapModel(const std::vector <std::string>&);
	};
}
