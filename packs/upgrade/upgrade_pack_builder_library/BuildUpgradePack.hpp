#ifndef UPGRADE_PACK_BUILD_LIBRARY_BUILD_UPGRADE_PACK_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_BUILD_UPGRADE_PACK_HPP

#include "infra/util/public/ByteRange.hpp"
#include "upgrade_pack_builder_library/UpgradePackBuilder.hpp"
#include <string>
#include <vector>
#include <utility>

namespace application
{
    int BuildUpgradePack(const application::UpgradePackBuilder::HeaderInfo& headerInfo, const std::vector<std::string>& supportedHexTargets,
        const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets, int argc, char* argv[], infra::ConstByteRange aesKey, infra::ConstByteRange ecDsa224PublicKey, infra::ConstByteRange ecDsa224PrivateKey);
}

#endif
