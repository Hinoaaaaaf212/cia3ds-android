#include "string_TranscodeUtil_TestClass.h"
#include "ByteData_TestClass.h"
#include "bn_binaryutils_TestClass.h"
#include "bn_pad_TestClass.h"
#include "bn_string_TestClass.h"
#include "bn_endian_TestClass.h"
#include "bn_bitarrayByteBEBitBE_TestClass.h"
#include "bn_bitarrayByteBEBitLE_TestClass.h"
#include "bn_bitarrayByteLEBitBE_TestClass.h"
#include "bn_bitarrayByteLEBitLE_TestClass.h"
#include "Optional_TestClass.h"
#include "io_Path_TestClass.h"
#include "io_BasicPathResolver_TestClass.h"
#include "io_LocalFileSystem_TestClass.h"
#include "io_FileStream_TestClass.h"
#include "io_MemoryStream_TestClass.h"
#include "io_SubStream_TestClass.h"
#include "io_ConcatenatedStream_TestClass.h"
#include "io_SubFileSystem_TestClass.h"
#include "io_VirtualFileSystem_TestClass.h"
#include "io_PaddingSource_TestClass.h"
#include "io_MemorySource_TestClass.h"
#include "io_OverlayedSource_TestClass.h"
#include "io_SubSource_TestClass.h"
#include "io_SubSink_TestClass.h"
#include "io_StreamSource_TestClass.h"
#include "io_StreamSink_TestClass.h"
#include "cli_FormatUtil_TestClass.h"
#include "cli_OptionParser_TestClass.h"
#include "crypto_Md5Generator_TestClass.h"
#include "crypto_Sha1Generator_TestClass.h"
#include "crypto_Sha256Generator_TestClass.h"
#include "crypto_Sha512Generator_TestClass.h"
#include "crypto_HmacMd5Generator_TestClass.h"
#include "crypto_HmacSha1Generator_TestClass.h"
#include "crypto_HmacSha256Generator_TestClass.h"
#include "crypto_HmacSha512Generator_TestClass.h"
#include "crypto_Pbkdf1Md5KeyDeriver_TestClass.h"
#include "crypto_Pbkdf1Sha1KeyDeriver_TestClass.h"
#include "crypto_Pbkdf2Sha1KeyDeriver_TestClass.h"
#include "crypto_Pbkdf2Sha256KeyDeriver_TestClass.h"
#include "crypto_Pbkdf2Sha512KeyDeriver_TestClass.h"
#include "crypto_PseudoRandomByteGenerator_TestClass.h"
#include "crypto_Aes128Encryptor_TestClass.h"
#include "crypto_Aes192Encryptor_TestClass.h"
#include "crypto_Aes256Encryptor_TestClass.h"
#include "crypto_Aes128EcbEncryptor_TestClass.h"
#include "crypto_Aes192EcbEncryptor_TestClass.h"
#include "crypto_Aes256EcbEncryptor_TestClass.h"
#include "crypto_Aes128CbcEncryptor_TestClass.h"
#include "crypto_Aes192CbcEncryptor_TestClass.h"
#include "crypto_Aes256CbcEncryptor_TestClass.h"
#include "crypto_Aes128CbcEncryptedStream_TestClass.h"
#include "crypto_Aes128CtrEncryptor_TestClass.h"
#include "crypto_Aes192CtrEncryptor_TestClass.h"
#include "crypto_Aes256CtrEncryptor_TestClass.h"
#include "crypto_Aes128CtrEncryptedStream_TestClass.h"
#include "crypto_Aes128XtsEncryptor_TestClass.h"
#include "crypto_Aes256XtsEncryptor_TestClass.h"
#include "crypto_Rsa1024OaepSha256Encryptor_TestClass.h"
#include "crypto_Rsa2048OaepSha256Encryptor_TestClass.h"
#include "crypto_Rsa4096OaepSha256Encryptor_TestClass.h"
#include "crypto_Rsa2048OaepSha512Encryptor_TestClass.h"
#include "crypto_Rsa4096OaepSha512Encryptor_TestClass.h"
#include "crypto_Rsa1024Pkcs1Md5Signer_TestClass.h"
#include "crypto_Rsa2048Pkcs1Md5Signer_TestClass.h"
#include "crypto_Rsa4096Pkcs1Md5Signer_TestClass.h"
#include "crypto_Rsa1024Pkcs1Sha1Signer_TestClass.h"
#include "crypto_Rsa2048Pkcs1Sha1Signer_TestClass.h"
#include "crypto_Rsa4096Pkcs1Sha1Signer_TestClass.h"
#include "crypto_Rsa1024Pkcs1Sha256Signer_TestClass.h"
#include "crypto_Rsa2048Pkcs1Sha256Signer_TestClass.h"
#include "crypto_Rsa4096Pkcs1Sha256Signer_TestClass.h"
#include "crypto_Rsa1024Pkcs1Sha512Signer_TestClass.h"
#include "crypto_Rsa2048Pkcs1Sha512Signer_TestClass.h"
#include "crypto_Rsa4096Pkcs1Sha512Signer_TestClass.h"
#include "crypto_Rsa1024PssSha256Signer_TestClass.h"
#include "crypto_Rsa1024PssSha512Signer_TestClass.h"
#include "crypto_Rsa2048PssSha256Signer_TestClass.h"
#include "crypto_Rsa2048PssSha512Signer_TestClass.h"
#include "crypto_Rsa4096PssSha256Signer_TestClass.h"
#include "crypto_Rsa4096PssSha512Signer_TestClass.h"

#include <iostream>

void runTest(ITestClass* testClass)
{
	testClass->runAllTests();
	delete testClass;
}

int main(int argc, char** argv)
{
	bool includeSlowTests = false;
	if (argc > 1)
	{
		static const std::string kNoSlowTestFlag = "--slow";
		if (strncmp(argv[1], kNoSlowTestFlag.c_str(), kNoSlowTestFlag.size()) == 0)
		{
			includeSlowTests = true;
		}
		else
		{
			std::cout << "usage: " << std::string(argv[0]) << " [--slow]" << std::endl;
			return 1;
		}
		
	}

	runTest(new string_TranscodeUtil_TestClass());
	runTest(new ByteData_TestClass());
	runTest(new bn_binaryutils_TestClass());
	runTest(new bn_pad_TestClass());
	runTest(new bn_string_TestClass());
	runTest(new bn_endian_TestClass());
	runTest(new bn_bitarrayByteBEBitBE_TestClass());
	runTest(new bn_bitarrayByteBEBitLE_TestClass());
	runTest(new bn_bitarrayByteLEBitBE_TestClass());
	runTest(new bn_bitarrayByteLEBitLE_TestClass());
	runTest(new Optional_TestClass());
	runTest(new io_Path_TestClass());
	runTest(new io_BasicPathResolver_TestClass());
	runTest(new io_LocalFileSystem_TestClass());
	runTest(new io_FileStream_TestClass());
	runTest(new io_MemoryStream_TestClass());
	runTest(new io_SubStream_TestClass());
	runTest(new io_ConcatenatedStream_TestClass());
	runTest(new io_SubFileSystem_TestClass());
	runTest(new io_VirtualFileSystem_TestClass());
	runTest(new io_PaddingSource_TestClass());
	runTest(new io_MemorySource_TestClass());
	runTest(new io_OverlayedSource_TestClass());
	runTest(new io_SubSource_TestClass());
	runTest(new io_SubSink_TestClass());
	runTest(new io_StreamSource_TestClass());
	runTest(new io_StreamSink_TestClass());
	runTest(new cli_FormatUtil_TestClass());
	runTest(new cli_OptionParser_TestClass());
	runTest(new crypto_Md5Generator_TestClass());
	runTest(new crypto_Sha1Generator_TestClass());
	runTest(new crypto_Sha256Generator_TestClass());
	runTest(new crypto_Sha512Generator_TestClass());
	runTest(new crypto_HmacMd5Generator_TestClass());
	runTest(new crypto_HmacSha1Generator_TestClass());
	runTest(new crypto_HmacSha256Generator_TestClass());
	runTest(new crypto_HmacSha512Generator_TestClass());
	if (includeSlowTests)
	{
		runTest(new crypto_Pbkdf1Md5KeyDeriver_TestClass());
		runTest(new crypto_Pbkdf1Sha1KeyDeriver_TestClass());
		runTest(new crypto_Pbkdf2Sha1KeyDeriver_TestClass());
		runTest(new crypto_Pbkdf2Sha256KeyDeriver_TestClass());
		runTest(new crypto_Pbkdf2Sha512KeyDeriver_TestClass());
	}
	runTest(new crypto_PseudoRandomByteGenerator_TestClass());
	runTest(new crypto_Aes128Encryptor_TestClass());
	runTest(new crypto_Aes192Encryptor_TestClass());
	runTest(new crypto_Aes256Encryptor_TestClass());
	runTest(new crypto_Aes128EcbEncryptor_TestClass());
	runTest(new crypto_Aes192EcbEncryptor_TestClass());
	runTest(new crypto_Aes256EcbEncryptor_TestClass());
	runTest(new crypto_Aes128CbcEncryptor_TestClass());
	runTest(new crypto_Aes192CbcEncryptor_TestClass());
	runTest(new crypto_Aes256CbcEncryptor_TestClass());
	runTest(new crypto_Aes128CbcEncryptedStream_TestClass());
	runTest(new crypto_Aes128CtrEncryptor_TestClass());
	runTest(new crypto_Aes192CtrEncryptor_TestClass());
	runTest(new crypto_Aes256CtrEncryptor_TestClass());
	runTest(new crypto_Aes128CtrEncryptedStream_TestClass());
	runTest(new crypto_Aes128XtsEncryptor_TestClass());
	runTest(new crypto_Aes256XtsEncryptor_TestClass());
	runTest(new crypto_Rsa1024OaepSha256Encryptor_TestClass());
	runTest(new crypto_Rsa2048OaepSha256Encryptor_TestClass());
	runTest(new crypto_Rsa4096OaepSha256Encryptor_TestClass());
	runTest(new crypto_Rsa2048OaepSha512Encryptor_TestClass());
	runTest(new crypto_Rsa4096OaepSha512Encryptor_TestClass());
	runTest(new crypto_Rsa1024Pkcs1Md5Signer_TestClass());
	runTest(new crypto_Rsa2048Pkcs1Md5Signer_TestClass());
	runTest(new crypto_Rsa4096Pkcs1Md5Signer_TestClass());
	runTest(new crypto_Rsa1024Pkcs1Sha1Signer_TestClass());
	runTest(new crypto_Rsa2048Pkcs1Sha1Signer_TestClass());
	runTest(new crypto_Rsa4096Pkcs1Sha1Signer_TestClass());
	runTest(new crypto_Rsa1024Pkcs1Sha256Signer_TestClass());
	runTest(new crypto_Rsa2048Pkcs1Sha256Signer_TestClass());
	runTest(new crypto_Rsa4096Pkcs1Sha256Signer_TestClass());
	runTest(new crypto_Rsa1024Pkcs1Sha512Signer_TestClass());
	runTest(new crypto_Rsa2048Pkcs1Sha512Signer_TestClass());
	runTest(new crypto_Rsa4096Pkcs1Sha512Signer_TestClass());
	runTest(new crypto_Rsa1024PssSha256Signer_TestClass());
	runTest(new crypto_Rsa1024PssSha512Signer_TestClass());
	runTest(new crypto_Rsa2048PssSha256Signer_TestClass());
	runTest(new crypto_Rsa2048PssSha512Signer_TestClass());
	runTest(new crypto_Rsa4096PssSha256Signer_TestClass());
	runTest(new crypto_Rsa4096PssSha512Signer_TestClass());
}