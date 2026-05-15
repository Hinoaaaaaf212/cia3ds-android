#pragma once
#include "types.h"
#include "KeyBag.h"
#include <tc/Optional.h>
#include <ntd/n3ds/exheader.h>

namespace ctrtool {

class ExHeaderProcess
{
public:
	ExHeaderProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setKeyBag(const ctrtool::KeyBag& key_bag);
	void setCliOutputMode(bool show_info);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);
	void setShowSyscallName(bool show_name);

	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	ctrtool::KeyBag mKeyBag;
	bool mShowInfo;
	bool mVerbose;
	bool mVerify;
	bool mShowSyscallNames;

	ntd::n3ds::ExtendedHeader mHeader;
	ntd::n3ds::AccessDescriptor mDesc;

	byte_t mValidSignature;
	struct ValidARM11SystemLocalCapabilities
	{
		ValidARM11SystemLocalCapabilities()
		{
			system_save_id[0] = ValidState::Unchecked;
			system_save_id[1] = ValidState::Unchecked;
			fs_access = ValidState::Unchecked;
			core_version = ValidState::Unchecked;
			program_id = ValidState::Unchecked;
			priority = ValidState::Unchecked;
			affinity_mask = ValidState::Unchecked;
			ideal_processor = ValidState::Unchecked;
			old3ds_system_mode = ValidState::Unchecked;
			new3ds_system_mode = ValidState::Unchecked;
			enable_l2_cache = ValidState::Unchecked;
			new3ds_cpu_speed = ValidState::Unchecked;
			service_control = ValidState::Unchecked;
		}

		std::array<byte_t, 2> system_save_id;
		byte_t fs_access;
		byte_t core_version;
		byte_t program_id;
		byte_t priority;
		byte_t affinity_mask;
		byte_t ideal_processor;
		byte_t old3ds_system_mode;
		byte_t new3ds_system_mode;
		byte_t enable_l2_cache;
		byte_t new3ds_cpu_speed;
		byte_t service_control;
	} mValidLocalCaps;

	void importExHeader();
	void verifyExHeader();
	void printExHeader();

	void printSystemControlInfo(const ntd::n3ds::SystemControlInfo& info);
	void printARM11SystemLocalCapabilities(const ntd::n3ds::AccessControlInfo& info, const ValidARM11SystemLocalCapabilities& valid);
	void printARM11KernelCapabilities(const ntd::n3ds::AccessControlInfo& info);
	void printARM9AccessControlInfo(const ntd::n3ds::AccessControlInfo& info);

	// string utils
	std::string getValidString(byte_t validstate);
	std::string getSystemModeString(byte_t system_mode);
	std::string getSystemModeExtString(byte_t system_mode_ext, byte_t system_mode);
	std::string getSystemModeAppMemorySizeString(byte_t system_mode);
	std::string getSystemModeExtAppMemorySizeString(byte_t system_mode_ext, byte_t system_mode);
	std::string getFsAccessBitString(size_t bit);
	std::string getMemoryTypeString(byte_t memory_type);
	std::string getByteHexString(byte_t byte);
	std::string getSysCallName(byte_t syscall);
	std::string getArm9CapabilityBitString(size_t bit);

};

}