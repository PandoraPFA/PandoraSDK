/**
 *  @file   LCContent/src/LCTopologicalAssociation/HighEnergyPhotonRecoveryAlgorithm.cc
 * 
 *  @brief  Implementation of the high energy photon recovery algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"
#include "PandoraMonitoringApi.h"
#include "LCTopologicalAssociation/HighEnergyPhotonRecoveryAlgorithm.h"

using namespace pandora;

namespace lc_content
{

HighEnergyPhotonRecoveryAlgorithm::HighEnergyPhotonRecoveryAlgorithm() :
    m_shouldUseCurrentClusterList(true),
    m_updateCurrentTrackClusterAssociations(true),
    m_numberContactLayers(2),
    m_centroidDistance2Cut(30000),
    m_fullClosestApproachCut(100.f),
    m_contactClosestApproachCut(100.f),
    m_minRmsRatioCut(0.3f),
    m_maxRmsRatioCut(5.f),
    m_daughterDistance2ToParentFitCut(2000.f),
    m_energyRatioCut(0.1f),
    m_fractionInConeCut(0.5f),
    m_coneCosineHalfAngle(0.9f),
    m_minCosConeAngleWrtRadial(0.25f),
    m_cosConeAngleWrtRadialCut1(0.5f),
    m_minHitSeparationCut1(std::sqrt(1000.f)),
    m_cosConeAngleWrtRadialCut2(0.75f),
    m_minHitSeparationCut2(std::sqrt(1500.f))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::Run()
{
    // Read specified lists of input clusters
    ClusterListToNameMap clusterListToNameMap;
    ClusterList clusterList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListAndNameMap(clusterList,clusterListToNameMap));

    // Then prepare clusters for this merging algorithm
    ClusterVector daughterVector,parentVector;
    ////ClusterParametersMap parentClusterParametersMap;    
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClusters(clusterList,daughterVector, parentVector));
    
    ClusterClusterMultiMap parentCandidateMultiMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PreSelectClusters(daughterVector, parentVector,parentCandidateMultiMap))
    
    // important to check if it is empty
    if (parentCandidateMultiMap.empty())
        return STATUS_CODE_SUCCESS;
        
    ClusterClusterMap daughterBestParentMap;
    for (ClusterVector::const_iterator iterI = daughterVector.begin(), iterIEnd = daughterVector.end(); iterI != iterIEnd; ++iterI)
    {
        // Daughter = HCal Fragments
        const Cluster *const pDaughterCluster = *iterI;
        const ClusterFitResult &daughterClusterFitResult(pDaughterCluster->GetFitToAllHitsResult());
        ClusterFitResult daughterFirstLayerFitResult;            
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitStart(pDaughterCluster, m_numberContactLayers, daughterFirstLayerFitResult))
            continue;
        if (!daughterClusterFitResult.IsFitSuccessful() || !daughterFirstLayerFitResult.IsFitSuccessful())
            continue;
        const float daughterFirstLayerRms(daughterFirstLayerFitResult.GetRms());
        float maxFractionInCone(0.f);
        const Cluster *pBestParentCluster(NULL);

        std::pair <ClusterClusterMultiMap::const_iterator, ClusterClusterMultiMap::const_iterator> parentCandidateRange;
        parentCandidateRange = parentCandidateMultiMap.equal_range(pDaughterCluster);        
        for (ClusterClusterMultiMap::const_iterator iterJ = parentCandidateRange.first, iterJEnd = parentCandidateRange.second; iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pParentCluster = iterJ->second;

            const ClusterFitResult &parentClusterFitResult(pParentCluster->GetFitToAllHitsResult());
            ClusterFitResult parentLastLayerFitResult;            
            if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitEnd(pParentCluster, m_numberContactLayers, parentLastLayerFitResult))
                continue;
            if (!parentClusterFitResult.IsFitSuccessful() || !parentLastLayerFitResult.IsFitSuccessful())
                continue;  

            const float parentLastLayerRms(parentLastLayerFitResult.GetRms());
            if (parentLastLayerRms<std::numeric_limits<float>::min()) 
                continue;
            
            const float rmsRatio( daughterFirstLayerRms/parentLastLayerRms);
            if (rmsRatio>m_maxRmsRatioCut || rmsRatio<m_minRmsRatioCut ) continue;

            float fullClosestApproach(std::numeric_limits<float>::max()),contactClosestApproach(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetFitResultsClosestApproach(daughterClusterFitResult, parentClusterFitResult, fullClosestApproach))
                continue;
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetFitResultsClosestApproach(daughterFirstLayerFitResult, parentLastLayerFitResult, contactClosestApproach))
                continue;
            if (fullClosestApproach>m_fullClosestApproachCut && contactClosestApproach>m_contactClosestApproachCut) continue;

            const float daughterDistance2ToParentFit(this->GetHadEnergyWeightedDistance2ToLine(pDaughterCluster, parentClusterFitResult));            
            if (daughterDistance2ToParentFit>m_daughterDistance2ToParentFitCut) continue;
            
            float fractionInCone(GetFractionInCone(pParentCluster, pDaughterCluster, parentClusterFitResult) );
            if (fractionInCone>maxFractionInCone){
                maxFractionInCone = fractionInCone;
                pBestParentCluster = pParentCluster;
            }

        }
        if (pBestParentCluster!=NULL && maxFractionInCone > m_fractionInConeCut ){
            if (!daughterBestParentMap.insert(ClusterClusterMap::value_type(pDaughterCluster, pBestParentCluster)).second)
                return STATUS_CODE_FAILURE;
        }
    }
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MergeClusters(daughterBestParentMap, clusterListToNameMap));    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::GetClusterListAndNameMap(ClusterList &clusterList, ClusterListToNameMap &clusterListToNameMap) const
{
    if (m_shouldUseCurrentClusterList)
    {
        std::string clusterListName;
        const ClusterList *pClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList, clusterListName));

        clusterList.insert(pClusterList->begin(), pClusterList->end());
        clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, clusterListName));

        if (m_updateCurrentTrackClusterAssociations)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));
        }
    }
    
    for (StringVector::const_iterator iter = m_additionalClusterListNames.begin(), iterEnd = m_additionalClusterListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;
        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetList(*this, *iter, pClusterList))
        {
            clusterList.insert(pClusterList->begin(), pClusterList->end());
            clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, *iter));
        }
        else
        {
            std::cout << "HighEnergyPhotonRecoveryAlgorithm: Failed to obtain cluster list " << *iter << std::endl;
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::PrepareClusters(const ClusterList &clusterList, ClusterVector &daughterVector, ClusterVector &parentVector) const
{
    for (ClusterList::const_iterator clusterIter = clusterList.begin(), clusterIterEnd = clusterList.end(); clusterIter != clusterIterEnd; ++clusterIter)
    {
        const Cluster *pCluster = *clusterIter;
        if (PHOTON == pCluster->GetParticleIdFlag())
        {               
            if (ECAL == pCluster->GetInnerLayerHitType()  && ECAL == pCluster->GetOuterLayerHitType() && pCluster->GetElectromagneticEnergy()>0.f)
            {
                parentVector.push_back(pCluster);
            }            
            continue;
        }
        if (!pCluster->GetAssociatedTrackList().empty())
        {
            continue;
        }
        if( pCluster->GetElectromagneticEnergy() < 0.f)
        {
            continue;
        }
        if (pCluster->IsPhotonFast(this->GetPandora()))
        {
            continue;
        }
        else
        {
            if (HCAL == pCluster->GetInnerLayerHitType()  && HCAL == pCluster->GetOuterLayerHitType() && pCluster->GetHadronicEnergy()>0.f)
                daughterVector.push_back(pCluster);
            continue;
        }
    }     
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
StatusCode HighEnergyPhotonRecoveryAlgorithm::PreSelectClusters(const ClusterVector &daughterVector, const ClusterVector &parentVector, ClusterClusterMultiMap &parentCandidateMultiMap) const
{
    for (ClusterVector::const_iterator iterI = daughterVector.begin(), iterIEnd = daughterVector.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *const pDaughterCluster = *iterI;
        const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());       
        const CartesianVector &centroidDaughterFirstLayer(pDaughterCluster->GetCentroid(daughterInnerLayer));
        for (ClusterVector::const_iterator iterJ = parentVector.begin(), iterJEnd = parentVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pParentCluster = *iterJ;
            const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
            if (daughterInnerLayer!=parentOuterLayer+1) continue;
            if (pDaughterCluster->GetHadronicEnergy() / pParentCluster->GetElectromagneticEnergy() > m_energyRatioCut) continue;
            
            const CartesianVector &centroidParentLastLayer(pParentCluster->GetCentroid(parentOuterLayer));
            const float centroidDistance2( (centroidParentLastLayer - centroidDaughterFirstLayer).GetMagnitudeSquared());
            if (centroidDistance2 > m_centroidDistance2Cut) continue;
            // past pre selection cuts
            if (parentCandidateMultiMap.insert(ClusterClusterMultiMap::value_type(pDaughterCluster, pParentCluster )) == parentCandidateMultiMap.end())
                return STATUS_CODE_FAILURE;
        }
    }
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::MergeClusters(const ClusterClusterMap &daughterBestParentMap, const ClusterListToNameMap &clusterListToNameMap) const
{
    for (ClusterClusterMap::const_iterator iter = daughterBestParentMap.begin(), iterEnd = daughterBestParentMap.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pDaughterCluster = iter->first;
        const Cluster *const pBestParentCluster = iter->second;
        if (clusterListToNameMap.size() > 1)
        {
            std::string parentListName, daughterListName;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pBestParentCluster, clusterListToNameMap, parentListName));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pDaughterCluster, clusterListToNameMap, daughterListName));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster,
                parentListName, daughterListName));
        }
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
        }
    }
    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

float HighEnergyPhotonRecoveryAlgorithm::GetFractionInCone(const Cluster *const pParentCluster, const Cluster *const pDaughterCluster,
    const ClusterFitResult &parentMipFitResult) const
{
    const unsigned int nDaughterCaloHits(pDaughterCluster->GetNCaloHits());

    if (0 == nDaughterCaloHits)
        return 0.;

    // Identify cone vertex
    const CartesianVector &parentMipFitDirection(parentMipFitResult.GetDirection());
    const CartesianVector &parentMipFitIntercept(parentMipFitResult.GetIntercept());

    const CartesianVector showerStartDifference(pParentCluster->GetCentroid(pParentCluster->GetShowerStartLayer(this->GetPandora())) - parentMipFitIntercept);
    const float parallelDistanceToShowerStart(showerStartDifference.GetDotProduct(parentMipFitDirection));
    const CartesianVector coneApex(parentMipFitIntercept + (parentMipFitDirection * parallelDistanceToShowerStart));

    // Don't allow large distance associations at low angle
    const float cosConeAngleWrtRadial(coneApex.GetUnitVector().GetDotProduct(parentMipFitDirection));
    if (cosConeAngleWrtRadial < m_minCosConeAngleWrtRadial)
        return 0.;

    // Count daughter cluster hits in cone
    unsigned int nHitsInCone(0);
    float minHitSeparation(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pDaughterCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector positionDifference((*hitIter)->GetPositionVector() - coneApex);
            const float hitSeparation(positionDifference.GetMagnitude());

            if (hitSeparation < std::numeric_limits<float>::epsilon())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            if (hitSeparation < minHitSeparation)
                minHitSeparation = hitSeparation;

            const float cosTheta(parentMipFitDirection.GetDotProduct(positionDifference) / hitSeparation);

            if (cosTheta > m_coneCosineHalfAngle)
                nHitsInCone++;
        }
    }
    // Further checks to prevent large distance associations at low angle
    if ( ((cosConeAngleWrtRadial < m_cosConeAngleWrtRadialCut1) && (minHitSeparation > m_minHitSeparationCut1)) ||
         ((cosConeAngleWrtRadial < m_cosConeAngleWrtRadialCut2) && (minHitSeparation > m_minHitSeparationCut2)) )
    {
        return 0.;
    }

    return static_cast<float>(nHitsInCone) / static_cast<float>(nDaughterCaloHits);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::GetClusterListName(const Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
    std::string &listName) const
{
    for (ClusterListToNameMap::const_iterator iter = clusterListToNameMap.begin(), iterEnd = clusterListToNameMap.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *const pClusterList = iter->first;

        if (pClusterList->end() != pClusterList->find(pCluster))
        {
            listName = iter->second;
            return STATUS_CODE_SUCCESS;
        }
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float HighEnergyPhotonRecoveryAlgorithm::GetHadEnergyWeightedDistance2ToLine(const Cluster *const pCluster, const ClusterFitResult &clusterFitResult) const
{
    float energySum(0.f);
    float weightedDistance2(0.f);
    
    const CartesianVector &direction(clusterFitResult.GetDirection());
    const CartesianVector &intercept(clusterFitResult.GetIntercept());
    
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hIter = iter->second->begin(), hIterEnd = iter->second->end(); hIter != hIterEnd; ++hIter)
        {
            const CaloHit *const pCaloHit(*hIter);
            energySum += pCaloHit->GetHadronicEnergy();
            weightedDistance2 += this->GetDistance2ToLine(pCaloHit->GetPositionVector(),direction,intercept ) * pCaloHit->GetHadronicEnergy();
        }
    }

    if (energySum < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return (weightedDistance2 / energySum / static_cast<float>(pCluster->GetNCaloHits()) );
}
//------------------------------------------------------------------------------------------------------------------------------------------

float HighEnergyPhotonRecoveryAlgorithm::GetHadEnergyWeightedDistance2ToLine(const Cluster *const pCluster, const ClusterFitResult &clusterFitResult,
        unsigned int pseudoLayer) const
{
    float energySum(0.f);
    float weightedDistance2(0.f);

    const CartesianVector &direction(clusterFitResult.GetDirection());
    const CartesianVector &intercept(clusterFitResult.GetIntercept());

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    OrderedCaloHitList::const_iterator iterLayer = orderedCaloHitList.find(pseudoLayer);

    for (CaloHitList::const_iterator hIter = iterLayer->second->begin(), hIterEnd = iterLayer->second->end(); hIter != hIterEnd; ++hIter)
    {
        const CaloHit *const pCaloHit(*hIter);
        energySum += pCaloHit->GetHadronicEnergy();
        weightedDistance2 += this->GetDistance2ToLine(pCaloHit->GetPositionVector(),direction,intercept ) * pCaloHit->GetHadronicEnergy();
    }

    if (energySum < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return (weightedDistance2 / energySum / static_cast<float>(pCluster->GetNCaloHits()) );
}

//------------------------------------------------------------------------------------------------------------------------------------------

float HighEnergyPhotonRecoveryAlgorithm::GetDistance2ToLine(const CartesianVector &point, const CartesianVector &direction,
    const CartesianVector &intercept ) const
{
    const float directionMag2(direction.GetMagnitudeSquared());
    const CartesianVector pointIntercept(intercept-point);
    const float pointInterceptMag2(pointIntercept.GetMagnitudeSquared());
    const float pointInterceptDotDirection(pointIntercept.GetDotProduct(direction));    
    
    if (directionMag2 < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
        
    return (pointInterceptMag2 - pointInterceptDotDirection * pointInterceptDotDirection / directionMag2);
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighEnergyPhotonRecoveryAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseCurrentClusterList", m_shouldUseCurrentClusterList));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpdateCurrentTrackClusterAssociations", m_updateCurrentTrackClusterAssociations));

    if (m_shouldUseCurrentClusterList && m_updateCurrentTrackClusterAssociations)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, 
            m_trackClusterAssociationAlgName));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "AdditionalClusterListNames", m_additionalClusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NumberContactLayers", m_numberContactLayers));
        
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CentroidDistance2Cut", m_centroidDistance2Cut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FullClosestApproachCut", m_fullClosestApproachCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactClosestApproachCut", m_contactClosestApproachCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinRmsRatioCut", m_minRmsRatioCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxRmsRatioCut", m_maxRmsRatioCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DaughterDistance2ToParentFitCut", m_daughterDistance2ToParentFitCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyRatioCut", m_energyRatioCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FractionInConeCut", m_fractionInConeCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle", m_coneCosineHalfAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosConeAngleWrtRadial", m_minCosConeAngleWrtRadial));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CosConeAngleWrtRadialCut1", m_cosConeAngleWrtRadialCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitSeparationCut1", m_minHitSeparationCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CosConeAngleWrtRadialCut2", m_cosConeAngleWrtRadialCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitSeparationCut2", m_minHitSeparationCut2));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
