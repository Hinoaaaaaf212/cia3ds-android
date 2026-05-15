#include <tc/crypto/RsaOaepSha256Encryptor.h>

bool tc::crypto::EncryptRsa1024OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa1024OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.encrypt(block, message, message_size);
}

bool tc::crypto::DecryptRsa1024OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa1024OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.decrypt(message, message_size, message_capacity, block);
}

bool tc::crypto::EncryptRsa2048OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa2048OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.encrypt(block, message, message_size);
}

bool tc::crypto::DecryptRsa2048OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa2048OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.decrypt(message, message_size, message_capacity, block);
}

bool tc::crypto::EncryptRsa4096OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa4096OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.encrypt(block, message, message_size);
}

bool tc::crypto::DecryptRsa4096OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested)
{
	tc::crypto::Rsa4096OaepSha256Encryptor impl;
	impl.initialize(key, label, label_size, isLabelDigested);
	return impl.decrypt(message, message_size, message_capacity, block);
}
