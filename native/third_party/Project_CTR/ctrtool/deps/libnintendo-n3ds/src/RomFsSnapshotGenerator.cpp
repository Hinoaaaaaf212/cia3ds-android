#include <ntd/n3ds/RomFsSnapshotGenerator.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <tc/cli.h>
#include <tc/io.h>
#include <tc/string.h>

ntd::n3ds::RomFsSnapshotGenerator::RomFsSnapshotGenerator(const std::shared_ptr<tc::io::IStream>& stream) :
	FileSystemSnapshot(),
	mBaseStream(stream),
	mDataOffset(0),
	mDirEntryTable(),
	mDirParentVaddrMap(),
	mFileEntryTable()
{
	//std::cout << "RomFsSnapshotGenerator begin" << std::endl;

	// validate stream properties
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException("ntd::n3ds::RomFsSnapshotGenerator", "Failed to open input stream.");
	}
	if (mBaseStream->canRead() == false || mBaseStream->canSeek() == false)
	{
		throw tc::NotSupportedException("ntd::n3ds::RomFsSnapshotGenerator", "Input stream requires read/seek permissions.");
	}

	//std::cout << "pos() -> " << mBaseStream->position() << std::endl;

	// validate and read ROMFS header
	ntd::n3ds::RomFsHeader hdr;
	if (mBaseStream->length() < sizeof(ntd::n3ds::RomFsHeader))
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::RomFsSnapshotGenerator", "Input stream is too small.");
	}
	mBaseStream->seek(0, tc::io::SeekOrigin::Begin);
	mBaseStream->read((byte_t*)(&hdr), sizeof(ntd::n3ds::RomFsHeader));

	/*
	std::cout << "hdr.header_size             : " << hdr.header_size.unwrap() << std::endl;
	std::cout << "sizeof(ntd::n3ds::RomFsHeader)  : " << sizeof(ntd::n3ds::RomFsHeader) << std::endl;
	std::cout << "hdr.dir_hash_bucket.offset : " << hdr.dir_hash_bucket.offset.unwrap() << std::endl;
	std::cout << "hdr.data_offset             : " << hdr.data_offset.unwrap() << std::endl;
	std::cout << "expected data offset        : " << align<uint32_t>(hdr.file_entry.offset.unwrap() + hdr.file_entry.size.unwrap(), ntd::n3ds::RomFsHeader::kRomFsDataAlignSize) << std::endl;
	*/


	if (hdr.header_size.unwrap() != sizeof(ntd::n3ds::RomFsHeader) ||
	    hdr.dir_hash_bucket.offset.unwrap() != sizeof(ntd::n3ds::RomFsHeader) ||
	    hdr.data_offset.unwrap() != align<uint32_t>(hdr.file_entry.offset.unwrap() + hdr.file_entry.size.unwrap(), ntd::n3ds::RomFsHeader::kRomFsDataAlignSize))
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::RomFsSnapshotGenerator", "RomFsHeader is corrupted.");
	}

	// save data offset
	mDataOffset = hdr.data_offset.unwrap();

	// get dir entry ptr
	mDirEntryTable = tc::ByteData(hdr.dir_entry.size.unwrap());
	mBaseStream->seek(hdr.dir_entry.offset.unwrap(), tc::io::SeekOrigin::Begin);
	mBaseStream->read(mDirEntryTable.data(), mDirEntryTable.size());
	

	// get file entry ptr
	mFileEntryTable = tc::ByteData(hdr.file_entry.size.unwrap());
	mBaseStream->seek(hdr.file_entry.offset.unwrap(), tc::io::SeekOrigin::Begin);
	mBaseStream->read(mFileEntryTable.data(), mFileEntryTable.size());

	//std::cout << "DirTable:" << std::endl;
	//std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(mDirEntryTable.data(), mDirEntryTable.size());

	/*
	for (uint32_t v_addr = 0; v_addr < mDirEntryTable.size();)
	{
		std::cout << "Dir:            0x" << std::hex << v_addr << std::endl;
		std::cout << " > parent:       0x" << std::hex << getDirEntry(v_addr)->parent_offset.unwrap() << std::endl;
		std::cout << " > sibling:      0x" << std::hex << getDirEntry(v_addr)->sibling_offset.unwrap() << std::endl;
		std::cout << " > child_offset: 0x" << std::hex << getDirEntry(v_addr)->child_offset.unwrap() << std::endl;
		std::cout << " > file_offset:  0x" << std::hex << getDirEntry(v_addr)->file_offset.unwrap() << std::endl;
		std::cout << " > hash_sibling: 0x" << std::hex << getDirEntry(v_addr)->hash_sibling_offset.unwrap() << std::endl;
		std::cout << " > name_size:    0x" << std::hex << getDirEntry(v_addr)->name_size.unwrap() << std::endl;

		uint32_t total_size = sizeof(ntd::n3ds::RomFsDirectoryEntry) + align<uint32_t>(getDirEntry(v_addr)->name_size.unwrap(), 4);
		std::cout << " > entry_size:   0x" << std::hex << total_size << std::endl;

		if (getDirEntry(v_addr)->sibling_offset.unwrap() < v_addr)
		{
			std::cout << "DirEntry looks sus" << std::endl;
			break;
		}

		v_addr += total_size;
	}
	*/

	//std::cout << "FileTable:" << std::endl;
	//std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(mFileEntryTable.data(), mFileEntryTable.size());
	/*
	for (uint32_t v_addr = 0; v_addr < mFileEntryTable.size();)
	{
		std::cout << "File:            0x" << std::hex << v_addr << std::endl;
		std::cout << " > parent:       0x" << std::hex << getFileEntry(v_addr)->parent_offset.unwrap() << std::endl;
		std::cout << " > sibling:      0x" << std::hex << getFileEntry(v_addr)->sibling_offset.unwrap() << std::endl;
		std::cout << " > data_offset:  0x" << std::hex << getFileEntry(v_addr)->data_offset.unwrap() << std::endl;
		std::cout << " > data_size:    0x" << std::hex << getFileEntry(v_addr)->data_size.unwrap() << std::endl;
		std::cout << " > hash_sibling: 0x" << std::hex << getFileEntry(v_addr)->hash_sibling_offset.unwrap() << std::endl;
		std::cout << " > name_size:    0x" << std::hex << getFileEntry(v_addr)->name_size.unwrap() << std::endl;

		uint32_t total_size = sizeof(ntd::n3ds::RomFsFileEntry) + align<uint32_t>(getFileEntry(v_addr)->name_size.unwrap(), 4);
		std::cout << " > entry_size:   0x" << std::hex << total_size << std::endl;

		if (getFileEntry(v_addr)->sibling_offset.unwrap() < v_addr)
		{
			std::cout << "FileEntry looks sus" << std::endl;
			break;
		}

		v_addr += total_size;
	}
	*/

	if (getDirEntry(0)->parent_offset.unwrap() != 0 ||
	    getDirEntry(0)->sibling_offset.unwrap() != 0xffffffff ||
	    getDirEntry(0)->name_size.unwrap() != 0)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::RomFsSnapshotGenerator", "Root RomFsDirectoryEntry corrupted.");
	}

	// add/index directories
	DirEntry dir_tmp;
	for (uint32_t v_addr = 0; v_addr < mDirEntryTable.size();)
	{
		// create root entry
		if (v_addr == 0)
		{
			// create dir path
			tc::io::Path dir_path = tc::io::Path("/");
			dir_tmp.dir_listing.abs_path = dir_path;

			// add dir entry to list
			dir_entries.push_back(dir_tmp);

			// add dir entry to map
			dir_entry_path_map[dir_path] = dir_entries.size() - 1;
			mDirParentVaddrMap[v_addr] = dir_entries.size() - 1;
		}
		// else create a regular entry
		else
		{
			// check parent is in map
			if (mDirParentVaddrMap.find(getDirEntry(v_addr)->parent_offset.unwrap()) == mDirParentVaddrMap.end())
				throw tc::InvalidOperationException("ntd::n3ds::RomFsSnapshotGenerator", "Directory had invalid parent");

			// save/transcode file name
			std::u16string utf16_string;
			std::string utf8_string;
			size_t str_len = getDirEntry(v_addr)->name_size.unwrap() / sizeof(uint16_t);
			for (size_t i = 0; i < str_len; i++)
			{
				utf16_string.push_back(getDirEntry(v_addr)->name[i].unwrap());
			}
			tc::string::TranscodeUtil::UTF16ToUTF8(utf16_string, utf8_string);

			// create dir path
			size_t parent_index = mDirParentVaddrMap[getDirEntry(v_addr)->parent_offset.unwrap()];
			tc::io::Path dir_path = dir_entries[parent_index].dir_listing.abs_path + utf8_string;
			dir_tmp.dir_listing.abs_path = dir_path;

			// add dir entry to list
			dir_entries.push_back(std::move(dir_tmp));

			// add dir entry to map
			dir_entry_path_map[dir_path] = dir_entries.size() - 1;
			mDirParentVaddrMap[v_addr] = dir_entries.size() - 1;

			// add name to parent directory listing
			dir_entries[parent_index].dir_listing.dir_list.push_back(utf8_string);
		}
		

		uint32_t total_size = sizeof(ntd::n3ds::RomFsDirectoryEntry) + align<uint32_t>(getDirEntry(v_addr)->name_size.unwrap(), 4);

		if (getDirEntry(v_addr)->sibling_offset.unwrap() < v_addr)
		{
			throw tc::InvalidOperationException("ntd::n3ds::RomFsSnapshotGenerator", "Possibly corrupted directory entry");
		}

		v_addr += total_size;
	}

	// add files
	FileEntry file_tmp;
	for (uint32_t v_addr = 0; v_addr < mFileEntryTable.size();)
	{
		// check parent is in map
		if (mDirParentVaddrMap.find(getFileEntry(v_addr)->parent_offset.unwrap()) == mDirParentVaddrMap.end())
			throw tc::InvalidOperationException("ntd::n3ds::RomFsSnapshotGenerator", "File had invalid parent");

		if (getFileEntry(v_addr)->data_size.unwrap() != 0)
		{
			// substream
			//std::cout << "trying to add file" << std::endl;
			//std::cout << "offset " << std::hex << getFileEntry(v_addr)->data_offset.unwrap() << std::endl;
			//std::cout << "size   " << std::hex << getFileEntry(v_addr)->data_size.unwrap() << std::endl;
			file_tmp.stream = std::shared_ptr<tc::io::SubStream>(new tc::io::SubStream(mBaseStream, mDataOffset + getFileEntry(v_addr)->data_offset.unwrap(), getFileEntry(v_addr)->data_size.unwrap()));
			//std::cout << "file was added" << std::endl;
		}
		else
		{
			// empty stream
			file_tmp.stream = std::shared_ptr<tc::io::SubStream>(new tc::io::SubStream());
		}

		// save/transcode file name
		std::u16string utf16_string;
		std::string utf8_string;
		size_t str_len = getFileEntry(v_addr)->name_size.unwrap() / sizeof(uint16_t);
		for (size_t i = 0; i < str_len; i++)
		{
			utf16_string.push_back(getFileEntry(v_addr)->name[i].unwrap());
		}
		tc::string::TranscodeUtil::UTF16ToUTF8(utf16_string, utf8_string);

		// create file path
		size_t parent_index = mDirParentVaddrMap[getFileEntry(v_addr)->parent_offset.unwrap()];
		tc::io::Path file_path = dir_entries[parent_index].dir_listing.abs_path + utf8_string;

		// add file entry to list
		file_entries.push_back(std::move(file_tmp));

		// add file entry to map
		file_entry_path_map[file_path] = file_entries.size() - 1;

		// add name to parent directory listing
		dir_entries[parent_index].dir_listing.file_list.push_back(utf8_string);

		uint32_t total_size = sizeof(ntd::n3ds::RomFsFileEntry) + align<uint32_t>(getFileEntry(v_addr)->name_size.unwrap(), 4);

		if (getFileEntry(v_addr)->sibling_offset.unwrap() < v_addr)
		{
			throw tc::InvalidOperationException("ntd::n3ds::RomFsSnapshotGenerator", "Possibly corrupted file entry");
		}

		v_addr += total_size;
	}
	
	// old style recursive add
	//addDirectory(getDirEntry(0), 0);
}

void ntd::n3ds::RomFsSnapshotGenerator::addFile(const ntd::n3ds::RomFsFileEntry* file_entry, size_t parent_dir)
{
	// create file entry
	FileEntry tmp;
	if (file_entry->data_size.unwrap() != 0)
	{
		// substream
		//std::cout << "trying to add file" << std::endl;
		//std::cout << "offset " << std::hex << file_entry->data_offset.unwrap() << std::endl;
		//std::cout << "size   " << std::hex << file_entry->data_size.unwrap() << std::endl;
		tmp.stream = std::shared_ptr<tc::io::SubStream>(new tc::io::SubStream(mBaseStream, mDataOffset + file_entry->data_offset.unwrap(), file_entry->data_size.unwrap()));
		//std::cout << "file was added" << std::endl;
	}
	else
	{
		// empty stream
		tmp.stream = std::shared_ptr<tc::io::SubStream>(new tc::io::SubStream());
	}

	// save/transcode file name
	std::u16string utf16_string;
	std::string utf8_string;
	size_t str_len = file_entry->name_size.unwrap() / sizeof(uint16_t);
	for (size_t i = 0; i < str_len; i++)
	{
		utf16_string.push_back(file_entry->name[i].unwrap());
	}
	tc::string::TranscodeUtil::UTF16ToUTF8(utf16_string, utf8_string);

	// create file path
	tc::io::Path file_path = dir_entries[parent_dir].dir_listing.abs_path + utf8_string;

	// add file entry to list
	file_entries.push_back(tmp);

	// add file entry to map
	file_entry_path_map[file_path] = file_entries.size() - 1;

	// add name to parent directory listing
	dir_entries[parent_dir].dir_listing.file_list.push_back(utf8_string);
}

void ntd::n3ds::RomFsSnapshotGenerator::addDirectory(const ntd::n3ds::RomFsDirectoryEntry* dir_entry, size_t parent_dir)
{
	// create dir entry
	DirEntry tmp;

	// save/transcode file name
	std::u16string utf16_string;
	std::string utf8_string;
	size_t str_len = dir_entry->name_size.unwrap() / sizeof(uint16_t);
	for (size_t i = 0; i < str_len; i++)
	{
		utf16_string.push_back(dir_entry->name[i].unwrap());
	}
	tc::string::TranscodeUtil::UTF16ToUTF8(utf16_string, utf8_string);

	// this is the root entry
	if (dir_entry->parent_offset.unwrap() == 0 && dir_entry->sibling_offset.unwrap() == 0xffffffff && dir_entry->name_size.unwrap() == 0)
	{
		// create dir path
		tc::io::Path dir_path = tc::io::Path("/");
		tmp.dir_listing.abs_path = dir_path;

		// add dir entry to list
		dir_entries.push_back(tmp);

		// add dir entry to map
		dir_entry_path_map[dir_path] = dir_entries.size() - 1;
	}
	// this is a regular directory
	else
	{
		// create dir path
		tc::io::Path dir_path = dir_entries[parent_dir].dir_listing.abs_path + utf8_string;
		tmp.dir_listing.abs_path = dir_path;

		// add dir entry to list
		dir_entries.push_back(tmp);

		// add dir entry to map
		dir_entry_path_map[dir_path] = dir_entries.size() - 1;

		// add name to parent directory listing
		dir_entries[parent_dir].dir_listing.dir_list.push_back(utf8_string);
	}
	
	
	// get cur_dir pointer
	auto cur_dir = dir_entries.size() - 1;
	
	// add file children
	for (uint32_t child = dir_entry->file_offset.unwrap();
	    child != 0xffffffff; 
	    child = getFileEntry(child)->sibling_offset.unwrap())
	{
		//std::cout << "file child addr :" << std::hex << std::setw(8) << std::setfill('0') << child << std::endl;
		addFile(getFileEntry(child), cur_dir);
	}

	// add dir children
	for (uint32_t child = dir_entry->child_offset.unwrap();
	    child != 0xffffffff; 
	    child = getDirEntry(child)->sibling_offset.unwrap())
	{
		addDirectory(getDirEntry(child), cur_dir);
	}	
}