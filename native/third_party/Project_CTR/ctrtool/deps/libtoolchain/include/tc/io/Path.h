/**
 * @file Path.h
 * @brief Declaration of tc::io::Path
 * @author Jack (jakcron)
 * @version 0.5
 * @date 2022/02/27
 */
#pragma once
#include <list>
#include <tc/types.h>

#include <tc/ArgumentException.h>

namespace tc { namespace io {

	/**
	 * @class Path
	 * @brief Represents a unicode path for a filesystem
	 *
	 * This stores a path as a list of path elements.
	 **/
class Path
{
public:

		/**
		 * @brief This enum defines the Path format type, used for encoding to string.
		 * @details 
		 * This defines the encoding type used when encoding Path as a string. 
		 * See @ref to_string(), @ref to_u16string() and @ref to_u32string() for more info.
		 */
	enum class Format
	{
		Native = 0, /**< Path format for the native environment */
		POSIX = 1, /**< Path format for POSIX based environments (Unix, Linux, macOS, WSL, etc) */
		Win32 = 2, /**< Path format for Microsoft Windows based environments */
	};

		/// Type of const_iterator for Path
	using const_iterator = typename std::list<std::string>::const_iterator;

		/// Type of iterator for Path
	using iterator = typename std::list<std::string>::iterator;

		/// Maximum value for size_t, used to indicated unbounded length for @ref subpath()
	static const size_t npos = -1;

		/// Default Constructor
	Path();

		/// Initalizer List Constructor
	Path(std::initializer_list<std::string> list);

		/**
		 * @brief Create Path from a literal UTF-8 encoded string
		 * 
		 * @param[in] path UTF-8 encoded path
		 * 
		 * @pre
		 * - @p path can have either forward or backward slash path separators ('/' or '\') but not both
		 * 
		 * @throws tc::ArgumentException Path literal has both forward ('/') and backward ('\') path separators.
		 * 
		 * @note No filtering or processing of special characters is done (e.g. '.', '~')
		 **/
	Path(const std::string& path);

		/**
		 * @brief Create Path from a literal UTF-16 encoded string
		 *
		 * @param[in] path UTF-16 encoded path
		 * 
		 * @pre
		 * - @p path can have either forward or backward slash path separators ('/' or '\') but not both
		 * 
		 * @throws tc::ArgumentException Path literal has both forward ('/') and backward ('\') path separators.
		 * 
		 * @note 
		 * No filtering or processing of special characters is done (e.g. '.', '~')
		 **/
	Path(const std::u16string& path);

		/**
		 * @brief Create Path from a literal UTF-32 encoded string
		 *
		 * @param[in] path UTF-32 encoded path
		 * 
		 * @pre
		 * - @p path can have either forward or backward slash path separators ('/' or '\') but not both
		 * 
		 * @throws tc::ArgumentException Path literal has both forward ('/') and backward ('\') path separators.
		 * 
		 * @note 
		 * No filtering or processing of special characters is done (e.g. '.', '~')
		 **/
	Path(const std::u32string& path);

		/// Addition operator
	Path operator+(const Path& other) const;

		/// Append operator
	void operator+=(const Path& other);

		/// Equality operator
	bool operator==(const Path& other) const;

		/// Inequality operator
	bool operator!=(const Path& other) const;

		/// Comparison operator
	bool operator<(const Path& other) const;

		/**
		 * @brief Returns a reference to the first element in the container.
		 * 
		 * @return reference to the first element 
		 * 
		 * @note Calling front on an empty container is undefined.
		 * @note For a Path p, the expression p.front() is equivalent to *p.begin(). 
		 **/
	std::string& front();

		/**
		 * @brief Returns a const reference to the first element in the container.
		 * 
		 * @return const reference to the first element 
		 * 
		 * @note Calling front on an empty container is undefined.
		 * @note For a Path p, the expression p.front() is equivalent to *p.begin(). 
		 **/
	const std::string& front() const;

		/**
		 * @brief Returns a reference to the last element in the container.
		 * 
		 * @return reference to the last element 
		 * 
		 * @note Calling back on an empty container is undefined.
		 * @note For a Path p, the expression p.back() is equivalent to *(--p.end()). 
		 **/
	std::string& back();

		/**
		 * @brief Returns a const reference to the last element in the container.
		 * 
		 * @return const reference to the last element 
		 * 
		 * @note Calling back on an empty container is undefined.
		 * @note For a Path p, the expression p.back() is equivalent to *(--p.end()). 
		 **/
	const std::string& back() const;

		/// Begin Iterator, points to front element
	iterator begin();

		/// Const Begin Iterator, points to front element
	const_iterator begin() const;

		/// End Iterator, points to after the last element
	iterator end();

		/// Const End Iterator, points to after the last element
	const_iterator end() const;

		/**
		 * @brief Remove element at the front of the path
		 * 
		 * @note Calling pop_front on an empty container is undefined.
		 **/
	void pop_front();

		/**
		 * @brief Remove element at the back of the path
		 * 
		 * @note Calling pop_back on an empty container is undefined.
		 **/
	void pop_back();

		/// Insert path element at the front of the path
	void push_front(const std::string& str);

		/// Insert path element at the back of the path
	void push_back(const std::string& str);

		/// Clear all elements from the path
	void clear();

		/// Get number of path elements
	size_t size() const;

		/// Checks whether the path is empty 
	bool empty() const;	

		/**
		 * @brief Create a path from a subset of this path
		 * 
		 * @param[in] pos Position of first path element
		 * @param[in] len Number of path elements. Default value is @ref npos, indicating include all path elements after @p pos.
		 * @return tc::io::Path Sub-path created from this path.
		 */ 
	tc::io::Path subpath(size_t pos, size_t len = npos) const;

		/**
		 * @brief Create a path from a subset of this path
		 * 
		 * @param[in] begin Iterator pointing to the first element
		 * @param[in] end Iterator 
		 * @return tc::io::Path Sub-path created from this path.
		 */ 
	tc::io::Path subpath(const_iterator begin, const_iterator end) const;

		/**
		 * @brief Convert path to std::string
		 * 
		 * @param[in] format @ref tc::io::Path::Format format to encode path as string. Default is @ref tc::io::Path::Format::Native.
		 * @return std::string UTF-8 encoded path string
		 */
	std::string to_string(Format format = Format::Native) const;

		/**
		 * @brief Convert path to std::u16string
		 * 
		 * @param[in] format @ref tc::io::Path::Format format to encode path as string. Default is @ref tc::io::Path::Format::Native.
		 * @return std::u16string UTF-16 encoded path string
		 */
	std::u16string to_u16string(Format format = Format::Native) const;

			/**
		 * @brief Convert path to std::u32string
		 * 
		 * @param[in] format @ref tc::io::Path::Format format to encode path as string. Default is @ref tc::io::Path::Format::Native.
		 * @return std::u32string UTF-32 encoded path string
		 */
	std::u32string to_u32string(Format format = Format::Native) const;

		/// Implicit conversion to a natively formatted std::string
	operator std::string() const;

		/// Implicit conversion to a natively formatted std::u16string
	operator std::u16string() const;

		/// Implicit conversion to a natively formatted std::u32string
	operator std::u32string() const;
private:
	static const std::string kClassName;

	std::list<std::string> mUnicodePath;

	void initializePath(const std::string& src);
	void appendPath(const std::list<std::string>& other);
};

}} // namespace tc::io