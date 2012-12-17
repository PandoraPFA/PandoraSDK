/**
 *    @file PandoraPFANew/Framework/src/Pandora/PandoraSettings.cc
 * 
 *    @brief Implementation of the pandora settings class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/ClusterHelper.h"
#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/FragmentRemovalHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/MCParticleHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

bool PandoraSettings::m_isInitialized = false;
PandoraSettings::SettingsFunctionToNameMap PandoraSettings::m_settingsFunctionToNameMap;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::RegisterSettingsFunction(const std::string &xmlTagName, SettingsFunction *pSettingsFunction)
{
    if (!m_settingsFunctionToNameMap.insert(SettingsFunctionToNameMap::value_type(pSettingsFunction, xmlTagName)).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        if (m_isInitialized)
            return STATUS_CODE_ALREADY_INITIALIZED;

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, EnergyCorrectionsHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, GeometryHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, MCParticleHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ParticleIdHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::ReadSettings(pXmlHandle));

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraSettings::RunRegisteredSettingsFunctions(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraSettings::ReadGlobalSettings(pXmlHandle));

        m_isInitialized = true;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize pandora settings: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::RunRegisteredSettingsFunctions(const TiXmlHandle *const pXmlHandle)
{
    for (SettingsFunctionToNameMap::const_iterator iter = m_settingsFunctionToNameMap.begin(), iterEnd = m_settingsFunctionToNameMap.end();
        iter != iterEnd; ++iter)
    {
        TiXmlElement *pXmlElement = pXmlHandle->FirstChild(iter->second).Element();

        if (NULL != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*(iter->first))(TiXmlHandle(pXmlElement)));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraSettings::m_isMonitoringEnabled = false;
bool PandoraSettings::m_shouldDisplayAlgorithmInfo = false;
bool PandoraSettings::m_shouldCollapseMCParticlesToPfoTarget = true;
float PandoraSettings::m_electromagneticEnergyResolution = 0.2f;
float PandoraSettings::m_hadronicEnergyResolution = 0.6f;
float PandoraSettings::m_mcPfoSelectionRadius = 500.f;
float PandoraSettings::m_mcPfoSelectionMomentum = 0.01f;
float PandoraSettings::m_mcPfoSelectionLowEnergyNPCutOff = 1.2f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::ReadGlobalSettings(const TiXmlHandle *const pXmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "IsMonitoringEnabled", m_isMonitoringEnabled));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldDisplayAlgorithmInfo", m_shouldDisplayAlgorithmInfo));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldCollapseMCParticlesToPfoTarget", m_shouldCollapseMCParticlesToPfoTarget));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectromagneticEnergyResolution", m_electromagneticEnergyResolution));

    if (0.f == m_electromagneticEnergyResolution)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HadronicEnergyResolution", m_hadronicEnergyResolution));

    if (0.f == m_hadronicEnergyResolution)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
