	/**
	 * @file endian_types.h
	 * @brief Declaration of macros and classes to unwrap primatives in an endian agnostic way.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/12/05
	 **/
#pragma once
#include <cinttypes>
#include <type_traits>

namespace tc { namespace bn { namespace detail {

static inline uint16_t __local_bswap16(uint16_t x) {
	return ((x << 8) & 0xff00) | ((x >> 8) & 0x00ff);
}

static inline void __local_bswap16(void* x) {
	uint16_t tmp = *((uint16_t*)x);
	*((uint16_t*)x) = ((tmp << 8) & 0xff00) | ((tmp >> 8) & 0x00ff);
}

static inline uint32_t __local_bswap32(uint32_t x) {
	return	((x << 24) & 0xff000000 ) |
			((x <<  8) & 0x00ff0000 ) |
			((x >>  8) & 0x0000ff00 ) |
			((x >> 24) & 0x000000ff );
}

static inline void __local_bswap32(void* x) {
	uint32_t tmp = *((uint32_t*)x);
	*((uint32_t*)x) =	((tmp << 24) & 0xff000000 ) |
						((tmp <<  8) & 0x00ff0000 ) |
						((tmp >>  8) & 0x0000ff00 ) |
						((tmp >> 24) & 0x000000ff );
}

static inline uint64_t __local_bswap64(uint64_t x)
{
	return (uint64_t)__local_bswap32(x>>32) |
	      ((uint64_t)__local_bswap32(x&0xFFFFFFFF) << 32);
}

static inline void __local_bswap64(void* x) {
	uint64_t tmp = *((uint64_t*)x);
	*((uint64_t*)x) =	((uint64_t)(tmp << 56) & (uint64_t)0xff00000000000000ULL ) |
						((uint64_t)(tmp << 40) & (uint64_t)0x00ff000000000000ULL ) |
						((uint64_t)(tmp << 24) & (uint64_t)0x0000ff0000000000ULL ) |
						((uint64_t)(tmp <<  8) & (uint64_t)0x000000ff00000000ULL ) |
						((uint64_t)(tmp >>  8) & (uint64_t)0x00000000ff000000ULL ) |
						((uint64_t)(tmp >> 24) & (uint64_t)0x0000000000ff0000ULL ) |
						((uint64_t)(tmp >> 40) & (uint64_t)0x000000000000ff00ULL ) |
						((uint64_t)(tmp >> 56) & (uint64_t)0x00000000000000ffULL );
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline uint64_t __be_uint64(uint64_t a) { return __local_bswap64(a); }
static inline uint32_t __be_uint32(uint32_t a) { return __local_bswap32(a); }
static inline uint16_t __be_uint16(uint16_t a) { return __local_bswap16(a); }
static inline uint64_t __le_uint64(uint64_t a) { return a; }
static inline uint32_t __le_uint32(uint32_t a) { return a; }
static inline uint16_t __le_uint16(uint16_t a) { return a; }

static inline void __be_swap64(void* a) { __local_bswap64(a); }
static inline void __be_swap32(void* a) { __local_bswap32(a); }
static inline void __be_swap16(void* a) { __local_bswap16(a); }
static inline void __le_swap64(void* a) { return; }
static inline void __le_swap32(void* a) { return; }
static inline void __le_swap16(void* a) { return; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
static inline uint64_t __be_uint64(uint64_t a) { return a; }
static inline uint32_t __be_uint32(uint32_t a) { return a; }
static inline uint16_t __be_uint16(uint16_t a) { return a; }
static inline uint64_t __le_uint64(uint64_t a) { return __local_bswap64(a); }
static inline uint32_t __le_uint32(uint32_t a) { return __local_bswap32(a); }
static inline uint16_t __le_uint16(uint16_t a) { return __local_bswap16(a); }

static inline void __be_swap64(void* a) { return; }
static inline void __be_swap32(void* a) { return; }
static inline void __be_swap16(void* a) { return; }
static inline void __le_swap64(void* a) { __local_bswap64(a); }
static inline void __le_swap32(void* a) { __local_bswap32(a); }
static inline void __le_swap16(void* a) { __local_bswap16(a); }
#endif

}}} // namespace tc::bn::detail

namespace tc { namespace bn {

	/**
	 * @struct le16
	 * @brief Wrapper that allows accessing a little-endian 16-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct le16 {
public:
	static_assert(sizeof(T) == sizeof(uint16_t), "le16 requires T to be 16 bit.");
	static_assert(std::is_pod<T>::value, "le16 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	le16& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__le_swap16(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__le_swap16(&mVar); }
private:
	T mVar;
};

	/**
	 * @struct be16
	 * @brief Wrapper that allows accessing a big-endian 16-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct be16 {
public:
	static_assert(sizeof(T) == sizeof(uint16_t), "be16 requires T to be 16 bit.");
	static_assert(std::is_pod<T>::value, "be16 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	be16& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__be_swap16(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__be_swap16(&mVar); }
private:
	T mVar;
};

	/**
	 * @struct le32
	 * @brief Wrapper that allows accessing a little-endian 32-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct le32 {
public:
	static_assert(sizeof(T) == sizeof(uint32_t), "le32 requires T to be 32 bit.");
	static_assert(std::is_pod<T>::value, "le32 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	le32& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__le_swap32(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__le_swap32(&mVar); }
private:
	T mVar;
};

	/**
	 * @struct be32
	 * @brief Wrapper that allows accessing a big-endian 32-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct be32 {
public:
	static_assert(sizeof(T) == sizeof(uint32_t), "be32 requires T to be 32 bit.");
	static_assert(std::is_pod<T>::value, "be32 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	be32& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__be_swap32(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__be_swap32(&mVar); }
private:
	T mVar;
};

	/**
	 * @struct le64
	 * @brief Wrapper that allows accessing a little-endian 64-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct le64 {
public:
	static_assert(sizeof(T) == sizeof(uint64_t), "le64 requires T to be 64 bit.");
	static_assert(std::is_pod<T>::value, "le64 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	le64& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__le_swap64(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__le_swap64(&mVar); }
private:
	T mVar;
};

	/**
	 * @struct be64
	 * @brief Wrapper that allows accessing a big-endian 64-bit POD regardless of processor endianness 
	 **/
template <typename T>
struct be64 {
public:
	static_assert(sizeof(T) == sizeof(uint64_t), "be64 requires T to be 64 bit.");
	static_assert(std::is_pod<T>::value, "be64 requires T to be a POD.");

		/// Unwrap value (Implicit)
	operator T() const { return unwrap(); }
		/// Wrap value (Implicit)
	be64& operator=(const T& var) { wrap(var); return *this; }

		/// Unwrap value
	inline T unwrap() const { T tmp = mVar; detail::__be_swap64(&tmp); return tmp; }
		/// Wrap value
	inline void wrap(const T& var) { mVar = var; detail::__be_swap64(&mVar); }
private:
	T mVar;
};

}} // namespace tc::bn

namespace tc { namespace bn {


}} // namespace tc::bn