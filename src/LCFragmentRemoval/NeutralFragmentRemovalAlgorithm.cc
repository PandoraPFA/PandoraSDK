/**
 *  @file   LCContent/src/LCFragmentRemoval/NeutralFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the neutral fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCFragmentRemoval/NeutralFragmentRemovalAlgorithm.h"

using namespace pandora;

namespace lc_content
{

NeutralFragmentRemovalAlgorithm::NeutralFragmentRemovalAlgorithm() :
    m_nMaxPasses(200),
    m_minDaughterCaloHits(5),
    m_minDaughterHadronicEnergy(0.025f),
    m_photonLikeMaxInnerLayer(10),
    m_photonLikeMinDCosR(0.5f),
    m_photonLikeMaxShowerStart(5.f),
    m_photonLikeMaxProfileDiscrepancy(0.75f),
    m_contactCutMaxDistance(500.f),
    m_contactCutNLayers(2),
    m_contactCutConeFraction1(0.5f),
    m_contactCutCloseHitFraction1(0.5f),
    m_contactCutCloseHitFraction2(0.5f),
    m_contactCutNearbyDistance(100.f),
    m_contactCutNearbyCloseHitFraction2(0.25f),
    m_contactEvidenceNLayers1(10),
    m_contactEvidenceNLayers2(4),
    m_contactEvidenceNLayers3(1),
    m_contactEvidence1(2.f),
    m_contactEvidence2(1.f),
    m_contactEvidence3(0.5f),
    m_coneEvidenceFraction1(0.5f),
    m_coneEvidenceFineGranularityMultiplier(0.5f),
    m_distanceEvidence1(100.f),
    m_distanceEvidence1d(100.f),
    m_distanceEvidenceCloseFraction1Multiplier(1.f),
    m_distanceEvidenceCloseFraction2Multiplier(2.f),
    m_contactWeight(1.f),
    m_coneWeight(1.f),
    m_distanceWeight(1.f),
    m_minEvidence(2.f)
{
    m_contactParameters.m_coneCosineHalfAngle1 = 0.9f;
    m_contactParameters.m_coneCosineHalfAngle2 = 0.95f;
    m_contactParameters.m_coneCosineHalfAngle3 = 0.985f;
    m_contactParameters.m_closeHitDistance1 = 100.f;
    m_contactParameters.m_closeHitDistance2 = 50.f;
    m_contactParameters.m_minCosOpeningAngle = 0.5f;
    m_contactParameters.m_distanceThreshold = 2.f;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode NeutralFragmentRemovalAlgorithm::Run()
{
    unsigned int nPasses(0);
    bool isFirstPass(true), shouldRecalculate(true);

    ClusterList affectedClusters;
    NeutralClusterContactMap neutralClusterContactMap;

    while ((nPasses++ < m_nMaxPasses) && shouldRecalculate)
    {
        shouldRecalculate = false;
        Cluster *pBestParentCluster(NULL), *pBestDaughterCluster(NULL);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetNeutralClusterContactMap(isFirstPass, affectedClusters, neutralClusterContactMap));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterMergingCandidates(neutralClusterContactMap, pBestParentCluster,
            pBestDaughterCluster));

        if ((NULL != pBestParentCluster) && (NULL != pBestDaughterCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusters(neutralClusterContactMap, pBestParentCluster,
                pBestDaughterCluster, affectedClusters));

            neutralClusterContactMap.erase(neutralClusterContactMap.find(pBestDaughterCluster));
            shouldRecalculate = true;

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster,
                pBestDaughterCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode NeutralFragmentRemovalAlgorithm::GetNeutralClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters,
    NeutralClusterContactMap &neutralClusterContactMap) const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(), iterIEnd = pClusterList->end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        // Identify whether cluster contacts need to be recalculated
        if (!isFirstPass)
        {
            if (affectedClusters.end() == affectedClusters.find(pDaughterCluster))
                continue;

            NeutralClusterContactMap::iterator pastEntryIter = neutralClusterContactMap.find(pDaughterCluster);

            if (neutralClusterContactMap.end() != pastEntryIter)
                neutralClusterContactMap.erase(neutralClusterContactMap.find(pDaughterCluster));
        }

        // Apply simple daughter selection cuts
        if (!pDaughterCluster->GetAssociatedTrackList().empty() || this->IsPhotonLike(pDaughterCluster))
            continue;

        if ((pDaughterCluster->GetNCaloHits() < m_minDaughterCaloHits) || (pDaughterCluster->GetHadronicEnergy() < m_minDaughterHadronicEnergy))
            continue;

        // Calculate the cluster contact information
        for (ClusterList::const_iterator iterJ = pClusterList->begin(), iterJEnd = pClusterList->end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            if (!pParentCluster->GetAssociatedTrackList().empty() || pParentCluster->IsPhotonFast(this->GetPandora()))
                continue;

            const NeutralClusterContact neutralClusterContact(this->GetPandora(), pDaughterCluster, pParentCluster, m_contactParameters);

            if (this->PassesClusterContactCuts(neutralClusterContact))
            {
                neutralClusterContactMap[pDaughterCluster].push_back(neutralClusterContact);
            }
        }
    }
    isFirstPass = false;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool NeutralFragmentRemovalAlgorithm::IsPhotonLike(Cluster *const pDaughterCluster) const
{
    if (pDaughterCluster->IsPhotonFast(this->GetPandora()))
        return true;

    const ClusterFitResult &clusterFitResult(pDaughterCluster->GetFitToAllHitsResult());

    if ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetInnerLayerHitType()) <= FINE) &&
        (pDaughterCluster->GetInnerPseudoLayer() < m_photonLikeMaxInnerLayer) &&
        (clusterFitResult.IsFitSuccessful()) && (clusterFitResult.GetRadialDirectionCosine() > m_photonLikeMinDCosR) &&
        (pDaughterCluster->GetShowerProfileStart(this->GetPandora()) < m_photonLikeMaxShowerStart) &&
        (pDaughterCluster->GetShowerProfileDiscrepancy(this->GetPandora()) < m_photonLikeMaxProfileDiscrepancy))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool NeutralFragmentRemovalAlgorithm::PassesClusterContactCuts(const NeutralClusterContact &neutralClusterContact) const
{
    if (neutralClusterContact.GetDistanceToClosestHit() > m_contactCutMaxDistance)
        return false;

    if ((neutralClusterContact.GetNContactLayers() > m_contactCutNLayers) ||
        (neutralClusterContact.GetConeFraction1() > m_contactCutConeFraction1) ||
        (neutralClusterContact.GetCloseHitFraction1() > m_contactCutCloseHitFraction1) ||
        (neutralClusterContact.GetCloseHitFraction2() > m_contactCutCloseHitFraction2))
    {
        return true;
    }

    return ((neutralClusterContact.GetDistanceToClosestHit() < m_contactCutNearbyDistance) &&
        (neutralClusterContact.GetCloseHitFraction2() > m_contactCutNearbyCloseHitFraction2));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode NeutralFragmentRemovalAlgorithm::GetClusterMergingCandidates(const NeutralClusterContactMap &neutralClusterContactMap, Cluster *&pBestParentCluster,
    Cluster *&pBestDaughterCluster) const
{
    float highestEvidence(m_minEvidence);
    float highestEvidenceParentEnergy(0.);

    for (NeutralClusterContactMap::const_iterator iterI = neutralClusterContactMap.begin(), iterIEnd = neutralClusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = iterI->first;

        for (NeutralClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            NeutralClusterContact neutralClusterContact = *iterJ;

            if (pDaughterCluster != neutralClusterContact.GetDaughterCluster())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            const float evidence(this->GetEvidenceForMerge(neutralClusterContact));

            const float parentEnergy(neutralClusterContact.GetParentCluster()->GetHadronicEnergy());

            if ((evidence > highestEvidence) || ((evidence == highestEvidence) && (parentEnergy > highestEvidenceParentEnergy)))
            {
                highestEvidence = evidence;
                pBestDaughterCluster = pDaughterCluster;
                pBestParentCluster = neutralClusterContact.GetParentCluster();
                highestEvidenceParentEnergy = parentEnergy;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float NeutralFragmentRemovalAlgorithm::GetEvidenceForMerge(const NeutralClusterContact &neutralClusterContact) const
{
    // Calculate a measure of the evidence that the daughter candidate cluster is a fragment of the parent candidate cluster:

    // 1. Layers in contact
    float contactEvidence(0.f);
    if (neutralClusterContact.GetNContactLayers() > m_contactEvidenceNLayers1)
    {
        contactEvidence = m_contactEvidence1;
    }
    else if (neutralClusterContact.GetNContactLayers() > m_contactEvidenceNLayers2)
    {
        contactEvidence = m_contactEvidence2;
    }
    else if (neutralClusterContact.GetNContactLayers() > m_contactEvidenceNLayers3)
    {
        contactEvidence = m_contactEvidence3;
    }
    contactEvidence *= (1.f + neutralClusterContact.GetContactFraction());

    // 2. Cone extrapolation
    float coneEvidence(0.f);
    if (neutralClusterContact.GetConeFraction1() > m_coneEvidenceFraction1)
    {
        coneEvidence = neutralClusterContact.GetConeFraction1() + neutralClusterContact.GetConeFraction2() + neutralClusterContact.GetConeFraction3();

        if (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(neutralClusterContact.GetDaughterCluster()->GetInnerLayerHitType()) <= FINE)
            coneEvidence *= m_coneEvidenceFineGranularityMultiplier;
    }

    // 3. Distance of closest approach
    float distanceEvidence(0.f);
    if (neutralClusterContact.GetDistanceToClosestHit() < m_distanceEvidence1)
    {
        distanceEvidence = (m_distanceEvidence1 - neutralClusterContact.GetDistanceToClosestHit()) / m_distanceEvidence1d;
        distanceEvidence += m_distanceEvidenceCloseFraction1Multiplier * neutralClusterContact.GetCloseHitFraction1();
        distanceEvidence += m_distanceEvidenceCloseFraction2Multiplier * neutralClusterContact.GetCloseHitFraction2();
    }

    return ((m_contactWeight * contactEvidence) + (m_coneWeight * coneEvidence) + (m_distanceWeight * distanceEvidence));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode NeutralFragmentRemovalAlgorithm::GetAffectedClusters(const NeutralClusterContactMap &neutralClusterContactMap, Cluster *const pBestParentCluster,
    Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const
{
    if (neutralClusterContactMap.end() == neutralClusterContactMap.find(pBestDaughterCluster))
        return STATUS_CODE_FAILURE;

    affectedClusters.clear();
    for (NeutralClusterContactMap::const_iterator iterI = neutralClusterContactMap.begin(), iterIEnd = neutralClusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        // Store addresses of all clusters that were in contact with the newly deleted daughter cluster
        if (iterI->first == pBestDaughterCluster)
        {
            for (NeutralClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
            {
                affectedClusters.insert(iterJ->GetParentCluster());
            }
            continue;
        }

        // Also store addresses of all clusters that contained either the parent or daughter clusters in their own NeutralClusterContactVectors
        for (NeutralClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            if ((iterJ->GetParentCluster() == pBestParentCluster) || (iterJ->GetParentCluster() == pBestDaughterCluster))
            {
                affectedClusters.insert(iterI->first);
                break;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

NeutralClusterContact::NeutralClusterContact(const Pandora &pandora, Cluster *const pDaughterCluster, Cluster *const pParentCluster,
        const Parameters &parameters) :
    ClusterContact(pandora, pDaughterCluster, pParentCluster, parameters),
    m_coneFraction2(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle2)),
    m_coneFraction3(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle3))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode NeutralFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Cluster contact parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle1", m_contactParameters.m_coneCosineHalfAngle1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle2", m_contactParameters.m_coneCosineHalfAngle2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle3", m_contactParameters.m_coneCosineHalfAngle3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CloseHitDistance1", m_contactParameters.m_closeHitDistance1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CloseHitDistance2", m_contactParameters.m_closeHitDistance2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosOpeningAngle", m_contactParameters.m_minCosOpeningAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceThreshold", m_contactParameters.m_distanceThreshold));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NMaxPasses", m_nMaxPasses));

    // Initial daughter cluster selection
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterCaloHits", m_minDaughterCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterHadronicEnergy", m_minDaughterHadronicEnergy));

    // Photon-like cuts
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxInnerLayer", m_photonLikeMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMinDCosR", m_photonLikeMinDCosR));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxShowerStart", m_photonLikeMaxShowerStart));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxProfileDiscrepancy", m_photonLikeMaxProfileDiscrepancy));

    // Cluster contact cuts
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMaxDistance", m_contactCutMaxDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNLayers", m_contactCutNLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutConeFraction1", m_contactCutConeFraction1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction1", m_contactCutCloseHitFraction1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction2", m_contactCutCloseHitFraction2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNearbyDistance", m_contactCutNearbyDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNearbyCloseHitFraction2", m_contactCutNearbyCloseHitFraction2));

    // Total evidence: Contact evidence
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers1", m_contactEvidenceNLayers1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers2", m_contactEvidenceNLayers2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers3", m_contactEvidenceNLayers3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence1", m_contactEvidence1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence2", m_contactEvidence2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence3", m_contactEvidence3));

    // Cone evidence
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeEvidenceFraction1", m_coneEvidenceFraction1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeEvidenceFineGranularityMultiplier", m_coneEvidenceFineGranularityMultiplier));

    // Distance of closest approach evidence
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidence1", m_distanceEvidence1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidence1d", m_distanceEvidence1d));

    if (m_distanceEvidence1d < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidenceCloseFraction1Multiplier", m_distanceEvidenceCloseFraction1Multiplier));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidenceCloseFraction2Multiplier", m_distanceEvidenceCloseFraction2Multiplier));

    // Evidence weightings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactWeight", m_contactWeight));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeWeight", m_coneWeight));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceWeight", m_distanceWeight));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinEvidence", m_minEvidence));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
