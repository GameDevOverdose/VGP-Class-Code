//====================================================================================================
// Filename:	Config.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Config.h"

#pragma warning(push)
#pragma warning(disable : 4996)
#include <RapidJSON/Inc/document.h>
#include <RapidJSON/Inc/filereadstream.h>
#include <RapidJSON/Inc/filewritestream.h>
#include <RapidJSON/Inc/prettywriter.h>
#pragma warning(pop)

using namespace X;

namespace
{
	Config* sConfig = nullptr;
}

//----------------------------------------------------------------------------------------------------

class Config::Impl
{
public:
	void Load(const char* fileName);
	void Save();
	void SaveAs(const char* fileName);

	int GetInt(const char* key, int defaultValue = 0) const;
	bool GetBool(const char* key, bool defaultValue = false) const;
	float GetFloat(const char* key, float defaultValue = 0.0f) const;
	const char* GetString(const char* key, const char* defaultValue = "") const;

	void SetInt(const char* key, int value);
	void SetBool(const char* key, bool value);
	void SetFloat(const char* key, float value);
	void SetString(const char* key, const char* value);

private:
	rapidjson::Document mDocument;
	std::string mFileName;
};

//----------------------------------------------------------------------------------------------------

void Config::Impl::Load(const char* fileName)
{
	if (fileName == nullptr)
	{
		XLOG("[Config] Not using config file. Default settings will be used.");
		return;
	}

	FILE* file = nullptr;
#ifdef _WIN32
	fopen_s(&file, fileName, "rb");
#else
	file = fopen(fileName, "rb");
#endif
	if (file == nullptr)
	{
		XLOG("[Config] Failed to open config file %s. Default settings will be used.", fileName);
		return;
	}

	char buffer[65536];
	rapidjson::FileReadStream readStream(file, buffer, std::size(buffer));
	mDocument.ParseStream(readStream);

	fclose(file);

	mFileName = fileName;
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::Save()
{
	SaveAs(mFileName.c_str());
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::SaveAs(const char* fileName)
{
	FILE* file = nullptr;
#ifdef _WIN32
	fopen_s(&file, fileName, "wb");
#else
	file = fopen(fileName, "wb");
#endif
	if (file == nullptr)
	{
		XLOG("[Config] Failed to save config file %s.", fileName);
		return;
	}

	char buffer[65536];
	rapidjson::FileWriteStream writeStream(file, buffer, std::size(buffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(writeStream);
	mDocument.Accept(writer);

	fclose(file);
}

//----------------------------------------------------------------------------------------------------

int Config::Impl::GetInt(const char* key, int defaultValue) const
{
	if (!mDocument.IsObject())
	{
		return defaultValue;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsInt())
	{
		return iter->value.GetInt();
	}
	return defaultValue;
}

//----------------------------------------------------------------------------------------------------

bool Config::Impl::GetBool(const char* key, bool defaultValue) const
{
	if (!mDocument.IsObject())
	{
		return defaultValue;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsBool())
	{
		return iter->value.GetBool();
	}
	return defaultValue;
}

//----------------------------------------------------------------------------------------------------

float Config::Impl::GetFloat(const char* key, float defaultValue) const
{
	if (!mDocument.IsObject())
	{
		return defaultValue;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsFloat())
	{
		return iter->value.GetFloat();
	}
	return defaultValue;
}

//----------------------------------------------------------------------------------------------------

const char* Config::Impl::GetString(const char* key, const char* defaultValue) const
{
	if (!mDocument.IsObject())
	{
		return defaultValue;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsString())
	{
		return iter->value.GetString();
	}
	return defaultValue;
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::SetInt(const char* key, int value)
{
	if (!mDocument.IsObject())
	{
		mDocument.StartObject();
	}

	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd())
	{
		XASSERT(iter->value.IsInt(), "[Config] %s is not an integer.", key);
		iter->value.SetInt(value);
	}
	else
	{
		rapidjson::Value k(key, mDocument.GetAllocator());
		mDocument.AddMember(k, value, mDocument.GetAllocator());
	}
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::SetBool(const char* key, bool value)
{
	if (!mDocument.IsObject())
	{
		mDocument.StartObject();
	}

	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd())
	{
		XASSERT(iter->value.IsBool(), "[Config] %s is not a boolean.", key);
		iter->value.SetBool(value);
	}
	else
	{
		rapidjson::Value k(key, mDocument.GetAllocator());
		mDocument.AddMember(k, value, mDocument.GetAllocator());
	}
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::SetFloat(const char* key, float value)
{
	if (!mDocument.IsObject())
	{
		mDocument.StartObject();
	}

	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd())
	{
		XASSERT(iter->value.IsFloat(), "[Config] %s is not a float.", key);
		iter->value.SetFloat(value);
	}
	else
	{
		rapidjson::Value k(key, mDocument.GetAllocator());
		mDocument.AddMember(k, value, mDocument.GetAllocator());
	}
}

//----------------------------------------------------------------------------------------------------

void Config::Impl::SetString(const char* key, const char* value)
{
	if (!mDocument.IsObject())
	{
		mDocument.StartObject();
	}

	rapidjson::Value v(value, mDocument.GetAllocator());

	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd())
	{
		XASSERT(iter->value.IsString(), "[Config] %s is not a boolean.", key);
		iter->value.SetString(value, (rapidjson::SizeType)std::strlen(value));
	}
	else
	{
		rapidjson::Value k(key, mDocument.GetAllocator());
		mDocument.AddMember(k, v, mDocument.GetAllocator());
	}
}

//----------------------------------------------------------------------------------------------------

void Config::StaticInitialize(const char* fileName)
{
	XASSERT(sConfig == nullptr, "[Config] System already initialized!");
	sConfig = new Config();
	sConfig->Load(fileName);
}

//----------------------------------------------------------------------------------------------------

void Config::StaticTerminate()
{
	if (sConfig != nullptr)
	{
		SafeDelete(sConfig);
	}
}

//----------------------------------------------------------------------------------------------------

Config* Config::Get()
{
	XASSERT(sConfig != nullptr, "[Config] No system registered.");
	return sConfig;
}

//----------------------------------------------------------------------------------------------------

Config::Config()
	: mImpl(std::make_unique<Impl>())
{
}

//----------------------------------------------------------------------------------------------------

void Config::Load(const char* fileName)
{
	mImpl->Load(fileName);
}

//----------------------------------------------------------------------------------------------------

void Config::Save()
{
	mImpl->Save();
}

//----------------------------------------------------------------------------------------------------

void Config::SaveAs(const char* fileName)
{
	mImpl->SaveAs(fileName);
}

//----------------------------------------------------------------------------------------------------

int Config::GetInt(const char* key, int defaultValue) const
{
	return mImpl->GetInt(key, defaultValue);
}

//----------------------------------------------------------------------------------------------------

bool Config::GetBool(const char* key, bool defaultValue) const
{
	return mImpl->GetBool(key, defaultValue);
}

//----------------------------------------------------------------------------------------------------

float Config::GetFloat(const char* key, float defaultValue) const
{
	return mImpl->GetFloat(key, defaultValue);
}

//----------------------------------------------------------------------------------------------------

const char* Config::GetString(const char* key, const char* defaultValue) const
{
	return mImpl->GetString(key, defaultValue);
}

//----------------------------------------------------------------------------------------------------

void Config::SetInt(const char* key, int value)
{
	mImpl->SetInt(key, value);
}

//----------------------------------------------------------------------------------------------------

void Config::SetBool(const char* key, bool value)
{
	mImpl->SetBool(key, value);
}

//----------------------------------------------------------------------------------------------------

void Config::SetFloat(const char* key, float value)
{
	mImpl->SetFloat(key, value);
}

//----------------------------------------------------------------------------------------------------
void Config::SetString(const char* key, const char* value)
{
	mImpl->SetString(key, value);
}
