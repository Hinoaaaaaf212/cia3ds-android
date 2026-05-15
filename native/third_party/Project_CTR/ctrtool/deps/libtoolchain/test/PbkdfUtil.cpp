#include "PbkdfUtil.h"

#include <tc/cli/FormatUtil.h>

void PbkdfUtil::generatePbkdf1TestVectors_Custom(std::vector<PbkdfUtil::TestVector>& test_list, PbkdfUtil::HashAlgo hash_algo)
{
	std::array<PbkdfUtil::TestVector, 6> tests;

	// test 0
	tests[0].test_name = "test 1";
	tests[0].in_password = "password";
	tests[0].in_salt = "salt";
	tests[0].in_rounds = 1;

	// test 1
	tests[1].test_name = "test 2";
	tests[1].in_password = "password";
	tests[1].in_salt = "salt";
	tests[1].in_rounds = 2;

	// test 2
	tests[2].test_name = "test 3";
	tests[2].in_password = "password";
	tests[2].in_salt = "salt";
	tests[2].in_rounds = 4096;

	// test 3
	tests[3].test_name = "test 4";
	tests[3].in_password = "password";
	tests[3].in_salt = "salt";
	tests[3].in_rounds = 16777216;

	// test 4
	tests[4].test_name = "test 5";
	tests[4].in_password = "passwordPASSWORDpassword";
	tests[4].in_salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
	tests[4].in_rounds = 4096;

	// test 5
	tests[5].test_name = "test 6";
	tests[5].in_password = std::string("pass\0word", 9);
	tests[5].in_salt = std::string("sa\0lt", 5);
	tests[5].in_rounds = 4096;

	if (hash_algo == HashAlgo::MD5)
	{
		tests[0].in_dk_len = 16;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("B305CADBB3BCE54F3AA59C64FEC00DEA");

		tests[1].in_dk_len = 16;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("5B6DAD229782C6547D1B20D5668EB834");

		tests[2].in_dk_len = 16;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("C8C1EA2F5E2357447AE9244725FAABB9");

		tests[3].in_dk_len = 16;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("6BE21BD5D64E926E78F543DC119875E7");

		tests[4].in_dk_len = 16;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("DE806293ACB904A2B63DE565C014A6C3");

		tests[5].in_dk_len = 8;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("921F9BB3A34D998B");
	}
	else if (hash_algo == HashAlgo::SHA1)
	{
		tests[0].in_dk_len = 20;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("C88E9C67041A74E0357BEFDFF93F87DDE0904214");

		tests[1].in_dk_len = 20;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("47E97E39E2B32B15EB9278E53F7BFCA57F8E6B2C");

		tests[2].in_dk_len = 20;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("2DE33AD2137E4650EA29FB13136F967B0A4508D9");

		tests[3].in_dk_len = 20;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("4287903B55F17B04B5D1E769A3AFB8290B9F3D50");

		tests[4].in_dk_len = 20;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("EBABF2EF0353667E5581021DD84B1342B6BCC8FA");

		tests[5].in_dk_len = 8;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("2B664B45A9A129DB");
	}
	else
	{
		// no case for provided hash algo
		return;
	}
	
	// copy populated tests to output
	for (size_t i = 0; i < tests.size(); i++)
		test_list.push_back(tests[i]);
}

void PbkdfUtil::generatePbkdf2TestVectors_RFC6070(std::vector<PbkdfUtil::TestVector>& test_list, PbkdfUtil::HashAlgo hash_algo)
{
	std::array<PbkdfUtil::TestVector, 6> tests;

	// test 0
	tests[0].test_name = "test 1";
	tests[0].in_password = "password";
	tests[0].in_salt = "salt";
	tests[0].in_rounds = 1;

	// test 1
	tests[1].test_name = "test 2";
	tests[1].in_password = "password";
	tests[1].in_salt = "salt";
	tests[1].in_rounds = 2;

	// test 2
	tests[2].test_name = "test 3";
	tests[2].in_password = "password";
	tests[2].in_salt = "salt";
	tests[2].in_rounds = 4096;

	// test 3
	tests[3].test_name = "test 4";
	tests[3].in_password = "password";
	tests[3].in_salt = "salt";
	tests[3].in_rounds = 16777216;

	// test 4
	tests[4].test_name = "test 5";
	tests[4].in_password = "passwordPASSWORDpassword";
	tests[4].in_salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
	tests[4].in_rounds = 4096;

	// test 5
	tests[5].test_name = "test 6";
	tests[5].in_password = std::string("pass\0word", 9);
	tests[5].in_salt = std::string("sa\0lt", 5);
	tests[5].in_rounds = 4096;

	if (hash_algo == HashAlgo::SHA1)
	{
		tests[0].in_dk_len = 20;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("0c60c80f961f0e71f3a9b524af6012062fe037a6");

		tests[1].in_dk_len = 20;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957");

		tests[2].in_dk_len = 20;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("4b007901b765489abead49d926f721d065a429c1");

		tests[3].in_dk_len = 20;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("eefe3d61cd4da4e4e9945b3d6ba2158c2634e984");

		tests[4].in_dk_len = 25;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038");

		tests[5].in_dk_len = 16;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("56fa6aa75548099dcc37d7f03425e0c3");
	}
	else if (hash_algo == HashAlgo::SHA224)
	{
		tests[0].in_dk_len = 28;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("3c198cbdb9464b7857966bd05b7bc92bc1cc4e6e63155d4e490557fd");

		tests[1].in_dk_len = 28;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("93200ffa96c5776d38fa10abdf8f5bfc0054b9718513df472d2331d2");

		tests[2].in_dk_len = 28;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("218c453bf90635bd0a21a75d172703ff6108ef603f65bb821aedade1");

		tests[3].in_dk_len = 28;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("b49925184cb4b559f365e94fcafcd4cdb9f7aef4a8ca8fcb4bd1ec53");

		tests[4].in_dk_len = 35;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("056c4ba438ded91fc14e0594e6f52b87e1f3690c0dc0fbc05784ed9a754ca780e6c017");

		tests[5].in_dk_len = 16;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("9b4011b641f40a2a500a31d4a392d15c");
	}
	else if (hash_algo == HashAlgo::SHA256)
	{
		tests[0].in_dk_len = 32;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b");

		tests[1].in_dk_len = 32;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43");

		tests[2].in_dk_len = 32;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("c5e478d59288c841aa530db6845c4c8d962893a001ce4e11a4963873aa98134a");

		tests[3].in_dk_len = 32;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("cf81c66fe8cfc04d1f31ecb65dab4089f7f179e89b3b0bcb17ad10e3ac6eba46");

		tests[4].in_dk_len = 40;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c4e2a1fb8dd53e1c635518c7dac47e9");

		tests[5].in_dk_len = 16;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("89b69d0516f829893c696226650a8687");
	}
	else if (hash_algo == HashAlgo::SHA384)
	{
		tests[0].in_dk_len = 48;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("c0e14f06e49e32d73f9f52ddf1d0c5c7191609233631dadd76a567db42b78676b38fc800cc53ddb642f5c74442e62be4");

		tests[1].in_dk_len = 48;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("54f775c6d790f21930459162fc535dbf04a939185127016a04176a0730c6f1f4fb48832ad1261baadd2cedd50814b1c8");

		tests[2].in_dk_len = 48;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("559726be38db125bc85ed7895f6e3cf574c7a01c080c3447db1e8a76764deb3c307b94853fbe424f6488c5f4f1289626");

		tests[3].in_dk_len = 48;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("a7fdb349ba2bfa6bf647bb0161bae1320df27e640a04e8f11148c81229e2131af179c3b3423b38abf763dfe208c1fb67");

		tests[4].in_dk_len = 60;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("819143ad66df9a552559b9e131c52ae6c5c1b0eed18f4d283b8c5c9eaeb92b392c147cc2d2869d58ffe2f7da13d15f8d925721f0ed1afafa24480d55");

		tests[5].in_dk_len = 16;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("a3f00ac8657e095f8e0823d232fc60b3");
	}
	else if (hash_algo == HashAlgo::SHA512)
	{
		tests[0].in_dk_len = 64;
		tests[0].out_dk = tc::cli::FormatUtil::hexStringToBytes("867f70cf1ade02cff3752599a3a53dc4af34c7a669815ae5d513554e1c8cf252c02d470a285a0501bad999bfe943c08f050235d7d68b1da55e63f73b60a57fce");

		tests[1].in_dk_len = 64;
		tests[1].out_dk = tc::cli::FormatUtil::hexStringToBytes("e1d9c16aa681708a45f5c7c4e215ceb66e011a2e9f0040713f18aefdb866d53cf76cab2868a39b9f7840edce4fef5a82be67335c77a6068e04112754f27ccf4e");

		tests[2].in_dk_len = 64;
		tests[2].out_dk = tc::cli::FormatUtil::hexStringToBytes("d197b1b33db0143e018b12f3d1d1479e6cdebdcc97c5c0f87f6902e072f457b5143f30602641b3d55cd335988cb36b84376060ecd532e039b742a239434af2d5");

		tests[3].in_dk_len = 64;
		tests[3].out_dk = tc::cli::FormatUtil::hexStringToBytes("6180a3ceabab45cc3964112c811e0131bca93a35d17e833ebc221a40bd758ae8328802896e40a54d2be21c7dd8e665f6490abbbdff6c5590bc656c9d0b3dad2a");

		tests[4].in_dk_len = 80;
		tests[4].out_dk = tc::cli::FormatUtil::hexStringToBytes("8c0511f4c6e597c6ac6315d8f0362e225f3c501495ba23b868c005174dc4ee71115b59f9e60cd9532fa33e0f75aefe30225c583a186cd82bd4daea9724a3d3b804f75bdd41494fa324cab24bcc680fb3");

		tests[5].in_dk_len = 16;
		tests[5].out_dk = tc::cli::FormatUtil::hexStringToBytes("9d9e9c4cd21fe4be24d5b8244c759665");
	}
	else
	{
		// no case for provided hash algo
		return;
	}
	
	// copy populated tests to output
	for (size_t i = 0; i < tests.size(); i++)
		test_list.push_back(tests[i]);
}