#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <direct.h>
#include <string>
#include <cstdlib>

class FileSystem
{
private:
	typedef std::string(*Builder) (const std::string& path);

public:
	static std::string getPath(const std::string& path)
	{
		static std::string(*pathBuilder)(std::string const&) = getPathBuilder();
		return (*pathBuilder)(path);
	}

private:
	static std::string const& getRoot()
	{
		std::string path = std::string(_getcwd(NULL, 0));
		path = path.substr(0, path.find_last_of('\\'));
		for (char& c : path)
			if (c == '\\')
				c = '/';
		static char const* envRoot = getenv("LOGL_ROOT_PATH");
		static char const* givenRoot = (envRoot != nullptr ? envRoot : path.c_str());
		static std::string root = (givenRoot != nullptr ? givenRoot : "");
		return root;
	}

	//static std::string(*foo (std::string const &)) getPathBuilder()
	static Builder getPathBuilder()
	{
		if (getRoot() != "")
			return &FileSystem::getPathRelativeRoot;
		else
			return &FileSystem::getPathRelativeBinary;
	}

	static std::string getPathRelativeRoot(const std::string& path)
	{
		return getRoot() + std::string("/") + path;
	}

	static std::string getPathRelativeBinary(const std::string& path)
	{
		return "../../../" + path;
	}
};

// FILESYSTEM_H
#endif
