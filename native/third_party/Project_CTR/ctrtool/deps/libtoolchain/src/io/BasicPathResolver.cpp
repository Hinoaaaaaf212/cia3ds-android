#include <tc/io/BasicPathResolver.h>

const std::string tc::io::BasicPathResolver::kClassName = "tc::io::BasicPathResolver";

tc::io::BasicPathResolver::BasicPathResolver() :
	BasicPathResolver(tc::io::Path("/"), {})
{}

tc::io::BasicPathResolver::BasicPathResolver(const tc::io::Path& current_directory_path) :
	BasicPathResolver(current_directory_path, {})
{}

tc::io::BasicPathResolver::BasicPathResolver(const tc::io::Path& current_directory_path, const std::vector<std::string>& root_names) :
	mCurrentDirPath(),
	mExplicitRootLabels(root_names)
{
	setCurrentDirectory(current_directory_path);
}


void tc::io::BasicPathResolver::setCurrentDirectory(const tc::io::Path& path)
{
	if (path.empty())
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "path was empty.");
	}

	mCurrentDirPath = path;
}

const tc::io::Path& tc::io::BasicPathResolver::getCurrentDirectory() const
{
	return mCurrentDirPath;
}

void tc::io::BasicPathResolver::setExplicitRootLabels(const std::vector<std::string>& root_labels)
{
	mExplicitRootLabels = root_labels;
}

const std::vector<std::string>& tc::io::BasicPathResolver::getExplicitRootLabels() const
{
	return mExplicitRootLabels;
}

void tc::io::BasicPathResolver::resolveCanonicalPath(const tc::io::Path& path, tc::io::Path& canonical_path) const
{
	canonical_path = resolveCanonicalPath(path);
}

tc::io::Path tc::io::BasicPathResolver::resolveCanonicalPath(const tc::io::Path& path) const
{
	// create output path
	tc::io::Path canonical_path;

	// get iterator for input path
	auto path_itr = path.begin();
	
	// if the begining of the path exists and is a valid root label, then the input path is an absolute (but not necessarily canonical) path
	if (path_itr != path.end() && (std::find(mExplicitRootLabels.begin(), mExplicitRootLabels.end(), *path_itr) != mExplicitRootLabels.end() || *path_itr == mCurrentDirPath.front()))
	{
		// the beginning of canonical_path is the path root name
		canonical_path = tc::io::Path(*path_itr + "/");

		// increment path iterator
		path_itr++;
	}
	else
	{
		// the beginning of the canonical_path is the current directory path
		canonical_path = mCurrentDirPath;
	}

	// process relative elements of path, combining with the base canonical_path
	for (; path_itr != path.end(); path_itr++)
	{
        // ignore "current directory" alias
		if (*path_itr == ".")
			continue;
        // ignore empty path elements
        else if (*path_itr == "")
            continue;
        // navigate up for "parent directory" alias
		else if (*path_itr == "..")
		{
			// ".." is the parent directory, so if there are path elements then we remove from the back to "go to the parent directory"
			if (canonical_path.size() > 1)
				canonical_path.pop_back();
			else
				continue;
		}
		else
			canonical_path.push_back(*path_itr);
	}

	return canonical_path;
}