/**
 *  @file   LCContent/src/LCFragmentRemoval/MergeSplitPhotonsAlgorithm.cc
 * 
 *  @brief  Implementation of the merge split photons algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCFragmentRemoval/MergeSplitPhotonsAlgorithm.h"

#include "LCHelpers/FragmentRemovalHelper.h"
#include "LCHelpers/SortingHelper.h"

using namespace pandora;

namespace lc_content
{

MergeSplitPhotonsAlgorithm::MergeSplitPhotonsAlgorithm() :
    m_minShowerMaxCosAngle(0.98f),
    m_contactDistanceThreshold(2.f),
    m_minContactLayers(3),
    m_minContactFraction(0.5f),
    m_transProfileMaxLayer(30),
    m_acceptMaxSmallFragmentEnergy(0.2f),
    m_acceptMaxSubsidiaryPeakEnergy(0.5f),
    m_acceptFragmentEnergyRatio(0.05f),
    m_acceptSubsidiaryPeakEnergyRatio(0.1f),
    m_earlyTransProfileMaxLayer(20)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MergeSplitPhotonsAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    const ShowerProfilePlugin *const pShowerProfilePlugin(PandoraContentApi::GetPlugins(*this)->GetShowerProfilePlugin());

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over photon candidate clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        if (NULL == pParentCluster)
            continue;

        if (!pParentCluster->GetAssociatedTrackList().empty())
            continue;

        if (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pParentCluster->GetOuterLayerHitType()) > FINE)
            continue;

        const CartesianVector parentShowerMaxCentroid(pParentCluster->GetCentroid(this->GetShowerMaxLayer(pParentCluster)));
        const bool isParentPhoton(pParentCluster->IsPhotonFast(this->GetPandora()));

        // Find daughter photon candidate clusters
        for (ClusterVector::iterator iterJ = iterI + 1, iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pDaughterCluster = *iterJ;

            if (NULL == pDaughterCluster)
                continue;

            if (!pDaughterCluster->GetAssociatedTrackList().empty())
                continue;

            if (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetOuterLayerHitType()) > FINE)
                continue;

            const CartesianVector daughterShowerMaxCentroid(pDaughterCluster->GetCentroid(this->GetShowerMaxLayer(pDaughterCluster)));
            const bool isDaughterPhoton(pDaughterCluster->IsPhotonFast(this->GetPandora()));

            // Look for compatible parent/daughter pairings
            if (!isParentPhoton && !isDaughterPhoton)
                continue;

            if (parentShowerMaxCentroid.GetCosOpeningAngle(daughterShowerMaxCentroid) <= m_minShowerMaxCosAngle)
                continue;

            unsigned int nContactLayers(0);
            float contactFraction(0.f);

            const StatusCode contactStatusCode(FragmentRemovalHelper::GetClusterContactDetails(pParentCluster, pDaughterCluster, 
                m_contactDistanceThreshold, nContactLayers, contactFraction));

            if ((STATUS_CODE_SUCCESS == contactStatusCode) && (nContactLayers >= m_minContactLayers) && (contactFraction > m_minContactFraction))
            {
                // Initialize fragmentation to compare merged cluster with original
                ClusterList clusterList;
                clusterList.insert(pParentCluster); clusterList.insert(pDaughterCluster);

                std::string originalClusterListName, mergedClusterListName;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
                    originalClusterListName, mergedClusterListName));

                PandoraContentApi::Cluster::Parameters parameters;
                pParentCluster->GetOrderedCaloHitList().GetCaloHitList(parameters.m_caloHitList);
                pDaughterCluster->GetOrderedCaloHitList().GetCaloHitList(parameters.m_caloHitList);
                parameters.m_caloHitList.insert(pParentCluster->GetIsolatedCaloHitList().begin(), pParentCluster->GetIsolatedCaloHitList().end());
                parameters.m_caloHitList.insert(pDaughterCluster->GetIsolatedCaloHitList().begin(), pDaughterCluster->GetIsolatedCaloHitList().end());

                Cluster *pMergedCluster = NULL;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pMergedCluster));

                // Look for peaks in cluster transverse shower profile
                ShowerProfilePlugin::ShowerPeakList showerPeakList;
                pShowerProfilePlugin->CalculateTransverseProfile(pMergedCluster, m_transProfileMaxLayer, showerPeakList);

                const float subsidiaryPeakEnergy((showerPeakList.size() > 1) ? showerPeakList[1].GetPeakEnergy() : 0.f);
                const float smallFragmentEnergy(std::min(pDaughterCluster->GetElectromagneticEnergy(), pParentCluster->GetElectromagneticEnergy()));
                const float largeFragmentEnergy(std::max(pDaughterCluster->GetElectromagneticEnergy(), pParentCluster->GetElectromagneticEnergy()));

                // Decide whether merged cluster is better than individual fragments
                bool acceptMerge(false);

                if (smallFragmentEnergy < m_acceptMaxSmallFragmentEnergy)
                {
                    acceptMerge = true;
                }
                else if (subsidiaryPeakEnergy < m_acceptMaxSubsidiaryPeakEnergy)
                {
                    if (smallFragmentEnergy < m_acceptFragmentEnergyRatio * largeFragmentEnergy)
                    {
                        acceptMerge = true;
                    }
                    else if (subsidiaryPeakEnergy < m_acceptSubsidiaryPeakEnergyRatio * smallFragmentEnergy)
                    {
                        acceptMerge = true;
                    }
                }

                // If merging hard photons, check for early peaks in transverse profile
                if (acceptMerge && (smallFragmentEnergy > m_acceptMaxSmallFragmentEnergy))
                {
                    ShowerProfilePlugin::ShowerPeakList earlyShowerPeakList;
                    pShowerProfilePlugin->CalculateTransverseProfile(pMergedCluster, m_earlyTransProfileMaxLayer, earlyShowerPeakList);

                    const float earlySubsidiaryPeakEnergy((earlyShowerPeakList.size() > 1) ? earlyShowerPeakList[1].GetPeakEnergy() : 0.f);

                    if (earlySubsidiaryPeakEnergy > m_acceptMaxSubsidiaryPeakEnergy)
                        acceptMerge = false;
                }

                // Tidy up
                const std::string clusterListToSaveName(acceptMerge ? mergedClusterListName : originalClusterListName);
                const std::string clusterListToDeleteName(acceptMerge ? originalClusterListName : mergedClusterListName);

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
                    clusterListToDeleteName));

                if (acceptMerge)
                {
                    *iterI = NULL;
                    *iterJ = NULL;
                    break;
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int MergeSplitPhotonsAlgorithm::GetShowerMaxLayer(const Cluster *const pCluster) const
{
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    float maxEnergyInLayer(0.f);
    unsigned int showerMaxLayer(0);
    bool isLayerFound(false);

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        float energyInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            energyInLayer += (*hitIter)->GetElectromagneticEnergy();
        }

        if (energyInLayer > maxEnergyInLayer)
        {
            maxEnergyInLayer = energyInLayer;
            showerMaxLayer = iter->first;
            isLayerFound = true;
        }
    }

    if (!isLayerFound)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return showerMaxLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MergeSplitPhotonsAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerMaxCosAngle", m_minShowerMaxCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactDistanceThreshold", m_contactDistanceThreshold));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinContactLayers", m_minContactLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinContactFraction", m_minContactFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileMaxLayer", m_transProfileMaxLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AcceptMaxSmallFragmentEnergy", m_acceptMaxSmallFragmentEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AcceptMaxSubsidiaryPeakEnergy", m_acceptMaxSubsidiaryPeakEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AcceptFragmentEnergyRatio", m_acceptFragmentEnergyRatio));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AcceptSubsidiaryPeakEnergyRatio", m_acceptSubsidiaryPeakEnergyRatio));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EarlyTransProfileMaxLayer", m_earlyTransProfileMaxLayer));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
