/**
 *  @file   LCContent/src/LCMonitoring/EfficiencyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the efficiency monitoring algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/SortingHelper.h"

#include "LCMonitoring/EfficiencyMonitoringAlgorithm.h"

using namespace pandora;

namespace lc_content
{

EfficiencyMonitoringAlgorithm::EfficiencyMonitoringAlgorithm() :
    m_mcThresholdEnergy(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EfficiencyMonitoringAlgorithm::~EfficiencyMonitoringAlgorithm()
{
    PANDORA_MONITORING_API(SaveTree(this->GetPandora(), "EffTree", m_monitoringFileName, "UPDATE"));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::Run()
{
#ifdef MONITORING
    // Extract the mc particle - this algorithm is intended to work only with single particle samples
    const MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    if (1 != pMCParticleList->size())
    {
        std::cout << "EfficiencyMonitoring - Algorithm works only with single particle samples, nParticles " << pMCParticleList->size() << std::endl;
        return STATUS_CODE_SUCCESS;
    }

    const MCParticle *const pMCParticle(*(pMCParticleList->begin()));

    // Extract the mc particle properties
    const float mcEnergy(pMCParticle->GetEnergy());
    const int mcPDGCode(pMCParticle->GetParticleId());

    if (mcEnergy < m_mcThresholdEnergy)
    {
        std::cout << "EfficiencyMonitoring - MC particle energy below threshold " << mcEnergy << "( < " << m_mcThresholdEnergy << ")" <<std::endl;
        return STATUS_CODE_SUCCESS;
    }

    float radius(0.f), phi(0.f), theta(0.f);
    const CartesianVector &mcPosition(pMCParticle->GetEndpoint());
    mcPosition.GetSphericalCoordinates(radius, phi, theta);

    // Extract the most energetic pfo
    const PfoList *pPfoList= NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

    PfoVector pfoVector(pPfoList->begin(), pPfoList->end());
    std::sort(pfoVector.begin(), pfoVector.end(), SortingHelper::SortPfosByEnergy);
    const ParticleFlowObject *const pMostEnergeticPfo((!pfoVector.empty()) ? *(pfoVector.begin()) : NULL);

    // Extract the pfo properties
    float recoEnergy((pMostEnergeticPfo != NULL) ? pMostEnergeticPfo->GetEnergy() : 0.f);
    int recoPDGCode((pMostEnergeticPfo != NULL) ? pMostEnergeticPfo->GetParticleId() : 0);

    // Look for specific case of photon conversion to e+e-
    int isPhotonConversion(0);

    if ((mcPDGCode == PHOTON) && (std::abs(recoPDGCode) == E_MINUS) && (pfoVector.size() == 2))
    {
        const ParticleFlowObject *const pPfo1(pfoVector.at(0));
        const ParticleFlowObject *const pPfo2(pfoVector.at(1));

        if ((pPfo1->GetParticleId() == -(pPfo2->GetParticleId())) && (std::abs(pPfo1->GetParticleId()) == E_MINUS))
        {
            recoPDGCode = PHOTON;
            recoEnergy = pPfo1->GetEnergy() + pPfo2->GetEnergy();
            isPhotonConversion = 1;
        }
    }

    // Fill tree with information for this single particle event
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "mcPDGCode", mcPDGCode));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "recoPDGCode", recoPDGCode));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "mcEnergy", mcEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "recoEnergy", recoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "radius", radius));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "phi", phi));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "theta", theta));
    PANDORA_MONITORING_API(SetTreeVariable(this->GetPandora(), "EffTree", "isPhotonConversion", isPhotonConversion));
    PANDORA_MONITORING_API(FillTree(this->GetPandora(), "EffTree"));
#endif
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MCThresholdEnergy", m_mcThresholdEnergy));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
