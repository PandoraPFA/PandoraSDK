/**
 *  @file   PandoraPFANew/Framework/src/Managers/PluginManager.cc
 * 
 *  @brief  Implementation of the pandora plugin manager class.
 * 
 *  $Log: $
 */

#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/XmlHelper.h"

#include "Managers/PluginManager.h"

namespace pandora
{

PluginManager::PluginManager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PluginManager::~PluginManager()
{
    m_hadEnergyCorrectionFunctionMap.clear();
    m_emEnergyCorrectionFunctionMap.clear();
    m_particleIdFunctionMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunction *pEnergyCorrectionFunction)
{
    EnergyCorrectionFunctionMap &energyCorrectionFunctionMap(this->GetEnergyCorrectionFunctionMap(energyCorrectionType));

    if (!energyCorrectionFunctionMap.insert(EnergyCorrectionFunctionMap::value_type(functionName, pEnergyCorrectionFunction)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializeEnergyCorrectionFunctions(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
    const EnergyCorrectionType energyCorrectionType, EnergyCorrectionFunctionVector &energyCorrectionFunctionVector)
{
    StringVector requestedFunctionNames;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(*pXmlHandle,
        xmlTagName, requestedFunctionNames));

    if (!requestedFunctionNames.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignEnergyCorrectionFunctions(requestedFunctionNames,
            energyCorrectionType, energyCorrectionFunctionVector));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::AssignEnergyCorrectionFunctions(const StringVector &functionNames, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunctionVector &energyCorrectionFunctionVector)
{
    EnergyCorrectionFunctionMap &energyCorrectionFunctionMap(this->GetEnergyCorrectionFunctionMap(energyCorrectionType));

    for (StringVector::const_iterator nameIter = functionNames.begin(), nameIterEnd = functionNames.end(); nameIter != nameIterEnd; ++nameIter)
    {
        EnergyCorrectionFunctionMap::const_iterator iter = energyCorrectionFunctionMap.find(*nameIter);

        if (energyCorrectionFunctionMap.end() == iter)
            return STATUS_CODE_NOT_FOUND;

        energyCorrectionFunctionVector.push_back(iter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PluginManager::EnergyCorrectionFunctionMap &PluginManager::GetEnergyCorrectionFunctionMap(const EnergyCorrectionType energyCorrectionType)
{
    switch (energyCorrectionType)
    {
    case HADRONIC:
        return m_hadEnergyCorrectionFunctionMap;

    case ELECTROMAGNETIC:
        return m_emEnergyCorrectionFunctionMap;

    default:
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction)
{
    if (!m_particleIdFunctionMap.insert(ParticleIdFunctionMap::value_type(functionName, pParticleIdFunction)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializeParticleIdFunction(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
    ParticleIdFunction *&pParticleIdFunction)
{
    std::string requestedFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        xmlTagName, requestedFunctionName));

    if (!requestedFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(requestedFunctionName, pParticleIdFunction));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::AssignParticleIdFunction(const std::string &functionName, ParticleIdFunction *&pParticleIdFunction) const
{
    ParticleIdFunctionMap::const_iterator iter = m_particleIdFunctionMap.find(functionName);

    if (m_particleIdFunctionMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    pParticleIdFunction = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    // Energy correction functions
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeEnergyCorrectionFunctions(pXmlHandle,
        "HadronicEnergyCorrectionFunctions", HADRONIC, EnergyCorrectionsHelper::m_hadEnergyCorrectionFunctions));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeEnergyCorrectionFunctions(pXmlHandle,
        "ElectromagneticEnergyCorrectionFunctions", ELECTROMAGNETIC, EnergyCorrectionsHelper::m_emEnergyCorrectionFunctions));

    // Particle id functions
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "EmShowerFastFunction", ParticleIdHelper::m_pEmShowerFastFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "EmShowerFullFunction", ParticleIdHelper::m_pEmShowerFullFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "PhotonFastFunction", ParticleIdHelper::m_pPhotonFastFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "PhotonFullFunction", ParticleIdHelper::m_pPhotonFullFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "ElectronFastFunction", ParticleIdHelper::m_pElectronFastFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "ElectronFullFunction", ParticleIdHelper::m_pElectronFullFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "MuonFastFunction", ParticleIdHelper::m_pMuonFastFunction));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializeParticleIdFunction(pXmlHandle,
        "MuonFullFunction", ParticleIdHelper::m_pMuonFullFunction));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
