/**
 *  @file   PandoraPFANew/Framework/src/Helpers/EnergyCorrectionsHelper.cc
 * 
 *  @brief  Implementation of the energy corrections helper class.
 * 
 *  $Log: $
 */

#include "Helpers/EnergyCorrectionsHelper.h"

#include "Objects/Cluster.h"

namespace pandora
{

EnergyCorrectionFunctionVector EnergyCorrectionsHelper::m_hadEnergyCorrectionFunctions;
EnergyCorrectionFunctionVector EnergyCorrectionsHelper::m_emEnergyCorrectionFunctions;
unsigned int EnergyCorrectionsHelper::m_nEnergyCorrectionCalls = 0;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrectionsHelper::EnergyCorrection(const Cluster *const pCluster, float &correctedElectromagneticEnergy,
    float &correctedHadronicEnergy)
{
    // ATTN: Current protection against recursive calls is not thread-safe
    if (m_nEnergyCorrectionCalls++ > 0)
        return STATUS_CODE_NOT_ALLOWED;

    // Hadronic energy corrections
    correctedHadronicEnergy = pCluster->GetHadronicEnergy();

    for (EnergyCorrectionFunctionVector::const_iterator iter = m_hadEnergyCorrectionFunctions.begin(),
        iterEnd = m_hadEnergyCorrectionFunctions.end(); iter != iterEnd; ++iter)
    {
        (*(*iter))(pCluster, correctedHadronicEnergy);
    }

    // Electromagnetic energy corrections
    correctedElectromagneticEnergy = pCluster->GetElectromagneticEnergy();

    for (EnergyCorrectionFunctionVector::const_iterator iter = m_emEnergyCorrectionFunctions.begin(),
        iterEnd = m_emEnergyCorrectionFunctions.end(); iter != iterEnd; ++iter)
    {
        (*(*iter))(pCluster, correctedElectromagneticEnergy);
    }

    m_nEnergyCorrectionCalls--;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrectionsHelper::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
    /*TiXmlElement *pXmlElement(pXmlHandle->FirstChild("EnergyCorrectionsHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);
    }*/

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
