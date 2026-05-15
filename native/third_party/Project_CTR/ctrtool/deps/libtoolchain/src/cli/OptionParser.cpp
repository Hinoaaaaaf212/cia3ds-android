#include <tc/cli/OptionParser.h>

#include <fmt/core.h>

const std::string tc::cli::OptionParser::kClassName = "tc::cli::OptionParser";

tc::cli::OptionParser::OptionParser() :
	mOptionaAliasMap(),
	mUnkOptHandler(nullptr)
{
}

void tc::cli::OptionParser::registerOptionHandler(const std::shared_ptr<IOptionHandler>& handler)
{
	if (handler == nullptr)
	{
		// throw exception
		throw tc::ArgumentNullException(kClassName, "OptionHandler was null.");
	}

	if (handler->getOptionStrings().empty() && handler->getOptionRegexPatterns().empty())
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "OptionHandler had no option strings or regex patterns.");
	}

	for (auto itr = handler->getOptionStrings().begin(); itr != handler->getOptionStrings().end(); itr++)
	{
		mOptionaAliasMap.insert(std::pair<std::string, std::shared_ptr<IOptionHandler>>(*itr, handler));
	}

	for (auto itr = handler->getOptionRegexPatterns().begin(); itr != handler->getOptionRegexPatterns().end(); itr++)
	{
		mOptionRegexList.push_back(std::pair<std::regex, std::shared_ptr<IOptionHandler>>(std::regex(*itr), handler));
	}
}

void tc::cli::OptionParser::registerUnrecognisedOptionHandler(const std::shared_ptr<IOptionHandler>& handler)
{
	if (handler == nullptr)
	{
		// throw exception
		throw tc::ArgumentNullException(kClassName, "OptionHandler was null.");
	}

	mUnkOptHandler = handler;
}

void tc::cli::OptionParser::processOptions(const std::vector<std::string>& args)
{
	/*
	fmt::print("OptionParser\n");
	fmt::print("  args:\n");
	for (auto itr = args.begin(); itr != args.end(); itr++)
	{
		fmt::print("  #{:s}#\n", *itr);
	}
	*/

	//fmt::print("Begin parsing options\n");

	std::string opt = std::string();
	std::vector<std::string> params = std::vector<std::string>();
	for (auto itr = args.begin(); itr != args.end(); itr++)
	{
		//fmt::print("itr={:s}\n", *itr);

		// (1) parse the current string
		std::string tmp_opt = std::string();
		std::string tmp_param = std::string();
		bool is_compound = false;

		// if the string begins with '-' then it is an option (which may be compound)
		if (itr->compare(0,1,"-") == 0)
		{
			//fmt::print("looks like an option\n");

			// if there is an "=" in this, then this is a compound option & paramter
			size_t equalsign_pos = itr->find('=');
			if (equalsign_pos == std::string::npos)
			{
				//fmt::print("the option looks like a solo option\n");
				tmp_opt = *itr;				
			}
			else
			{
				//fmt::print("the option looks like a compound opt=param\n");
				tmp_opt = itr->substr(0, equalsign_pos);
				tmp_param = itr->substr(equalsign_pos + 1, std::string::npos);
				//fmt::print(" > opt :   {:s}\n", tmp_opt);
				//fmt::print(" > param : {:s}\n", tmp_param);
				// --path=here
				// 0123456789a
				// --path : pos=0, size = 6 = equalsign_pos
				// here : pos = 7 = eqialsign_pos + 1, size = 4 = 11 - 7 = itr->length() -(equalsign_pos+1)
				is_compound = true;
			}
			
		}
		// otherwise it is a param
		else
		{
			tmp_param = *itr;
		}

		// (2) interprete it in the context of the current state

		// the user has indicated the end of the current option
		// hand off to option handler and clear state
		if (opt.empty() == false && tmp_opt.empty() == false)
		{
			handleOption(opt, params);

			opt = std::string();
			params = std::vector<std::string>();
		}

		// if tmp_opt isn't empty then make it the option
		if (tmp_opt.empty() == false)
		{
			opt = tmp_opt;
		}

		// if tmp_param isn't empty then add it to the param list
		if (tmp_param.empty() == false)
		{
			// if there is no option set, then this is a head-less parameter, throw exception
			if (opt.empty() == true)
			{
				throw tc::ArgumentException(kClassName, "Option parameter was provided without an option.");
			}
			params.push_back(tmp_param);
		}

		// compound options only accept one parameter
		if (is_compound)
		{
			handleOption(opt, params);

			opt = std::string();
			params = std::vector<std::string>();
		}
	}

	// process dangling opt/params
	if (opt.empty() == false)
	{
		handleOption(opt, params);
	}
}

void tc::cli::OptionParser::processOptions(const std::vector<std::string>& args, size_t pos, size_t num)
{
	processOptions({args.begin()+pos, args.begin()+pos+num});
}

void tc::cli::OptionParser::handleOption(const std::string& opt, const std::vector<std::string>& params)
{
	// attempt to locate a literal alias
	auto aliasItr = mOptionaAliasMap.find(opt);
	if (aliasItr != mOptionaAliasMap.end())
	{
		aliasItr->second->processOption(opt, params);
		return;
	}

	// attempt to pattern match
	for (auto regexItr = mOptionRegexList.begin(); regexItr != mOptionRegexList.end(); regexItr++)
	{
		if (std::regex_match(opt, regexItr->first))
		{
			regexItr->second->processOption(opt, params);
			return;
		}
	}

	// attempt to use unknown option handler 
	if (mUnkOptHandler != nullptr)
	{
		mUnkOptHandler->processOption(opt, params);
		return;
	}
	
	// if no handler is located, throw exception
	throw tc::ArgumentException(kClassName, fmt::format("Option \"{}\" is not recognised.", opt));
}