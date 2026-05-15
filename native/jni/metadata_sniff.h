#pragma once

#include <string>

struct SniffedMetadata {
    std::string title_id;
    std::string title_version;
    bool already_decrypted = false;
    bool is_3ds = false;
    bool valid = false;
};

bool sniff_metadata_from_fd(int fd, SniffedMetadata &out);
