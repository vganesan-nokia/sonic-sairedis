#include "CorePortIndexMap.h"

#include "swss/logger.h"

#include <set>

using namespace saivs;

CorePortIndexMap::CorePortIndexMap(
        _In_ uint32_t switchIndex):
    m_switchIndex(switchIndex)
{
    SWSS_LOG_ENTER();

    // empty
}

uint32_t CorePortIndexMap::getSwitchIndex() const
{
    SWSS_LOG_ENTER();

    return m_switchIndex;
}

bool CorePortIndexMap::add(
        _In_ const std::string& ifname,
        _In_ const std::vector<uint32_t>& corePortIndex)
{
    SWSS_LOG_ENTER();

    auto n = corePortIndex.size();

    if (n != 2)
    {
        SWSS_LOG_ERROR("Invalid corePortIndex. Core port index must have core and core port index %d, %s", n, ifname.c_str());
        return false;
    }

    if (m_ifname_to_coreportindex.find(ifname) != m_ifname_to_coreportindex.end())
    {
        SWSS_LOG_ERROR("interface %s already in core port index map (%d, %d)", ifname.c_str(), corePortIndex[0], corePortIndex[1]);
        return false;
    }

    m_corePortIndexMap.push_back(corePortIndex);

    m_coreportindex_to_ifname[corePortIndex] = ifname;

    m_ifname_to_coreportindex[ifname] = corePortIndex;

    return true;
}

bool CorePortIndexMap::remove(
        _In_ const std::string& ifname)
{
    SWSS_LOG_ENTER();

    auto it = m_ifname_to_coreportindex.find(ifname);

    if (it == m_ifname_to_coreportindex.end())
    {
        SWSS_LOG_ERROR("interfce %s does not have core port index in switch %d", ifname.c_str(), m_switchIndex);
        return false;
    }

    auto corePortIndex = it->second;

    m_coreportindex_to_ifname.erase(corePortIndex);

    for (size_t idx = 0; idx < m_corePortIndexMap.size(); idx++)
    {
        if (m_corePortIndexMap[idx][0] == corePortIndex[0] && m_corePortIndexMap[idx][1] == corePortIndex[1])
        {
            m_corePortIndexMap.erase(m_corePortIndexMap.begin() + idx);
            break;
        }
    }

    m_ifname_to_coreportindex.erase(it);

    return true;
}

std::shared_ptr<CorePortIndexMap> CorePortIndexMap::getDefaultCorePortIndexMap(
        _In_ uint32_t switchIndex)
{
    SWSS_LOG_ENTER();

    const uint32_t defaultPortCount = 32;

    uint32_t defaultCorePortIndexMap[defaultPortCount * 2] = {
        0,1,
        0,2,
        0,3,
        0,4,
        0,5,
        0,6,
        0,7,
        0,8,
        0,9,
        0,10,
        0,11,
        0,12,
        0,13,
        0,14,
        0,15,
        0,16,
        1,1,
        1,2,
        1,3,
        1,4,
        1,5,
        1,6,
        1,7,
        1,8,
        1,9,
        1,10,
        1,11,
        1,12,
        1,13,
        1,14,
        1,15,
        1,16
    };

    auto corePortIndexMap = std::make_shared<CorePortIndexMap>(switchIndex);

    for (uint32_t idx = 0; idx < defaultPortCount; idx++)
    {
        auto ifname = "eth" + std::to_string(idx);

        std::vector<uint32_t> cpidx;

        cpidx.push_back(defaultCorePortIndexMap[idx * 2]);
        cpidx.push_back(defaultCorePortIndexMap[idx * 2 + 1]);

        corePortIndexMap->add(ifname, cpidx);
    }

    return corePortIndexMap;
}

bool CorePortIndexMap::isEmpty() const
{
    SWSS_LOG_ENTER();

    return m_ifname_to_coreportindex.size() == 0;
}

bool CorePortIndexMap::hasInterface(
        _In_ const std::string& ifname) const
{
    SWSS_LOG_ENTER();

    return m_ifname_to_coreportindex.find(ifname) != m_ifname_to_coreportindex.end();
}

const std::vector<std::vector<uint32_t>> CorePortIndexMap::getCorePortIndexVector() const
{
    SWSS_LOG_ENTER();

    return m_corePortIndexMap;
}

std::string CorePortIndexMap::getInterfaceFromCorePortIndex(
        _In_ std::vector<uint32_t> corePortIndex) const
{
    SWSS_LOG_ENTER();

    auto it = m_coreportindex_to_ifname.find(corePortIndex);

    if (it == m_coreportindex_to_ifname.end())
    {
        SWSS_LOG_WARN("Core port index (%d, %d) not found on index %u", corePortIndex[0], corePortIndex[1], m_switchIndex);

        return "";
    }

    return it->second;
}
