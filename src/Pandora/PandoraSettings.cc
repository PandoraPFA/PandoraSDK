/**
 *  @file PandoraSDK/src/Pandora/PandoraSettings.cc
 *
 *  @brief Implementation of the pandora settings class.
 *
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

PandoraSettings::PandoraSettings(const Pandora *const pPandora) :
    m_isMonitoringEnabled(false),
    m_shouldDisplayAlgorithmInfo(false),
    m_singleHitTypeClusteringMode(false),
    m_shouldCollapseMCParticlesToPfoTarget(false),
    m_useSingleMCParticleAssociation(false),
    m_electromagneticEnergyResolution(0.2f),
    m_hadronicEnergyResolution(0.6f),
    m_mcPfoSelectionRadius(500.f),
    m_mcPfoSelectionMomentum(0.01f),
    m_mcPfoSelectionLowEnergyNPCutOff(1.2f),
    m_gapTolerance(0.f),
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraSettings::~PandoraSettings()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        THROW_ON_ERROR(PandoraSettings::ReadGlobalSettings(pXmlHandle));

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize pandora settings: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::ReadGlobalSettings(const TiXmlHandle *const pXmlHandle)
{
    m_isMonitoringEnabled = false;
    PandoraOptionalXML(*pXmlHandle, "IsMonitoringEnabled", m_isMonitoringEnabled);

    m_shouldDisplayAlgorithmInfo = false;
    PandoraOptionalXML(*pXmlHandle, "ShouldDisplayAlgorithmInfo", m_shouldDisplayAlgorithmInfo);

    m_singleHitTypeClusteringMode = false;
    PandoraOptionalXML(*pXmlHandle, "SingleHitTypeClusteringMode", m_singleHitTypeClusteringMode);

    m_shouldCollapseMCParticlesToPfoTarget = false;
    PandoraOptionalXML(*pXmlHandle, "ShouldCollapseMCParticlesToPfoTarget", m_shouldCollapseMCParticlesToPfoTarget);

    m_useSingleMCParticleAssociation = false;
    PandoraOptionalXML(*pXmlHandle, "UseSingleMCParticleAssociation", m_useSingleMCParticleAssociation);

    m_electromagneticEnergyResolution = 0.2f;
    PandoraOptionalXML(*pXmlHandle, "ElectromagneticEnergyResolution", m_electromagneticEnergyResolution);

    if (m_electromagneticEnergyResolution < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    m_hadronicEnergyResolution = 0.6f;
    PandoraOptionalXML(*pXmlHandle, "HadronicEnergyResolution", m_hadronicEnergyResolution);

    if (m_hadronicEnergyResolution < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    m_mcPfoSelectionRadius = 500.f;
    PandoraOptionalXML(*pXmlHandle, "MCPfoSelectionRadius", m_mcPfoSelectionRadius);

    m_mcPfoSelectionMomentum = 0.01f;
    PandoraOptionalXML(*pXmlHandle, "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum);

    m_mcPfoSelectionLowEnergyNPCutOff = 1.2f;
    PandoraOptionalXML(*pXmlHandle, "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
