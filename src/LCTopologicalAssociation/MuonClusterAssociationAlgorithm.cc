/**
 *  @file   LCContent/src/LCTopologicalAssociation/MuonClusterAssociationAlgorithm.cc
 * 
 *  @brief  Implementation of the muon cluster association algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/ReclusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/MuonClusterAssociationAlgorithm.h"

using namespace pandora;

namespace lc_content
{

MuonClusterAssociationAlgorithm::MuonClusterAssociationAlgorithm() :
    m_dCosCut(0.95f),
    m_minHitsInMuonCluster(3),
    m_shouldEstimateEnergyLostInCoil(true),
    m_coilCorrectionMinInnerRadius(4000.f),
    m_coilCorrectionMinInnerLayerHits(3),
    m_coilEnergyLossCorrection(10.f),
    m_minClusterHadronicEnergy(0.25f),
    m_minHitsInCluster(5),
    m_clusterAssociationChi(3.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonClusterAssociationAlgorithm::Run()
{
    // Get the muon cluster list, which will only exist if there are muon hits to cluster
    const ClusterList *pMuonClusterList = NULL;
    StatusCode listStatusCode = PandoraContentApi::GetList(*this, m_muonClusterListName, pMuonClusterList);

    if (STATUS_CODE_NOT_INITIALIZED == listStatusCode)
        return STATUS_CODE_SUCCESS;

    if (STATUS_CODE_SUCCESS != listStatusCode)
        return listStatusCode;

    ClusterVector muonClusterVector(pMuonClusterList->begin(), pMuonClusterList->end());
    std::sort(muonClusterVector.begin(), muonClusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Get the current cluster list, with which muon clusters will be associated
    std::string inputClusterListName;
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pInputClusterList, inputClusterListName));

    ClusterList standaloneMuonClusters;

    // Loop over muon cluster list, looking for muon clusters containing sufficient hits
    for (ClusterVector::iterator iterI = muonClusterVector.begin(), iterIEnd = muonClusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *const pMuonCluster = *iterI;

        if (NULL == pMuonCluster)
            continue;

        if (pMuonCluster->GetNCaloHits() < m_minHitsInMuonCluster)
            continue;

        const Cluster *pBestHadron(NULL), *pBestLeavingTrack(NULL), *pBestNonLeavingTrack(NULL);
        float bestDCosHadron(m_dCosCut), bestDCosLeavingTrack(m_dCosCut), bestDCosNonLeavingTrack(m_dCosCut);
        float bestEnergyHadron(0.), bestEnergyLeavingTrack(0.), bestEnergyNonLeavingTrack(0.);

        // Calculate muon cluster properties
        const unsigned int muonClusterInnerLayer(pMuonCluster->GetInnerPseudoLayer());
        const CartesianVector muonInnerCentroid(pMuonCluster->GetCentroid(muonClusterInnerLayer));
        const CartesianVector &muonDirection(pMuonCluster->GetInitialDirection());

        float energyLostInCoil(0.);

        if (m_shouldEstimateEnergyLostInCoil)
        {
            const float muonInnerX(muonInnerCentroid.GetX()), muonInnerY(muonInnerCentroid.GetY());
            const float innerRadius(std::sqrt(muonInnerX * muonInnerX + muonInnerY * muonInnerY));

            const unsigned int nHitsInInnerLayer(pMuonCluster->GetOrderedCaloHitList().GetNCaloHitsInPseudoLayer(muonClusterInnerLayer));

            if ((innerRadius > m_coilCorrectionMinInnerRadius) && (nHitsInInnerLayer >= m_coilCorrectionMinInnerLayerHits))
                energyLostInCoil += m_coilEnergyLossCorrection;
        }

        // For each muon cluster, examine suitable clusters in the input cluster list, looking for merging possibilities
        for (ClusterList::const_iterator iterJ = pInputClusterList->begin(), iterJEnd = pInputClusterList->end(); iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pCluster = *iterJ;

            // Apply basic threshold cuts to cluster candidate
            const float clusterEnergy(pCluster->GetHadronicEnergy());

            if (clusterEnergy < m_minClusterHadronicEnergy)
                continue;

            if (pCluster->GetNCaloHits() < m_minHitsInCluster)
                continue;

            // Calculate properties of cluster candidate
            const CartesianVector clusterDirection(pCluster->GetCentroid(pCluster->GetOuterPseudoLayer()).GetUnitVector());
            const float dCos(muonDirection.GetDotProduct(clusterDirection));

            const TrackList &trackList(pCluster->GetAssociatedTrackList());

            float trackEnergySum(0.);

            for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
            {
                trackEnergySum += (*trackIter)->GetEnergyAtDca();
            }

            const bool hasAssociatedTrack(!trackList.empty());
            const bool isLeavingDetector(ClusterHelper::IsClusterLeavingDetector(pCluster));

            // Identify best association with a leaving, non-track-associated cluster
            if (isLeavingDetector && !hasAssociatedTrack)
            {
                if ((dCos > bestDCosHadron) || ((dCos == bestDCosHadron) && (clusterEnergy > bestEnergyHadron)))
                {
                    bestDCosHadron = dCos;
                    pBestHadron = pCluster;
                    bestEnergyHadron = clusterEnergy;
                }
            }

            // Identify best association with a leaving, track-associated cluster
            if (isLeavingDetector && hasAssociatedTrack)
            {
                if ((dCos > bestDCosLeavingTrack) || ((dCos == bestDCosLeavingTrack) && (clusterEnergy > bestEnergyLeavingTrack)))
                {
                    const float chi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(),
                        pCluster->GetTrackComparisonEnergy(this->GetPandora()) + energyLostInCoil, trackEnergySum));

                    if (chi < m_clusterAssociationChi)
                    {
                        bestDCosLeavingTrack = dCos;
                        pBestLeavingTrack = pCluster;
                        bestEnergyLeavingTrack = clusterEnergy;
                    }
                }
            }

            // Identify best association with a non-leaving, track-associated cluster
            if (!isLeavingDetector && hasAssociatedTrack)
            {
                if ((dCos > bestDCosNonLeavingTrack) || ((dCos == bestDCosNonLeavingTrack) && (clusterEnergy > bestEnergyNonLeavingTrack)))
                {
                    const float oldChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(),
                        pCluster->GetTrackComparisonEnergy(this->GetPandora()), trackEnergySum));
                    const float newChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(),
                        pCluster->GetTrackComparisonEnergy(this->GetPandora()) + energyLostInCoil, trackEnergySum));

                    if ((oldChi < -m_clusterAssociationChi) && (newChi < m_clusterAssociationChi))
                    {
                        bestDCosNonLeavingTrack = dCos;
                        pBestNonLeavingTrack = pCluster;
                        bestEnergyNonLeavingTrack = clusterEnergy;
                    }
                }
            }
        }

        // Select best merging candidate from those identified above
        const Cluster *pBestInputCluster(NULL);

        if (NULL != pBestLeavingTrack)
        {
            pBestInputCluster = pBestLeavingTrack;
        }
        else if (NULL != pBestHadron)
        {
            pBestInputCluster = pBestHadron;
        }
        else if (NULL != pBestNonLeavingTrack)
        {
            pBestInputCluster = pBestNonLeavingTrack;
        }
        else
        {
            standaloneMuonClusters.insert(pMuonCluster);
        }

        // Merge the clusters
        if (NULL != pBestInputCluster)
        {
            *iterI = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestInputCluster,
                pMuonCluster, inputClusterListName, m_muonClusterListName));
        }
    }

    // Merge any identified standalone muon clusters into the input cluster list
    if (!standaloneMuonClusters.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, m_muonClusterListName,
            inputClusterListName, standaloneMuonClusters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonClusterAssociationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonClusterListName", m_muonClusterListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DCosCut", m_dCosCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInMuonCluster", m_minHitsInMuonCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldEstimateEnergyLostInCoil", m_shouldEstimateEnergyLostInCoil));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilCorrectionMinInnerRadius", m_coilCorrectionMinInnerRadius));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilCorrectionMinInnerLayerHits", m_coilCorrectionMinInnerLayerHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilEnergyLossCorrection", m_coilEnergyLossCorrection));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadronicEnergy", m_minClusterHadronicEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterAssociationChi", m_clusterAssociationChi));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
