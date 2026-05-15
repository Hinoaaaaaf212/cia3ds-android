#include <iostream>
#include <brd/es.h>

#ifdef _WIN32
#pragma warning(disable : 4101) // silence warnings for unused local variables
#endif

namespace brd {
	namespace es {

		int dummy_method1a()
		{
			// types
			brd::es::Aes128Key Aes128Key_var;
			brd::es::Aes192Key Aes192Key_var;
			brd::es::Aes256Key Aes256Key_var;
			brd::es::Sha1Hash Sha1Hash_var;
			brd::es::Sha1Hmac Sha1Hmac_var;
			brd::es::Sha256Hash Sha256Hash_var;
			brd::es::Sha256Hmac Sha256Hmac_var;
			brd::es::RsaPublicExponent RsaPublicExponent_var;
			brd::es::Rsa2048Integer Rsa2048Integer_var;
			brd::es::Rsa2048PublicKey Rsa2048PublicKey_var;
			brd::es::Rsa2048PrivateKey Rsa2048PrivateKey_var;
			brd::es::Rsa2048Sig Rsa2048Sig_var;
			return 11;
		}

		int dummy_method1b()
		{
			// types
			brd::es::Rsa4096Integer Rsa4096Integer_var;
			brd::es::Rsa4096PublicKey Rsa4096PublicKey_var;
			brd::es::Rsa4096PrivateKey Rsa4096PrivateKey_var;
			brd::es::Rsa4096Sig Rsa4096Sig_var;
			brd::es::Ecc233Integer Ecc233Integer_var;
			brd::es::Ecc233Point Ecc233Point_var;
			brd::es::Ecc233PrivateKey Ecc233PrivateKey_var;
			brd::es::Ecc233PublicKey Ecc233PublicKey_var;
			brd::es::Ecc233Sig Ecc233Sig_var;

			return 01323;
		}

		int dummy_method2()
		{
			// sign
			brd::es::ESSigType ESSigType_var;
			size_t ES_ISSUER_SIZE_var = brd::es::ES_ISSUER_SIZE;
			brd::es::ESIssuer ESIssuer_var;
			brd::es::ESSigRsa2048 ESSigRsa2048_struct;
			brd::es::ESSigRsa4096 ESSigRsa4096_struct;
			brd::es::ESSigEcc233 ESSigEcc233_struct;

			return 1232;
		}

		int dummy_method3()
		{
			// cert
			brd::es::ESCertPubKeyType ESCertPubKeyType_var;
			size_t ES_CERT_NAME_SIZE_var = brd::es::ES_CERT_NAME_SIZE;
			brd::es::ESCertName ESCertName_var;
			brd::es::ESServerId ESServerId_var;
			brd::es::ESDeviceId ESDeviceId_var;
			brd::es::ESCertHeader ESCertHeader_struct;
			brd::es::ESCertRsa2048PublicKey ESCertRsa2048PublicKey_struct;
			brd::es::ESCertRsa4096PublicKey ESCertRsa4096PublicKey_struct;
			brd::es::ESCertEcc233PublicKey ESCertEcc233PublicKey_struct;
			brd::es::ESRootCert ESRootCert_struct;
			brd::es::ESCACert ESCACert_struct;
			brd::es::ESCASignedCert ESCASignedCert_struct;
			brd::es::ESDeviceCert ESDeviceCert_struct;
			brd::es::ESDeviceSignedCert ESDeviceSignedCert_struct;

			return 55;
		}

		int dummy_method4()
		{
			// tik
			brd::es::ESLicenseType ESLicenseType_var;
			uint8_t ES_LICENSE_MASK_var = brd::es::ES_LICENSE_MASK;
			brd::es::ESLimitCode ESLimitCode_var;
			uint32_t ES_MAX_LIMIT_TYPE_var = brd::es::ES_MAX_LIMIT_TYPE;
			brd::es::ESItemType ESItemType_var;
			brd::es::ESPropertyMaskFlag ESPropertyMaskFlag_var;
			brd::es::ESV1SectionHeaderFlag ESV1SectionHeaderFlag_var;
			brd::es::ESV2TitleKekType ESV2TitleKekType_var;
			brd::es::ESLimitedPlayEntry ESLimitedPlayEntry_struct;
			brd::es::ESSysAccessMask ESSysAccessMask_var;
			brd::es::ESTicketCustomData ESTicketCustomData_var;
			brd::es::ESTicketReserved ESTicketReserved_var;
			brd::es::ESCidxMask ESCidxMask_var;
			brd::es::ESLimitedPlayArray ESLimitedPlayArray_var;
			brd::es::ESReferenceId ESReferenceId_var;
			brd::es::ESV1CidxMask ESV1CidxMask_var;
			brd::es::ESV2TitleKey ESV2TitleKey_var;
			brd::es::ESRightsId ESRightsId_var;
			brd::es::ESV2TicketReserved ESV2TicketReserved_var;
			brd::es::ESTicket ESTicket_struct;
			brd::es::ESV1TicketHeader ESV1TicketHeader_struct;
			brd::es::ESV1SectionHeader ESV1SectionHeader_struct;
			brd::es::ESV1Ticket ESV1Ticket_struct;
			brd::es::ESV1PermanentRecord ESV1PermanentRecord_struct;
			brd::es::ESV1SubscriptionRecord ESV1SubscriptionRecord_struct;
			brd::es::ESV1ContentRecord ESV1ContentRecord_struct;
			brd::es::ESV1ContentConsumptionRecord ESV1ContentConsumptionRecord_struct;
			brd::es::ESV1AccessTitleRecord ESV1AccessTitleRecord_struct;
			brd::es::ESV1LimitedResourceRecord ESV1LimitedResourceRecord_struct;
			brd::es::ESV2Ticket ESV2Ticket_struct;
			brd::es::ESV2SectionHeader ESV2SectionHeader_struct;

			return 0;
		}

		int dummy_method5()
		{
			// tmd
			brd::es::ESTitleType ESTitleType_var;
			brd::es::ESContentType ESContentType_var;
			size_t ES_CONTENT_INDEX_MAX_var = brd::es::ES_CONTENT_INDEX_MAX;
			size_t ES_MAX_CMDS_IN_GROUP_var = brd::es::ES_MAX_CMDS_IN_GROUP;
			brd::es::ESContentMeta ESContentMeta_struct;
			brd::es::ESV1ContentMeta ESV1ContentMeta_struct;
			brd::es::ESTitleMetaHeader ESTitleMetaHeader_struct;
			brd::es::ESV1ContentMetaGroup ESV1ContentMetaGroup_struct;
			brd::es::ESV1TitleMetaHeader ESV1TitleMetaHeader_struct;
			brd::es::ESTitleMeta ESTitleMeta_struct;
			brd::es::ESV1TitleMeta ESV1TitleMeta_struct;
			return 54;
		}

	}
}
#ifdef _WIN32
#pragma warning(default : 4101)
#endif