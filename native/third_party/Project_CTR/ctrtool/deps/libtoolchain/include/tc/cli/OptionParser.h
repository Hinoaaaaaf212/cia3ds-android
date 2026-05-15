	/**
	 * @file OptionParser.h
	 * @brief Declaration of tc::cli::OptionParser
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2022/01/22
	 **/
#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include <regex>
#include <memory>
#include <tc/ArgumentException.h>
#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace cli {

	/**
	 * @class OptionParser
	 * @brief Class for parsing command-line options.
	 * 
	 * This class processes command-line options according to user defined implementations of @ref tc::cli::OptionParser::IOptionHandler that are registered with this class.
	 * 
	 * The format of command-line arguments varies by system and convention. This class supports the following styles of command-line options:
	 * * @b --opt : Option name prefixed by "--" with no parameters,
	 * * @b --opt=var : Option name prefixed by "--" with only one parameter delimited by "=",
	 * * @b --opt @b var1 @b var2 : Option name prefixed by "--" with one or more parameters delimited by white space,
	 * * @b -opt : Option name prefixed by "-" with no parameters,
	 * * @b -opt=var : Option name prefixed by "-" with one parameter delimited by "=",
	 * * @b -opt @b var1 @b var2 : Option name prefixed by "-" with one or more parameters delimited by white space,
	 * 
	 * When parsing options from command-line arguments, it will (in order of occurence) collect the option name and the parameters that follow in 
	 * accordance to the above rules, and defer to the user defined @ref tc::cli::OptionParser::IOptionHandler for this option. If none is defined 
	 * it will throw a @ref tc::ArgumentException, or alternatively defer to a user defined unknown option handler (also a @ref tc::cli::OptionParser::IOptionHandler).
	 * 
	 * For example, say we have some state struct like this:
	 * @code
	 * struct UserOpt
	 * {
	 * 	std::string sku_code;
	 * 	std::map<std::string, std::string> environment_vars;
	 * };
	 * @endcode 
	 * 
	 * And the command-line is intended be formatted as follows:
	 * @code
	 * someprogram -sku <your SKU code here> -DVAR1=<value for variable key VAR1 here> -DVAR2=<value for variable key VAR2 here>
	 * @endcode
	 * 
	 * A possible IOptionHandler for "-sku <your SKU code here>" could be implemented as follows:
	 * @code
	 * class SkuOptionHandler : public tc::cli::OptionParser::IOptionHandler
	 * {
	 * public:
	 * 	// The constructor is where you link the object to the state you want to modify in the call-back
	 * 	SkuOptionHandler(UserOpt& user_opt) : 
	 * 		mUserOpt(user_opt),
	 * 		mOptStrings({"-sku"}), // here we define an array of literal options to match against
	 * 		mOptRegex()
	 * 	{}
	 * 
	 * 	// OptionParser uses this to determine which IOptionHandler to use, so this should return all aliases of the option
	 * 	const std::vector<std::string>& getOptionStrings() const
	 * 	{
	 * 		return mOptStrings;
	 * 	}
	 *
	 * 	// OptionParser uses this to determine which IOptionHandler to use, so this should return all regex patterns that will match for the option
	 * 	const std::vector<std::string>& getOptionRegexPatterns() const
	 * 	{
	 * 		return mOptRegex;
	 * 	}
	 * 
	 * 	// This is what is called when OptionParser defers to IOptionHandler to process the option and any arguments
	 * 	// In your implementation this is where you validate the data and modify your linked state data accordingly
	 * 	void processOption(const std::string& option, const std::vector<std::string>& params)
	 * 	{
	 * 		// validate number of paramaters (in this case you we only want 1 parameter)
	 * 		if (params.size() != 1)
	 * 		{
	 * 			throw tc::ArgumentOutOfRangeException("Option \"" + option + "\" requires a parameter.");
	 * 		}
	 * 
	 * 		mUserOpt.sku_code = params[0];
	 * 	}
	 * private:
	 * 	UserOpt& mUserOpt;
	 * 	std::vector<std::string> mOptStrings;
	 * 	std::vector<std::string> mOptRegex;
	 * };
	 * @endcode
	 *
	 * A possible IOptionHandler for generic "-DKEY=VALUE" could be implemented as follows:
	 * @code
	 * class KeyValueOptionHandler : public tc::cli::OptionParser::IOptionHandler
	 * {
	 * public:
	 * 	// The constructor is where you link the object to the state you want to modify in the call-back
	 * 	KeyValueOptionHandler(UserOpt& user_opt) : 
	 * 		mUserOpt(user_opt),
	 * 		mOptStrings(),
	 * 		mOptRegex({"(-D)(.+)"}) // here we define a REGEX pattern to match the beginning of the option "-D" followed by the key.
	 * 	{}
	 * 
	 * 	// OptionParser uses this to determine which IOptionHandler to use, so this should return all aliases of the option
	 * 	const std::vector<std::string>& getOptionStrings() const
	 * 	{
	 * 		return mOptStrings;
	 * 	}
	 * 
	 * 	// OptionParser uses this to determine which IOptionHandler to use, so this should return all regex patterns that will match for the option
	 * 	const std::vector<std::string>& getOptionRegexPatterns() const
	 * 	{
	 * 		return mOptRegex;
	 * 	}
	 *
	 * 	// This is what is called when OptionParser defers to IOptionHandler to process the option and any arguments
	 * 	// In your implementation this is where you validate the data and modify your linked state data accordingly
	 * 	void processOption(const std::string& option, const std::vector<std::string>& params)
	 * 	{
	 * 		// validate number of paramaters (in this case you we only want 1 parameter)
	 * 		if (params.size() != 1)
	 * 		{
	 * 			throw tc::ArgumentOutOfRangeException("Option \"" + option + "\" requires a parameter.");
	 * 		}
	 * 
	 * 		mUserOpt.environment_vars.insert(std::pair<std::string>(option.substr(2), params[0]));
	 * 	}
	 * private:
	 * 	UserOpt& mUserOpt;
	 * 	std::vector<std::string> mOptStrings;
	 * 	std::vector<std::string> mOptRegex;
	 * };
	 * @endcode
	 * 
	 * Defining an unknown option handler is optional, but at a minimum allows customising the exception.
	 * @code
	 * class UnkOptionHandler : public tc::cli::OptionParser::IOptionHandler
	 * {
	 * public:
	 * 	UnkOptionHandler()
	 * 	{}
	 * 
	 * 	// this throws an exception as it should not be called
	 * 	const std::vector<std::string>& getOptionStrings() const
	 * 	{
	 * 		throw tc::InvalidOperationException("getOptionStrings() not defined for UnkOptionHandler.");
	 * 	}
	 *
	 * 	// this throws an exception as it should not be called
	 * 	const std::vector<std::string>& getOptionRegexPatterns() const
	 * 	{
	 * 		throw tc::InvalidOperationException("getOptionRegexPatterns() not defined for UnkOptionHandler.");
	 * 	}
	 * 
	 * 	void processOption(const std::string& option, const std::vector<std::string>& params)
	 * 	{
	 * 		throw tc::Exception("Unrecognized option: \"" + option + "\"");
	 * 	}
	 * private:
	 * };
	 * @endcode
	 * 
	 * Then process the command-line arguments with OptionParser::processOptions():
	 * @code
	 * int umain(const std::vector<std::string>& args, const std::vector<std::string>& env)
	 * {
	 * 	UserOpt user_opt;
	 * 	tc::cli::OptionParser opt_parser;
	 * 
	 * 	// register the option handler for "-sku"
	 * 	opt_parser.registerOptionHandler(std::shared_ptr<SkuOptionHandler>(new SkuOptionHandler(user_opt)));
	 *
	 * 	// register the option handler for "-DKEY=VALUE"
	 * 	opt_parser.registerOptionHandler(std::shared_ptr<KeyValueOptionHandler>(new KeyValueOptionHandler(user_opt)));
	 * 
	 * 	// register the unknown option handler
	 * 	opt_parser.registerUnrecognisedOptionHandler(std::shared_ptr<UnkOptionHandler>(new UnkOptionHandler()));
	 * 
	 * 	// since args will include at args[0], the program executable path, use the overload of processOptions that selects a sub vector of args.
	 * 	opt_parser.processOptions(args, 1, args.size()-1);
	 * 
	 * 	// user_opt.sku_type will now be populated if it was set via command-line with "-sku"
	 * 	// user_opt.environment_vars will now be populated if it was set via command-line with "-DKEY=VALUE" style options.
	 * 
	 * 	std::cout << "SKUCODE: \"" << user_opt.sku_code << "\"" << std::endl;
	 * 	for (auto itr = user_opt.environment_vars.begin(); itr != environment_vars.end(); itr++)
	 * 	{
	 * 		std::cout << "EnvVar: [" << itr->first << "] -> [" << itr->second << "]" << std::endl;
	 * 	}
	 * 
	 * 	// finish program
	 * 	return 0;
	 * }
	 * @endcode
	 */
class OptionParser
{
public:

		/**
		 * @class IOptionHandler
		 * @brief Interface for handling command-line options and any parameters, to be used with @ref OptionParser.
		 * 
		 * See @ref OptionParser for an example on how to implement this class.
		 */
	class IOptionHandler
	{
	public:
		virtual ~IOptionHandler() = default;

			/**
			 * @brief Returns a vector of aliases for the option this will handle.
			 */
		virtual const std::vector<std::string>& getOptionStrings() const = 0;

			/**
			 * @brief Returns a vector of option regex patterns that this will handle.
			 */
		virtual const std::vector<std::string>& getOptionRegexPatterns() const = 0;

			/**
			 * @brief Processes command-line option and any parameters.
			 * 
			 * @param[in] option This is full option name that was used.
			 * @param[in] params This is a vector of parameters that were supplied with the option name, size may be 0 or more.
			 */ 
		virtual void processOption(const std::string& option, const std::vector<std::string>& params) = 0;
	};

		/// Default Constructor
	OptionParser();

		/**
		 * @brief Register an IOptionHandler to handle an option.
		 * 
		 * @param[in] handler Shared pointer to the IOptionHandler.
		 *
		 * @throw tc::ArgumentNullException @p handler was null.
		 * @throw tc::ArgumentOutOfRangeException @p handler had no option strings or regex patterns.
		 */
	void registerOptionHandler(const std::shared_ptr<IOptionHandler>& handler);

		/**
		 * @brief Register an IOptionHandler to handle all unrecognised options. Only use this extra processing of unrecognised options is required beyond a generic exception.
		 * 
		 * @param[in] handler Shared pointer to the IOptionHandler.
		 *
		 * @throw tc::ArgumentNullException @p handler was null.
		 */
	void registerUnrecognisedOptionHandler(const std::shared_ptr<IOptionHandler>& handler);

		/**
		 * @brief Process a vector of command-line args in accordance with the registered option handlers.
		 * 
		 * @param[in] args Command-line args to process.
		 *
		 * @throw tc::ArgumentException An option was not recognised, or otherwise malformed. 
		 */
	void processOptions(const std::vector<std::string>& args);

		/**
		 * @brief Process a vector of command-line args in accordance with the registered option handlers.
		 * 
		 * @param[in] args Command-line args to process.
		 * @param[in] pos Position in args vector to begin with.
		 * @param[in] num Number of elements in the args vector to process.
		 *
		 * @throw tc::ArgumentException An option was not recognised, or otherwise malformed. 
		 */
	void processOptions(const std::vector<std::string>& args, size_t pos, size_t num);

private:
	static const std::string kClassName;

	void handleOption(const std::string& opt, const std::vector<std::string>& params);
	std::map<std::string, std::shared_ptr<IOptionHandler>> mOptionaAliasMap;
	std::list<std::pair<std::regex, std::shared_ptr<IOptionHandler>>> mOptionRegexList;
	std::shared_ptr<IOptionHandler> mUnkOptHandler;
};

}} // namespace tc::cli