#include "ExHeaderProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>

ctrtool::ExHeaderProcess::ExHeaderProcess() :
	mModuleLabel("ctrtool::ExHeaderProcess"),
	mInputStream(),
	mKeyBag(),
	mShowInfo(false),
	mVerbose(false),
	mVerify(false),
	mShowSyscallNames(false),
	mValidSignature(ValidState::Unchecked),
	mValidLocalCaps()
{
	memset((byte_t*)&mHeader, 0, sizeof(ntd::n3ds::ExtendedHeader));
	memset((byte_t*)&mDesc, 0, sizeof(ntd::n3ds::AccessDescriptor));
}

void ctrtool::ExHeaderProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::ExHeaderProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::ExHeaderProcess::setCliOutputMode(bool show_info)
{
	mShowInfo = show_info;
}

void ctrtool::ExHeaderProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::ExHeaderProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void ctrtool::ExHeaderProcess::setShowSyscallName(bool show_name)
{
	mShowSyscallNames = show_name;
}

void ctrtool::ExHeaderProcess::process()
{
	// begin processing
	importExHeader();
	if (mVerify)
		verifyExHeader();
	if (mShowInfo)
		printExHeader();
}

void ctrtool::ExHeaderProcess::importExHeader()
{
	// validate input stream
	if (mInputStream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Input stream was null.");
	}
	if (mInputStream->canRead() == false || mInputStream->canSeek() == false)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream requires read/seek permissions.");
	}	

	// import header
	if (mInputStream->length() < (sizeof(ntd::n3ds::ExtendedHeader) + sizeof(ntd::n3ds::AccessDescriptor)))
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream too small.");
	}
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read((byte_t*)&mHeader, sizeof(ntd::n3ds::ExtendedHeader));
	mInputStream->read((byte_t*)&mDesc, sizeof(ntd::n3ds::AccessDescriptor));

}

void ctrtool::ExHeaderProcess::verifyExHeader()
{
	std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> desc_hash;
	
	tc::crypto::GenerateSha256Hash(desc_hash.data(), (byte_t*)&mDesc.ncch_rsa_modulus, sizeof(mDesc) - sizeof(mDesc.signature));

	if (mKeyBag.rsa_key.find(mKeyBag.RSAKEY_ACCESSDESC) != mKeyBag.rsa_key.end())
	{
		tc::crypto::RsaKey pubkey = mKeyBag.rsa_key[mKeyBag.RSAKEY_ACCESSDESC];

		mValidSignature = tc::crypto::VerifyRsa2048Pkcs1Sha256(mDesc.signature.data(), desc_hash.data(), pubkey) ? ValidState::Good : ValidState::Fail;
	}
	else
	{
		fmt::print(stderr, "[{} ERROR] Could not load AccessDescriptor RSA2048 public key.\n", mModuleLabel);
		mValidSignature = ValidState::Fail;
	}

	if (mValidSignature != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] Signature for AccessDescriptor was invalid.\n", mModuleLabel);
	}

	mValidLocalCaps.system_save_id[0] = ValidState::Good;
	mValidLocalCaps.system_save_id[1] = ValidState::Good;
	mValidLocalCaps.fs_access = ValidState::Good;
	mValidLocalCaps.core_version = ValidState::Good;
	mValidLocalCaps.program_id = ValidState::Good;
	mValidLocalCaps.priority = ValidState::Good;
	mValidLocalCaps.affinity_mask = ValidState::Good;
	mValidLocalCaps.ideal_processor = ValidState::Good;
	mValidLocalCaps.old3ds_system_mode = ValidState::Good;
	mValidLocalCaps.new3ds_system_mode = ValidState::Good;
	mValidLocalCaps.enable_l2_cache = ValidState::Good;
	mValidLocalCaps.new3ds_cpu_speed = ValidState::Good;
	mValidLocalCaps.service_control = ValidState::Good;

	byte_t* exhdr_program_id = (byte_t*)&mHeader.access_control_info.program_id;
	byte_t* desc_program_id = (byte_t*)&mDesc.access_control_info.program_id;
	for (size_t i = 0; i < sizeof(uint64_t); i++)
	{
		if (exhdr_program_id[i] == desc_program_id[i] || desc_program_id[i] == 0xFF)
			continue;

		mValidLocalCaps.program_id = ValidState::Fail;
		break;
	}

	/*
	// this does not appear to be correct given working examples: SystemUpdater exhdr: 0x1, desc: 0x2
	auto exhdr_core_version = mHeader.access_control_info.core_version.unwrap();
	auto desc_core_version = mDesc.access_control_info.core_version.unwrap();
	if (exhdr_core_version != desc_core_version)
		mValidLocalCaps.core_version = ValidState::Fail;
	*/

	auto exhdr_thread_priority = mHeader.access_control_info.flags.unwrap().thread_priority;
	auto desc_thread_priority = mDesc.access_control_info.flags.unwrap().thread_priority;
	if (exhdr_thread_priority < desc_thread_priority)
		mValidLocalCaps.priority = ValidState::Fail;

	auto exhdr_ideal_processor = mHeader.access_control_info.flags.unwrap().ideal_processor;
	auto desc_ideal_processor = mDesc.access_control_info.flags.unwrap().ideal_processor;
	if((1<<exhdr_ideal_processor & desc_ideal_processor) == 0)
		mValidLocalCaps.ideal_processor = ValidState::Fail;
	
	auto exhdr_affinity_mask = mHeader.access_control_info.flags.unwrap().affinity_mask;
	auto desc_affinity_mask = mDesc.access_control_info.flags.unwrap().affinity_mask;
	if (exhdr_affinity_mask & ~desc_affinity_mask)
		mValidLocalCaps.affinity_mask = ValidState::Fail;

	auto exhdr_system_mode = mHeader.access_control_info.flags.unwrap().system_mode;
	auto desc_system_mode = mDesc.access_control_info.flags.unwrap().system_mode;
	if (exhdr_system_mode > desc_system_mode)
		mValidLocalCaps.old3ds_system_mode = ValidState::Fail;

	auto exhdr_system_mode_ext = mHeader.access_control_info.flags.unwrap().system_mode_ext;
	auto desc_system_mode_ext = mDesc.access_control_info.flags.unwrap().system_mode_ext;
	if (exhdr_system_mode_ext > desc_system_mode_ext)
		mValidLocalCaps.new3ds_system_mode = ValidState::Fail;

	auto exhdr_enable_l2_cache = mHeader.access_control_info.flags.unwrap().enable_l2_cache;
	auto desc_enable_l2_cache = mDesc.access_control_info.flags.unwrap().enable_l2_cache;
	if (exhdr_enable_l2_cache != desc_enable_l2_cache)
		mValidLocalCaps.enable_l2_cache = ValidState::Fail;

	auto exhdr_cpu_speed = mHeader.access_control_info.flags.unwrap().cpu_speed;
	auto desc_cpu_speed = mDesc.access_control_info.flags.unwrap().cpu_speed;
	if (exhdr_cpu_speed != desc_cpu_speed)
		mValidLocalCaps.new3ds_cpu_speed = ValidState::Fail;


	// Storage Info Verify
	auto exhdr_system_savedata_id = mHeader.access_control_info.system_savedata_id;
	auto desc_system_savedata_id = mDesc.access_control_info.system_savedata_id;
	if(exhdr_system_savedata_id[0].unwrap() & ~desc_system_savedata_id[0].unwrap())
		mValidLocalCaps.system_save_id[0] = ValidState::Fail;
	if(exhdr_system_savedata_id[1].unwrap() & ~desc_system_savedata_id[1].unwrap())
		mValidLocalCaps.system_save_id[1] = ValidState::Fail;

	auto exhdr_fs_access = mHeader.access_control_info.fs_access;
	auto desc_fs_access = mDesc.access_control_info.fs_access;
	for (size_t fs_bit = 0; fs_bit < exhdr_fs_access.bit_size(); fs_bit++)
	{
		if (exhdr_fs_access.test(fs_bit) == true && desc_fs_access.test(fs_bit) == false)
		{
			mValidLocalCaps.fs_access = ValidState::Fail;
			if (mVerbose)
			{
				fmt::print(stderr, "[{} ERROR] FsAccess Bit {:d} was not permitted\n", mModuleLabel, fs_bit);
			}
		}
	}

	// Service Access Control
	auto exhdr_service_access_control = mHeader.access_control_info.service_access_control;
	auto desc_service_access_control = mDesc.access_control_info.service_access_control;
	bool found_service;
	for (size_t i = 0, j; i < exhdr_service_access_control.size(); i++) {
		// skip if empty string
		if (exhdr_service_access_control[i].decode().empty())
			break;

		found_service = false;

		// locate entry in desc
		for (j = 0; j < desc_service_access_control.size(); j++) {
			if (exhdr_service_access_control[i].decode() == desc_service_access_control[j].decode())
				found_service = true;
		}

		if (found_service == false)
		{
			mValidLocalCaps.service_control = Fail;
			if (mVerbose)
			{
				fmt::print(stderr, "[{} ERROR] Service \"{}\" was not permitted\n", mModuleLabel, exhdr_service_access_control[i].decode());
			}
		}
	}

	if (mValidLocalCaps.system_save_id[0] != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "SystemSaveId1");
	}
	if (mValidLocalCaps.system_save_id[1] != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "SystemSaveId2");
	}
	if (mValidLocalCaps.fs_access != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "FsAccess");
	}
	/*
	if (mValidLocalCaps.core_version != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "CoreVersion");
	}
	*/
	if (mValidLocalCaps.program_id != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "ProgramId");
	}
	if (mValidLocalCaps.priority != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "ThreadPriority");
	}
	if (mValidLocalCaps.affinity_mask != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "AffinityMask");
	}
	if (mValidLocalCaps.ideal_processor != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "IdealProcessor");
	}
	if (mValidLocalCaps.old3ds_system_mode != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "SystemMode (Old3DS)");
	}
	if (mValidLocalCaps.new3ds_system_mode != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "SystemMode (New3DS)");
	}
	if (mValidLocalCaps.enable_l2_cache != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "EnableL2Cache");
	}
	if (mValidLocalCaps.new3ds_cpu_speed != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "CpuSpeed");
	}
	if (mValidLocalCaps.service_control != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] {} was not permmited by AccessDescriptor.\n", mModuleLabel, "ServiceAccess");
	}
}

void ctrtool::ExHeaderProcess::printExHeader()
{
	fmt::print("\n");
	fmt::print("Extended header:\n");
	fmt::print("Signature: {:6}       {}", getValidString(mValidSignature), tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mDesc.signature.data(), mDesc.signature.size(), true, "", 0x20, 24, false));
	fmt::print("NCCH Hdr RSA Modulus:   {}", tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mDesc.ncch_rsa_modulus.data(), mDesc.ncch_rsa_modulus.size(), true, "", 0x20, 24, false));
	printSystemControlInfo(mHeader.system_control_info);
	printARM11SystemLocalCapabilities(mHeader.access_control_info, mValidLocalCaps);
	printARM11KernelCapabilities(mHeader.access_control_info);
	printARM9AccessControlInfo(mHeader.access_control_info);
}

void ctrtool::ExHeaderProcess::printSystemControlInfo(const ntd::n3ds::SystemControlInfo& info)
{
	//fmt::print("[SystemControlInfo]\n");
	// basic info
	fmt::print("Name:                   {}\n", info.name.decode());
	fmt::print("Flags:                  {:02X}", (uint32_t)info.flags.raw);
	if (info.flags.bitarray.test(info.Flags_CompressExefsPartition0))
		fmt::print(" [compressed]");
	if (info.flags.bitarray.test(info.Flags_SdmcApplication))
		fmt::print(" [sd app]");
	fmt::print("\n");
	fmt::print("Remaster version:       {:04x}\n", (uint32_t)info.remaster_version.unwrap());

	// code set info
	fmt::print("Code text address:      0x{:08X}\n", info.text.address.unwrap());
	fmt::print("Code text size:         0x{:08X}\n", info.text.code_size.unwrap());
	fmt::print("Code text max pages:    0x{:08X} (0x{:08X})\n", info.text.num_max_pages.unwrap(), info.text.num_max_pages.unwrap() * 0x1000);

	fmt::print("Code ro address:        0x{:08X}\n", info.rodata.address.unwrap());
	fmt::print("Code ro size:           0x{:08X}\n", info.rodata.code_size.unwrap());
	fmt::print("Code ro max pages:      0x{:08X} (0x{:08X})\n", info.rodata.num_max_pages.unwrap(), info.rodata.num_max_pages.unwrap() * 0x1000);

	fmt::print("Code data address:      0x{:08X}\n", info.data.address.unwrap());
	fmt::print("Code data size:         0x{:08X}\n", info.data.code_size.unwrap());
	fmt::print("Code data max pages:    0x{:08X} (0x{:08X})\n", info.data.num_max_pages.unwrap(), info.data.num_max_pages.unwrap() * 0x1000);

	fmt::print("Code bss size:          0x{:08X}\n", info.bss_size.unwrap());
	fmt::print("Code stack size:        0x{:08X}\n", info.stack_size.unwrap());

	// Dependency
	for (size_t i = 0; i < info.dependency_list.size(); i++)
	{
		if (info.dependency_list[i].unwrap() != 0)
		{
			fmt::print("Dependency:             {:016x}\n", info.dependency_list[i].unwrap());
		}
	}

	// savedata size
	fmt::print("Savedata size:          ");
	uint64_t savedata_size = info.savedata_size.unwrap();
	if (savedata_size < (1024)) // < KB
	{
		fmt::print("0x{:x}", savedata_size);
	}
	else if (savedata_size < (1024 * 1024)) // < MB
	{
		fmt::print("{:d}K", (savedata_size >> 10));
	}
	else
	{
		fmt::print("{:d}M", (savedata_size >> 20));
	}
	fmt::print("\n");

	fmt::print("Jump id:                {:016x}\n", info.jump_id.unwrap());
}

void ctrtool::ExHeaderProcess::printARM11SystemLocalCapabilities(const ntd::n3ds::AccessControlInfo& info, const ValidARM11SystemLocalCapabilities& valid)
{
	//fmt::print("[ARM11SystemLocalCapabilities]\n");
	fmt::print("Program id:             {:016x} {}\n", info.program_id.unwrap(), getValidString(valid.program_id));
	fmt::print("Core version:           0x{:08x}\n", info.core_version.unwrap());
	fmt::print("System mode:            {} (AppMemory: {}) {}\n", getSystemModeString(info.flags.unwrap().system_mode), getSystemModeAppMemorySizeString(info.flags.unwrap().system_mode), getValidString(valid.old3ds_system_mode));
	fmt::print("System mode (New3DS):   {} (AppMemory: {}) {}\n", getSystemModeExtString(info.flags.unwrap().system_mode_ext, info.flags.unwrap().system_mode), getSystemModeExtAppMemorySizeString(info.flags.unwrap().system_mode_ext, info.flags.unwrap().system_mode), getValidString(valid.new3ds_system_mode));
	fmt::print("CPU Speed (New3DS):     {} {}\n", (info.flags.unwrap().cpu_speed ? "804MHz" : "268MHz"), getValidString(valid.new3ds_cpu_speed));
	fmt::print("Enable L2 Cache:        {} {}\n", (info.flags.unwrap().enable_l2_cache ? "YES" : "NO"), getValidString(valid.enable_l2_cache));
	fmt::print("Ideal processor:        {:d} {}\n", (uint32_t)info.flags.unwrap().ideal_processor, getValidString(valid.ideal_processor));
	fmt::print("Affinity mask:          {:d} {}\n", (uint32_t)info.flags.unwrap().affinity_mask, getValidString(valid.affinity_mask));
	fmt::print("Main thread priority:   {:d} {}\n", (uint32_t)info.flags.unwrap().thread_priority, getValidString(valid.priority));
	fmt::print("MaxCpu:                 {:d}\n", info.resource_limit_descriptor[info.ResourceLimitDescriptorIndex_MaxCpu].unwrap());

	std::vector<uint32_t> accessible_save_ids;
	uint64_t ext_savedata_id = 0;
	std::array<uint32_t, 3> other_user_save_ids = {0, 0, 0};
	bool use_other_variation_savedata = false;
	if (info.other_attributes.test(info.OtherAttribute_UseExtendedSavedataAccessControl))
	{
		uint32_t id;

		if (0 != (id = info.accessible_unique_ids_0.unwrap().save_id0))
			accessible_save_ids.push_back(id);
		
		if (0 != (id = info.accessible_unique_ids_0.unwrap().save_id1))
			accessible_save_ids.push_back(id);
		
		if (0 != (id = info.accessible_unique_ids_0.unwrap().save_id2))
			accessible_save_ids.push_back(id);
		
		if (0 != (id = info.accessible_unique_ids_1.unwrap().save_id0))
			accessible_save_ids.push_back(id);
		
		if (0 != (id = info.accessible_unique_ids_1.unwrap().save_id1))
			accessible_save_ids.push_back(id);

		if (0 != (id = info.accessible_unique_ids_1.unwrap().save_id2))
			accessible_save_ids.push_back(id);
	}
	else
	{
		ext_savedata_id = info.ext_savedata_id.unwrap();

		other_user_save_ids[0] = info.accessible_unique_ids_0.unwrap().save_id0;
		other_user_save_ids[1] = info.accessible_unique_ids_0.unwrap().save_id1;
		other_user_save_ids[2] = info.accessible_unique_ids_0.unwrap().save_id2;
	}
	use_other_variation_savedata = info.accessible_unique_ids_0.unwrap().flag;

	fmt::print("Ext savedata id:        0x{:016x}\n", ext_savedata_id);
	for (size_t i = 0; i < info.system_savedata_id.size(); i++)
	{
		fmt::print("System savedata id {:d}:   0x{:08x} {}\n", i+1, info.system_savedata_id[i].unwrap(), getValidString(valid.system_save_id[i]));
	}
	for (size_t i = 0; i < other_user_save_ids.size(); i++)
	{
		fmt::print("OtherUserSaveDataId{:d}:   0x{:05x}\n", i+1, other_user_save_ids[i]);
	}
	fmt::print("Accessible Savedata Ids: {}\n", (accessible_save_ids.size() == 0 ? "None" : ""));
	for (size_t i = 0; i < accessible_save_ids.size(); i++)
	{
		fmt::print(" > 0x{:05x}\n", accessible_save_ids[i]);
	}
	fmt::print("Other Variation Saves:  {}\n", (use_other_variation_savedata ? "Accessible" : "Inaccessible"));
	uint64_t fs_access_raw = ((((tc::bn::le64<uint64_t>*)&info.fs_access)->unwrap() << 8) >> 8); // clearing the upper 8 bits since fs_access is 56 bits
	fmt::print("FS access: {:6}       0x{:014x}\n", getValidString(valid.fs_access), fs_access_raw);
	for (size_t i = 0; i < info.fs_access.bit_size(); i++)
	{
		if (info.fs_access.test(i))
		{
			fmt::print(" > {}\n", getFsAccessBitString(i));
		}
	}

	fmt::print("Service access: {}\n", getValidString(mValidLocalCaps.service_control));
	auto& service_access = info.service_access_control;
	for (size_t i = 0; i < service_access.size(); i++)
	{
		if (service_access[i].decode().empty())
			break;

		fmt::print(" > {}\n", service_access[i].decode());
	}
	fmt::print("Reslimit category:      {:02X}\n", (uint32_t)info.resource_limit_category);
}

void ctrtool::ExHeaderProcess::printARM11KernelCapabilities(const ntd::n3ds::AccessControlInfo& info)
{
	size_t i, j;
	std::vector<byte_t> syscall_list;
	std::vector<byte_t> interrupt_list;
	std::vector<uint32_t> unknown_desc_list;

	union KernDesc
	{
		uint32_t raw;
		tc::bn::bitarray<4> bits;
		ntd::n3ds::AccessControlInfo::InterruptDescriptor interrupt;
		ntd::n3ds::AccessControlInfo::SystemCallDescriptor syscall;
		ntd::n3ds::AccessControlInfo::ReleaseKernelVersionDescriptor kernel_ver;
		ntd::n3ds::AccessControlInfo::HandleTableSizeDescriptor handle_table;
		ntd::n3ds::AccessControlInfo::OtherCapabilitiesDescriptor other_cap;
		ntd::n3ds::AccessControlInfo::MappingStaticDescriptor mapping_static;
		ntd::n3ds::AccessControlInfo::MappingIODescriptor mapping_io;
	};

	KernDesc prev_desc;
	prev_desc.raw = 0;

	for (i = 0; i < info.kernel_descriptors.size(); i++)
	{
		KernDesc desc;
		desc.raw = info.kernel_descriptors[i].unwrap();
		
		uint32_t prefix_bits;
		for (prefix_bits = 0; prefix_bits < 32; prefix_bits++)
		{
			if (desc.bits.test(31 - prefix_bits) == false)
			{
				break;
			}
		}

		if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_InterruptNumList)
		{
			if (desc.interrupt.interrupt_0 != 0)
				interrupt_list.push_back(desc.interrupt.interrupt_0);
			if (desc.interrupt.interrupt_1 != 0)
				interrupt_list.push_back(desc.interrupt.interrupt_1);
			if (desc.interrupt.interrupt_2 != 0)
				interrupt_list.push_back(desc.interrupt.interrupt_2);
			if (desc.interrupt.interrupt_3 != 0)
				interrupt_list.push_back(desc.interrupt.interrupt_3);
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_SysCallList)
		{
			for (j = 0; j < 24; j++)
			{
				if ((desc.syscall.systemcall_lower_bitarray >> j) & 1)
					syscall_list.push_back(byte_t(desc.syscall.systemcall_upper * 24) + byte_t(j));
			}
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_ReleaseKernelVersion)
		{
			fmt::print("Kernel release version: {:d}.{:d}\n", ((desc.kernel_ver.version >> 8) & 0xFF), ((desc.kernel_ver.version >> 0) & 0xFF));
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_HandleTableSize)
		{
			fmt::print("Handle table size:      0x{:X}\n", (uint32_t)desc.handle_table.size);
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_OtherCapabilities)
		{
			fmt::print("Kernel flags:           \n");
			fmt::print(" > Allow debug:         {}\n", (desc.other_cap.permit_debug ? "YES" : "NO"));
			fmt::print(" > Force debug:         {}\n", (desc.other_cap.force_debug ? "YES" : "NO"));
			fmt::print(" > Allow non-alphanum:  {}\n", (desc.other_cap.can_use_non_alphabet_and_number ? "YES" : "NO"));
			fmt::print(" > Shared page writing: {}\n", (desc.other_cap.can_write_shared_page ? "YES" : "NO"));
			fmt::print(" > Privilege priority:  {}\n", (desc.other_cap.can_use_privileged_priority ? "YES" : "NO"));
			fmt::print(" > Allow main() args:   {}\n", (desc.other_cap.permit_main_function_argument ? "YES" : "NO"));
			fmt::print(" > Shared device mem:   {}\n", (desc.other_cap.can_share_device_memory ? "YES" : "NO"));
			fmt::print(" > Memory Type:         {}\n", getMemoryTypeString(desc.other_cap.memory_type));
			fmt::print(" > Runnable on sleep:   {}\n", (desc.other_cap.runnable_on_sleep ? "YES" : "NO"));
			fmt::print(" > Special memory:      {}\n", (desc.other_cap.special_memory_layout ? "YES" : "NO"));
			fmt::print(" > Access Core 2:       {}\n", (desc.other_cap.can_access_core2 ? "YES" : "NO"));
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_MappingStatic)
		{
			if (prev_desc.raw == 0)
			{
				prev_desc.raw = desc.raw;
			}
			else
			{
				fmt::print("{:24}0x{:X}-0x{:X}{}\n", 
					(desc.mapping_static.flag ? "StaticMapping:" : "IoMapping:"),
					(prev_desc.mapping_static.page << 12),
					(desc.mapping_static.page << 12)-1,
					(prev_desc.mapping_static.flag ? ":r" : "")
				);
				prev_desc.raw = 0;
			}
		}
		else if (prefix_bits == ntd::n3ds::AccessControlInfo::DescriptorPrefix_MappingIo)
		{
			fmt::print("IoMapping:              0x{:X}\n", (desc.mapping_io.page << 12));
		}
		else if (prefix_bits == 32)
		{
			continue;
		}
		else
		{
			unknown_desc_list.push_back(desc.raw);
		}
	}

	fmt::print("Allowed systemcalls:    ");
	if (syscall_list.size() > 0)
	{
		if (!mShowSyscallNames)
		{
			std::vector<std::string> string_list;
			for (size_t i = 0; i < syscall_list.size(); i++)
			{
				string_list.push_back(getByteHexString(syscall_list[i]));
			}

			fmt::print("{}", tc::cli::FormatUtil::formatListWithLineLimit(string_list, 46, 24, false));
		}
		else
		{
			fmt::print("\n");
			for (size_t i = 0; i < syscall_list.size(); i++)
			{
				fmt::print(" > {} {}\n", getByteHexString(syscall_list[i]), getSysCallName(syscall_list[i]));
			}
		}
		
	}
	else
	{
		fmt::print("none\n");
	}

	fmt::print("Allowed interrupts:     ");
	if (interrupt_list.size() > 0)
	{
		std::vector<std::string> string_list;
		for (size_t i = 0; i < interrupt_list.size(); i++)
		{
			string_list.push_back(getByteHexString(interrupt_list[i]));
		}

		fmt::print("{}", tc::cli::FormatUtil::formatListWithLineLimit(string_list, 46, 24, false));
	}
	else
	{
		fmt::print("none\n");
	}

	for (i = 0; i < unknown_desc_list.size(); i++)
	{
		fmt::print("Unknown descriptor:     {:08X}\n", unknown_desc_list[i]);
	}
}

void ctrtool::ExHeaderProcess::printARM9AccessControlInfo(const ntd::n3ds::AccessControlInfo& info)
{
	//fmt::print("[ARM9AccessControlInfo]\n");
	
	// collect arm9 caps as a vector of strings
	std::vector<std::string> arm9_caps_str;
	for (size_t i = 0; i < info.arm9_access_control.bit_size(); i++)
	{
		if (info.arm9_access_control.test(i))
		{
			arm9_caps_str.push_back(getArm9CapabilityBitString(i));
		}
	}
	
	// print arm9 caps
	fmt::print("Arm9Capability:         {}\n", (arm9_caps_str.size() == 0 ? "none" : ""));
	for (size_t i = 0; i < arm9_caps_str.size(); i++)
	{
		fmt::print(" > {}\n", arm9_caps_str[i]);
	}
	
	// print desc version
	fmt::print("Desc Version:           0x{:x}\n", (uint32_t)info.desc_version);
}

std::string ctrtool::ExHeaderProcess::getValidString(byte_t validstate)
{
	std::string ret_str;

	switch (validstate)
	{
		case ValidState::Unchecked:
			ret_str =  "";
			break;
		case ValidState::Good:
			ret_str =  "(GOOD)";
			break;
		case ValidState::Fail:
		default:
			ret_str =  "(FAIL)";
			break;
	}

	return ret_str;
}

std::string ctrtool::ExHeaderProcess::getSystemModeString(byte_t system_mode)
{
	std::string str;

	switch (system_mode)
	{
		case ntd::n3ds::AccessControlInfo::SystemMode_PROD :
			str = "prod";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV1 :
			str = "dev1";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV2 :
			str = "dev2";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV3 :
			str = "dev3";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV4 :
			str = "dev4";
			break;
		default:
			str = fmt::format("Unknown (0x{:x})", (uint32_t)system_mode);
			break;
	}

	return str;
}

std::string ctrtool::ExHeaderProcess::getSystemModeExtString(byte_t system_mode_ext, byte_t system_mode)
{
	std::string str;

	switch (system_mode_ext)
	{
		case ntd::n3ds::AccessControlInfo::SystemModeExt_LEGACY :
			//str = "Legacy";
			str = fmt::format("ctr {}", getSystemModeString(system_mode));
			break;
		case ntd::n3ds::AccessControlInfo::SystemModeExt_PROD :
			str = "snake prod";
			break;
		case ntd::n3ds::AccessControlInfo::SystemModeExt_DEV1 :
			str = "snake dev1";
			break;
		default:
			str = fmt::format("Unknown (0x{:x})", (uint32_t)system_mode_ext);
			break;
	}

	return str;
}

std::string ctrtool::ExHeaderProcess::getSystemModeAppMemorySizeString(byte_t system_mode)
{
	std::string str;

	switch (system_mode)
	{
		case ntd::n3ds::AccessControlInfo::SystemMode_PROD :
			str = "64MB";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV1 :
			str = "96MB";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV2 :
			str = "80MB";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV3 :
			str = "72MB";
			break;
		case ntd::n3ds::AccessControlInfo::SystemMode_DEV4 :
			str = "32MB";
			break;
		default:
			str = "Unknown";
			break;
	}

	return str;
}

std::string ctrtool::ExHeaderProcess::getSystemModeExtAppMemorySizeString(byte_t system_mode_ext, byte_t system_mode)
{
	std::string str;

	switch (system_mode_ext)
	{
		case ntd::n3ds::AccessControlInfo::SystemModeExt_LEGACY :
			str = getSystemModeAppMemorySizeString(system_mode);
			break;
		case ntd::n3ds::AccessControlInfo::SystemModeExt_PROD :
			str = "124MB";
			break;
		case ntd::n3ds::AccessControlInfo::SystemModeExt_DEV1 :
			str = "178MB";
			break;
		default:
			str = "Unknown";
			break;
	}

	return str;
}

std::string ctrtool::ExHeaderProcess::getFsAccessBitString(size_t bit)
{
	std::string str;

	switch(bit)
	{
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CategorySystemApplication : 
			str = "Category System Application";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CategoryHardwareCheck : 
			str = "Category Hardware Check";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CategoryFileSystemTool : 
			str = "Category File System Tool";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_Debug : 
			str = "Debug";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_TwlCardBackup : 
			str = "TWL Card Backup";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_TwlNandData : 
			str = "TWL Nand Data";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_Boss : 
			str = "BOSS";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_DirectSdmc : 
			str = "Direct SDMC";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_Core : 
			str = "Core";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CtrNandRo : 
			str = "CTR NAND RO";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CtrNandRw : 
			str = "CTR NAND RW";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CtrNandRoWrite : 
			str = "CTR NAND RO (Write Access)";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CategorySystemSettings : 
			str = "Category System Settings";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CardBoard : 
			str = "CARD BOARD";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_ExportImportIvs : 
			str = "Export Import IVS";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_DirectSdmcWrite : 
			str = "Direct SDMC (Write Only)";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_SwitchCleanup : 
			str = "Switch Cleanup";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_SaveDataMove : 
			str = "Save Data Move";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_Shop : 
			str = "Shop";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_Shell : 
			str = "Shell";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_CategoryHomeMenu : 
			str = "Category HomeMenu";
			break;
		case ntd::n3ds::AccessControlInfo::FileSystemAccess_ExternalSeed : 
			str = "External Seed (Seed DB)";
			break;
		default : 
			str = fmt::format("Bit {:d} (unknown)", bit);
			break;
	}
	
	return str;
}

std::string ctrtool::ExHeaderProcess::getMemoryTypeString(byte_t memory_type)
{
	std::string str;

	switch(memory_type)
	{
		case ntd::n3ds::AccessControlInfo::MemoryType_Application : 
			str = "APPLICATION";
			break;
		case ntd::n3ds::AccessControlInfo::MemoryType_System : 
			str = "SYSTEM";
			break;
		case ntd::n3ds::AccessControlInfo::MemoryType_Base : 
			str = "BASE";
			break;
		default : 
			str = fmt::format("Unknown ({:d})", memory_type);
			break;
	}
	
	return str;
}

std::string ctrtool::ExHeaderProcess::getByteHexString(byte_t byte)
{
	return fmt::format("{:02X}", byte);
}

std::string ctrtool::ExHeaderProcess::getSysCallName(byte_t syscall)
{
	// List of 3DS system calls.  NULL indicates unknown.
	static const size_t kSysCallNum = 128;
	static const char *const kSysCallList[kSysCallNum] =
	{
		NULL,                                // 00
		"ControlMemory",                     // 01
		"QueryMemory",                       // 02
		"ExitProcess",                       // 03
		"GetProcessAffinityMask",            // 04
		"SetProcessAffinityMask",            // 05
		"GetProcessIdealProcessor",          // 06
		"SetProcessIdealProcessor",          // 07
		"CreateThread",                      // 08
		"ExitThread",                        // 09
		"SleepThread",                       // 0A
		"GetThreadPriority",                 // 0B
		"SetThreadPriority",                 // 0C
		"GetThreadAffinityMask",             // 0D
		"SetThreadAffinityMask",             // 0E
		"GetThreadIdealProcessor",           // 0F
		"SetThreadIdealProcessor",           // 10
		"GetCurrentProcessorNumber",         // 11
		"Run",                               // 12
		"CreateMutex",                       // 13
		"ReleaseMutex",                      // 14
		"CreateSemaphore",                   // 15
		"ReleaseSemaphore",                  // 16
		"CreateEvent",                       // 17
		"SignalEvent",                       // 18
		"ClearEvent",                        // 19
		"CreateTimer",                       // 1A
		"SetTimer",                          // 1B
		"CancelTimer",                       // 1C
		"ClearTimer",                        // 1D
		"CreateMemoryBlock",                 // 1E
		"MapMemoryBlock",                    // 1F
		"UnmapMemoryBlock",                  // 20
		"CreateAddressArbiter",              // 21
		"ArbitrateAddress",                  // 22
		"CloseHandle",                       // 23
		"WaitSynchronization1",              // 24
		"WaitSynchronizationN",              // 25
		"SignalAndWait",                     // 26
		"DuplicateHandle",                   // 27
		"GetSystemTick",                     // 28
		"GetHandleInfo",                     // 29
		"GetSystemInfo",                     // 2A
		"GetProcessInfo",                    // 2B
		"GetThreadInfo",                     // 2C
		"ConnectToPort",                     // 2D
		"SendSyncRequest1",                  // 2E
		"SendSyncRequest2",                  // 2F
		"SendSyncRequest3",                  // 30
		"SendSyncRequest4",                  // 31
		"SendSyncRequest",                   // 32
		"OpenProcess",                       // 33
		"OpenThread",                        // 34
		"GetProcessId",                      // 35
		"GetProcessIdOfThread",              // 36
		"GetThreadId",                       // 37
		"GetResourceLimit",                  // 38
		"GetResourceLimitLimitValues",       // 39
		"GetResourceLimitCurrentValues",     // 3A
		"GetThreadContext",                  // 3B
		"Break",                             // 3C
		"OutputDebugString",                 // 3D
		"ControlPerformanceCounter",         // 3E
		NULL,                                // 3F
		NULL,                                // 40
		NULL,                                // 41
		NULL,                                // 42
		NULL,                                // 43
		NULL,                                // 44
		NULL,                                // 45
		NULL,                                // 46
		"CreatePort",                        // 47
		"CreateSessionToPort",               // 48
		"CreateSession",                     // 49
		"AcceptSession",                     // 4A
		"ReplyAndReceive1",                  // 4B
		"ReplyAndReceive2",                  // 4C
		"ReplyAndReceive3",                  // 4D
		"ReplyAndReceive4",                  // 4E
		"ReplyAndReceive",                   // 4F
		"BindInterrupt",                     // 50
		"UnbindInterrupt",                   // 51
		"InvalidateProcessDataCache",        // 52
		"StoreProcessDataCache",             // 53
		"FlushProcessDataCache",             // 54
		"StartInterProcessDma",              // 55
		"StopDma",                           // 56
		"GetDmaState",                       // 57
		"RestartDma",                        // 58
		"SetGpuProt",                        // 59
		"SetWifiEnabled",                    // 5A
		NULL,                                // 5B
		NULL,                                // 5C
		NULL,                                // 5D
		NULL,                                // 5E
		NULL,                                // 5F
		"DebugActiveProcess",                // 60
		"BreakDebugProcess",                 // 61
		"TerminateDebugProcess",             // 62
		"GetProcessDebugEvent",              // 63
		"ContinueDebugEvent",                // 64
		"GetProcessList",                    // 65
		"GetThreadList",                     // 66
		"GetDebugThreadContext",             // 67
		"SetDebugThreadContext",             // 68
		"QueryDebugProcessMemory",           // 69
		"ReadProcessMemory",                 // 6A
		"WriteProcessMemory",                // 6B
		"SetHardwareBreakPoint",             // 6C
		"GetDebugThreadParam",               // 6D
		NULL,                                // 6E
		NULL,                                // 6F
		"ControlProcessMemory",              // 70
		"MapProcessMemory",                  // 71
		"UnmapProcessMemory",                // 72
		"CreateCodeSet",                     // 73
		NULL,                                // 74
		"CreateProcess",                     // 75
		"TerminateProcess",                  // 76
		"SetProcessResourceLimits",          // 77
		"CreateResourceLimit",               // 78
		"SetResourceLimitValues",            // 79
		"AddCodeSegment",                    // 7A
		"Backdoor",                          // 7B
		"KernelSetState",                    // 7C
		"QueryProcessMemory",                // 7D
		NULL,                                // 7E
		NULL,                                // 7F
	};

	std::string str;

	if (syscall >= kSysCallNum)
		return std::string();

	if (kSysCallList[syscall] != nullptr)
	{
		str = kSysCallList[syscall];
	}
	else
	{
		str = fmt::format("Unknown {:02X}", syscall);;
	}

	return str;
}

std::string ctrtool::ExHeaderProcess::getArm9CapabilityBitString(size_t bit)
{
	std::string str;

	switch(bit)
	{
		case ntd::n3ds::AccessControlInfo::Arm9Capability_FsMountNand : 
			str = "FsMountNand";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_FsMountNandRoWrite : 
			str = "FsMountNandRoWrite";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_FsMountTwln : 
			str = "FsMountTwln";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_FsMountWnand : 
			str = "FsMountWnand";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_FsMountCardSpi : 
			str = "FsMountCardSpi";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_UseSdif3 : 
			str = "UseSdif3";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_CreateSeed : 
			str = "CreateSeed";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_UseCardSpi : 
			str = "UseCardSpi";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_SdApplication : 
			str = "SdApplication";
			break;
		case ntd::n3ds::AccessControlInfo::Arm9Capability_UseDirectSdmc : 
			str = "UseDirectSdmc";
			break;
		default : 
			str = fmt::format("Bit {:d} (unknown)", bit);
			break;
	}
	
	return str;
}