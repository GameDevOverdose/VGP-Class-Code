#include "CmdSetColor.h"

#include "Rasterizer.h"
#include "VariableCache.h"

bool CmdSetColor::Execute(const std::vector<std::string>& params)
{
	if (params.size() < 3)
	{
		return false;
	}

	VariableCache* vc = VariableCache::Get();
	
	const float r = vc->GetFloat(params[0]);
	const float g = vc->GetFloat(params[1]);
	const float b = vc->GetFloat(params[2]);

	Rasterizer::Get()->SetColor(X::Color(r, g, b, 1.0f));
	return true;
}