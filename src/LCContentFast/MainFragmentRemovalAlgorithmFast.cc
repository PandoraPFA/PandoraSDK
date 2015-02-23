/**
 *  @file   LCContent/src/LCFragmentRemoval/MainFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the main fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCContentFast/MainFragmentRemovalAlgorithmFast.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/ReclusterHelper.h"

#include <cstdlib>
#include <memory>

using namespace pandora;

using lc_content::ClusterHelper;
using lc_content::ReclusterHelper;

namespace lc_content_fast
{

MainFragmentRemovalAlgorithm::MainFragmentRemovalAlgorithm() :
    m_minDaughterCaloHits(5),
    m_minDaughterHadronicEnergy(0.025f),
    m_contactCutMaxDistance(750.f),
    m_contactCutNLayers(0),
    m_contactCutConeFraction1(0.25f),
    m_contactCutCloseHitFraction1(0.25f),
    m_contactCutCloseHitFraction2(0.15f),
    m_contactCutMeanDistanceToHelix(250.f),
    m_contactCutClosestDistanceToHelix(150.f),
    m_contactCutMaxHitDistance(250.f),
    m_contactCutMinDaughterInnerLayer(19),
    m_maxChi2(16.f),
    m_maxGlobalChi2(9.f),
    m_chi2Base(5.f),
    m_globalChi2Penalty(5.f),
    m_correctionLayerNHitLayers(3),
    m_correctionLayerEnergyFraction(0.25f),
    m_contactEvidenceNLayers1(10),
    m_contactEvidenceNLayers2(4),
    m_contactEvidenceNLayers3(1),
    m_contactEvidence1(2.f),
    m_contactEvidence2(1.f),
    m_contactEvidence3(0.5f),
    m_coneEvidenceFraction1(0.5f),
    m_coneEvidenceFineGranularityMultiplier(0.5f),
    m_closestTrackEvidence1(200.f),
    m_closestTrackEvidence1d(100.f),
    m_closestTrackEvidence2(50.f),
    m_closestTrackEvidence2d(20.f),
    m_meanTrackEvidence1(200.f),
    m_meanTrackEvidence1d(100.f),
    m_meanTrackEvidence2(50.f),
    m_meanTrackEvidence2d(50.f),
    m_distanceEvidence1(100.f),
    m_distanceEvidence1d(100.f),
    m_distanceEvidenceCloseFraction1Multiplier(1.f),
    m_distanceEvidenceCloseFraction2Multiplier(2.f),
    m_contactWeight(1.f),
    m_coneWeight(1.f),
    m_distanceWeight(1.f),
    m_trackExtrapolationWeight(1.f),
    m_layerCorrectionLayerValue1(15),
    m_layerCorrectionLayerValue2(30),
    m_layerCorrectionLayerValue3(50),
    m_layerCorrection1(2.f),
    m_layerCorrection2(0.f),
    m_layerCorrection3(-1.f),
    m_layerCorrection4(-2.f),
    m_layerCorrectionLayerSpan(4),
    m_layerCorrectionMinInnerLayer(5),
    m_layerCorrection5(-2.f),
    m_leavingCorrection(5.f),
    m_energyCorrectionThreshold(3.f),
    m_lowEnergyCorrectionThreshold(1.5f),
    m_lowEnergyCorrectionNHitLayers1(6),
    m_lowEnergyCorrectionNHitLayers2(4),
    m_lowEnergyCorrectionNHitLayers3(29),
    m_lowEnergyCorrection1(-1.f),
    m_lowEnergyCorrection2(-1.f),
    m_lowEnergyCorrection3(-1.f),
    m_angularCorrectionOffset(0.75f),
    m_angularCorrectionConstant(-0.5f),
    m_angularCorrectionGradient(2.f),
    m_photonCorrectionEnergy1(2.f),
    m_photonCorrectionEnergy2(0.5f),
    m_photonCorrectionEnergy3(1.f),
    m_photonCorrectionShowerStart1(5.f),
    m_photonCorrectionShowerStart2(2.5f),
    m_photonCorrectionShowerDiscrepancy1(0.8f),
    m_photonCorrectionShowerDiscrepancy2(1.f),
    m_photonCorrection1(10.f),
    m_photonCorrection2(100.f),
    m_photonCorrection3(5.f),
    m_photonCorrection4(10.f),
    m_photonCorrection5(2.f),
    m_photonCorrection6(2.f),
    m_photonCorrection7(0.f),
    m_minRequiredEvidence(0.5f)
{
    m_contactParameters.m_coneCosineHalfAngle1 = 0.9f;
    m_contactParameters.m_coneCosineHalfAngle2 = 0.95f;
    m_contactParameters.m_coneCosineHalfAngle3 = 0.985f;
    m_contactParameters.m_closeHitDistance1 = 100.f;
    m_contactParameters.m_closeHitDistance2 = 50.f;
    m_contactParameters.m_minCosOpeningAngle = 0.5f;
    m_contactParameters.m_distanceThreshold = 2.f;
    m_contactParameters.m_helixComparisonMipFractionCut = 0.8f;
    m_contactParameters.m_helixComparisonStartOffset = 20;
    m_contactParameters.m_helixComparisonStartOffsetMip = 20;
    m_contactParameters.m_nHelixComparisonLayers = 9;
    m_contactParameters.m_maxLayersCrossedByHelix = 100;
    m_contactParameters.m_maxTrackClusterDeltaZ = 250.f;

    const float maxTrackClusterHitDist = std::max(m_contactParameters.m_maxTrackClusterDeltaZ,std::max(m_contactCutClosestDistanceToHelix,m_contactCutMeanDistanceToHelix));
    const float maxHitToHitDistance = std::max(m_contactCutMaxHitDistance,std::max(m_contactParameters.m_closeHitDistance1,m_contactParameters.m_closeHitDistance2));
    // take the minimum distance between the largest allowed hit-hit distance and the maximum hit-hit or hit-cluster distance
    m_minimalSearchRadius = std::min(m_contactCutMaxDistance,std::max(maxTrackClusterHitDist,maxHitToHitDistance));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::Run()
{
    bool isFirstPass(true), shouldRecalculate(true);

    ClusterList affectedClusters;
    ChargedClusterContactMap chargedClusterContactMap;

    // setup fast search utilities    
    ClusterToClusterMap clusters_to_clusters;
    ClusterToNeighbourClustersMap neighbours_cache;

    // need to so some of the searching with vectors (hash table too slow)
    HitKDTreeByIndex hits_kdtree_byindex;
    std::vector<HitKDNodeByIndex> hit_nodes_by_index;
    std::vector<const CaloHit*> hits_by_index;
    std::vector<const Cluster*> hits_to_clusters_by_hit_index;

    //save local kd-trees of each cluster that we will update over time
    // this will hopefully speed up ContactMap calculation
    ClusterToKDTreeMap clusters_to_hit_tree;    

    // get the *starting* cluster list 
    const ClusterList *pClusterList = nullptr;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
    neighbours_cache.reserve(pClusterList->size());
    clusters_to_clusters.reserve(pClusterList->size());

    for( auto clusterIter = pClusterList->begin(); clusterIter != pClusterList->end(); ++clusterIter ) {
      clusters_to_clusters.emplace(*clusterIter,*clusterIter);
      const auto& tree = clusters_to_hit_tree.emplace(*clusterIter,std::unique_ptr<HitKDTree>(new HitKDTree())).first->second;
      std::array<float,3> minpos{ {0.0f,0.0f,0.0f} }, maxpos{ {0.0f,0.0f,0.0f} };
      std::vector<HitKDNode> nodes_for_local_tree;
      CaloHitList temp;      
      (*clusterIter)->GetOrderedCaloHitList().GetCaloHitList(temp);
      unsigned nhits = 0;
      for( auto* hit : temp ) {
	const CartesianVector& pos = hit->GetPositionVector();
	nodes_for_local_tree.emplace_back(hit,pos.GetX(),pos.GetY(),pos.GetZ());
	if( nhits == 0 ) {
	  minpos[0] = pos.GetX(); minpos[1] = pos.GetY(); minpos[2] = pos.GetZ();
	  maxpos[0] = pos.GetX(); maxpos[1] = pos.GetY(); maxpos[2] = pos.GetZ();
	} else {
	  minpos[0] = std::min((float)pos.GetX(),minpos[0]);
	  minpos[1] = std::min((float)pos.GetY(),minpos[1]);
	  minpos[2] = std::min((float)pos.GetZ(),minpos[2]);
	  maxpos[0] = std::max((float)pos.GetX(),maxpos[0]);
	  maxpos[1] = std::max((float)pos.GetY(),maxpos[1]);
	  maxpos[2] = std::max((float)pos.GetZ(),maxpos[2]);
	}
	hits_by_index.emplace_back(hit);
	hits_to_clusters_by_hit_index.emplace_back(*clusterIter);	
	++nhits;
      }
      KDTreeCube clusterBoundingBox(minpos[0],maxpos[0],
				    minpos[1],maxpos[1],
				    minpos[2],maxpos[2]);
      tree->build(nodes_for_local_tree,clusterBoundingBox);
    }
    KDTreeCube hitsByIndexBoundingRegion =
      fill_and_bound_3d_kd_tree_by_index(hits_by_index,hit_nodes_by_index);
    hits_kdtree_byindex.build(hit_nodes_by_index,hitsByIndexBoundingRegion);
    hit_nodes_by_index.clear();    
    
    // now we build the neighbours cache so that we can efficiently search in the inner loop
    for( auto clusterIter = pClusterList->begin(); clusterIter != pClusterList->end(); ++clusterIter ) {
      CaloHitList hits;
      (*clusterIter)->GetOrderedCaloHitList().GetCaloHitList(hits);
      ClusterList neighbours;
      for( auto* hit : hits ) {
	KDTreeCube hitSearchRegion = build_3d_kd_search_region(hit,
							       m_minimalSearchRadius,
							       m_minimalSearchRadius,
							       m_minimalSearchRadius );
	std::vector<HitKDNodeByIndex> found_hits;
	hits_kdtree_byindex.search(hitSearchRegion,found_hits);
	for( auto& found_hit : found_hits ) {	  	  
	  auto* pNeighbour = hits_to_clusters_by_hit_index[found_hit.data];
	  if( *(clusterIter) != pNeighbour ) { // make sure this is a neighbour
	    neighbours.insert(pNeighbour);
	  }
	}
      }
      neighbours_cache.emplace(*clusterIter,std::move(neighbours));
    }

    while (shouldRecalculate)
    {
        shouldRecalculate = false;
        const Cluster *pBestParentCluster(NULL), *pBestDaughterCluster(NULL);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetChargedClusterContactMap(isFirstPass, affectedClusters, chargedClusterContactMap, clusters_to_clusters, neighbours_cache, clusters_to_hit_tree));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterMergingCandidates(chargedClusterContactMap, pBestParentCluster,
            pBestDaughterCluster));

        if ((NULL != pBestParentCluster) && (NULL != pBestDaughterCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusters(chargedClusterContactMap, pBestParentCluster,
                pBestDaughterCluster, affectedClusters));

            chargedClusterContactMap.erase(chargedClusterContactMap.find(pBestDaughterCluster));
	    // update the cluster to cluster to effective remove this cluster	    
	    clusters_to_clusters[pBestDaughterCluster] = pBestParentCluster;
	    shouldRecalculate = true;

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster,
                pBestDaughterCluster));
	    // update the parent cluster kd tree
	    auto& tree = clusters_to_hit_tree[pBestParentCluster];
	    tree.reset(new HitKDTree());
	    CaloHitList temp;
	    std::vector<HitKDNode> hit_nodes;
	    pBestParentCluster->GetOrderedCaloHitList().GetCaloHitList(temp);
	    KDTreeCube clusterBoundingBox =
	      fill_and_bound_3d_kd_tree(temp,hit_nodes);
	    tree->build(hit_nodes,clusterBoundingBox);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetChargedClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters,
	ChargedClusterContactMap &chargedClusterContactMap, const ClusterToClusterMap& clusters_to_clusters, 
	const ClusterToNeighbourClustersMap& neighbours_cache, const ClusterToKDTreeMap& cluster_to_tree) const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Filter current cluster list to exclude muon candidates
    ClusterList clusterList;
    
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const Cluster *pCluster = *iter;
        const ParticleId *pParticleId(PandoraContentApi::GetPlugins(*this)->GetParticleId());

        if (!pParticleId->IsMuon(pCluster) && !pParticleId->IsElectron(pCluster))
        {
            clusterList.insert(pCluster);
        }	
    }

    // Create cluster contacts
    for (ClusterList::const_iterator iterI = clusterList.begin(), iterIEnd = clusterList.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *pDaughterCluster = *iterI;

        // Identify whether cluster contacts need to be recalculated
        if (!isFirstPass)
        {
            if (affectedClusters.end() == affectedClusters.find(pDaughterCluster))
                continue;

            ChargedClusterContactMap::iterator pastEntryIter = chargedClusterContactMap.find(pDaughterCluster);

            if (chargedClusterContactMap.end() != pastEntryIter)
                chargedClusterContactMap.erase(pastEntryIter);
        }
	
        // Apply simple daughter selection cuts
        if (!pDaughterCluster->GetAssociatedTrackList().empty())
            continue;

        if ((pDaughterCluster->GetNCaloHits() < m_minDaughterCaloHits) || (pDaughterCluster->GetHadronicEnergy() < m_minDaughterHadronicEnergy))
            continue;

	// find our nearby clusters in the neighbours cache	
	ClusterList nearby_clusters;
	const auto& original_neighbours = neighbours_cache.find(pDaughterCluster)->second;
	for( auto* neighbour : original_neighbours ) {
	  nearby_clusters.insert( clusters_to_clusters.find(neighbour)->second );
	}

        // Calculate the cluster contact information
	
        for (auto iterJ = nearby_clusters.begin(), iterJEnd = nearby_clusters.end(); iterJ != iterJEnd; ++iterJ)
        {
	    const Cluster *pParentCluster = *iterJ;	    

            if (pDaughterCluster == pParentCluster )
                continue;

            if (pParentCluster->GetAssociatedTrackList().empty())
                continue;
	    	    
	    const auto& tree = cluster_to_tree.find(pParentCluster)->second;

            ChargedClusterContact chargedClusterContact(this->GetPandora(), pDaughterCluster, pParentCluster, m_contactParameters, tree);

            if (this->PassesClusterContactCuts(chargedClusterContact))
            {
	        chargedClusterContactMap[pDaughterCluster].push_back(std::move(chargedClusterContact));
            }	    
        }
    }
    isFirstPass = false;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MainFragmentRemovalAlgorithm::PassesClusterContactCuts(const ChargedClusterContact &chargedClusterContact) const
{
    if (chargedClusterContact.GetDistanceToClosestHit() > m_contactCutMaxDistance)
        return false;

    if ((chargedClusterContact.GetNContactLayers() > m_contactCutNLayers) ||
        (chargedClusterContact.GetConeFraction1() > m_contactCutConeFraction1) ||
        (chargedClusterContact.GetCloseHitFraction1() > m_contactCutCloseHitFraction1) ||
        (chargedClusterContact.GetCloseHitFraction2() > m_contactCutCloseHitFraction2) ||
        (chargedClusterContact.GetMeanDistanceToHelix() < m_contactCutMeanDistanceToHelix) ||
        (chargedClusterContact.GetClosestDistanceToHelix() < m_contactCutClosestDistanceToHelix))
    {
        return true;
    }

    return ((chargedClusterContact.GetDistanceToClosestHit() < m_contactCutMaxHitDistance) &&
        (chargedClusterContact.GetDaughterCluster()->GetInnerPseudoLayer() > m_contactCutMinDaughterInnerLayer));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetClusterMergingCandidates(const ChargedClusterContactMap &chargedClusterContactMap, const Cluster *&pBestParentCluster,
    const Cluster *&pBestDaughterCluster)
{
    float highestExcessEvidence(0.f);
    float highestEvidenceParentEnergy(0.);

    for (ChargedClusterContactMap::const_iterator iterI = chargedClusterContactMap.begin(), iterIEnd = chargedClusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *pDaughterCluster = iterI->first;
        float globalDeltaChi2(0.f);

        // Check to see if merging parent and daughter clusters would improve track-cluster compatibility
        if (!this->PassesPreselection(pDaughterCluster, iterI->second, globalDeltaChi2))
            continue;

        const unsigned int daughterCorrectionLayer(this->GetClusterCorrectionLayer(pDaughterCluster));

        for (ChargedClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            ChargedClusterContact chargedClusterContact = *iterJ;

            if (pDaughterCluster != chargedClusterContact.GetDaughterCluster())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            const float totalEvidence(this->GetTotalEvidenceForMerge(chargedClusterContact));
            const float requiredEvidence(this->GetRequiredEvidenceForMerge(pDaughterCluster, chargedClusterContact, daughterCorrectionLayer,
                globalDeltaChi2));
            const float excessEvidence(totalEvidence - requiredEvidence);

            const float parentEnergy(chargedClusterContact.GetParentCluster()->GetHadronicEnergy());

            if ((excessEvidence > highestExcessEvidence) || ((excessEvidence == highestExcessEvidence) && (parentEnergy > highestEvidenceParentEnergy)))
            {
                highestExcessEvidence = excessEvidence;
                pBestDaughterCluster = pDaughterCluster;
                pBestParentCluster = chargedClusterContact.GetParentCluster();
                highestEvidenceParentEnergy = parentEnergy;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MainFragmentRemovalAlgorithm::PassesPreselection(const Cluster *const pDaughterCluster, const ChargedClusterContactVector &chargedClusterContactVector,
    float &globalDeltaChi2) const
{
    bool passesPreselection(false);
    float totalTrackEnergy(0.f), totalClusterEnergy(0.f);
    const float daughterClusterEnergy(pDaughterCluster->GetTrackComparisonEnergy(this->GetPandora()));

    // Check to see if merging parent and daughter clusters would improve track-cluster compatibility
    for (ChargedClusterContactVector::const_iterator iter = chargedClusterContactVector.begin(), iterEnd = chargedClusterContactVector.end(); iter != iterEnd;
        ++iter)
    {
        ChargedClusterContact chargedClusterContact = *iter;
        const float parentTrackEnergy(chargedClusterContact.GetParentTrackEnergy());
        const float parentClusterEnergy(chargedClusterContact.GetParentCluster()->GetTrackComparisonEnergy(this->GetPandora()));

        const float oldChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), parentClusterEnergy, parentTrackEnergy));
        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), daughterClusterEnergy + parentClusterEnergy, parentTrackEnergy));

        const float oldChi2(oldChi * oldChi);
        const float newChi2(newChi * newChi);

        if ((newChi2 < m_maxChi2) || (newChi2 < oldChi2))
            passesPreselection = true;

        totalTrackEnergy += parentTrackEnergy;
        totalClusterEnergy += parentClusterEnergy;
    }

    // Check again using total energies of all contact clusters and their associated tracks
    const float oldChiTotal(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), totalClusterEnergy, totalTrackEnergy));
    const float newChiTotal(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), daughterClusterEnergy + totalClusterEnergy, totalTrackEnergy));

    const float oldChi2Total(oldChiTotal * oldChiTotal);
    const float newChi2Total(newChiTotal * newChiTotal);

    globalDeltaChi2 = oldChi2Total - newChi2Total;

    if ((newChi2Total < m_maxGlobalChi2) || (newChi2Total < oldChi2Total))
        passesPreselection = true;

    return passesPreselection;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetTotalEvidenceForMerge(const ChargedClusterContact &chargedClusterContact) const
{
    // Calculate a measure of the evidence that the daughter candidate cluster is a fragment of the parent candidate cluster:

    // 1. Layers in contact
    float contactEvidence(0.f);
    if (chargedClusterContact.GetNContactLayers() > m_contactEvidenceNLayers1)
    {
        contactEvidence = m_contactEvidence1;
    }
    else if (chargedClusterContact.GetNContactLayers() > m_contactEvidenceNLayers2)
    {
        contactEvidence = m_contactEvidence2;
    }
    else if (chargedClusterContact.GetNContactLayers() > m_contactEvidenceNLayers3)
    {
        contactEvidence = m_contactEvidence3;
    }
    contactEvidence *= (1.f + chargedClusterContact.GetContactFraction());

    // 2. Cone extrapolation
    float coneEvidence(0.f);
    if (chargedClusterContact.GetConeFraction1() > m_coneEvidenceFraction1)
    {
        coneEvidence = chargedClusterContact.GetConeFraction1() + chargedClusterContact.GetConeFraction2() + chargedClusterContact.GetConeFraction3();

        if (PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(chargedClusterContact.GetDaughterCluster()->GetInnerLayerHitType()) <= FINE)
            coneEvidence *= m_coneEvidenceFineGranularityMultiplier;
    }

    // 3. Track extrapolation
    float trackExtrapolationEvidence(0.f);
    if (chargedClusterContact.GetClosestDistanceToHelix() < m_closestTrackEvidence1)
    {
        trackExtrapolationEvidence = (m_closestTrackEvidence1 - chargedClusterContact.GetClosestDistanceToHelix()) / m_closestTrackEvidence1d;

        if (chargedClusterContact.GetClosestDistanceToHelix() < m_closestTrackEvidence2)
            trackExtrapolationEvidence += (m_closestTrackEvidence2 - chargedClusterContact.GetClosestDistanceToHelix()) / m_closestTrackEvidence2d;

        trackExtrapolationEvidence += (m_meanTrackEvidence1 - chargedClusterContact.GetMeanDistanceToHelix()) / m_meanTrackEvidence1d;

        if (chargedClusterContact.GetMeanDistanceToHelix() < m_meanTrackEvidence2)
            trackExtrapolationEvidence += (m_meanTrackEvidence2 - chargedClusterContact.GetMeanDistanceToHelix()) / m_meanTrackEvidence2d;
    }

    // 4. Distance of closest approach
    float distanceEvidence(0.f);
    if (chargedClusterContact.GetDistanceToClosestHit() < m_distanceEvidence1)
    {
        distanceEvidence = (m_distanceEvidence1 - chargedClusterContact.GetDistanceToClosestHit()) / m_distanceEvidence1d;
        distanceEvidence += m_distanceEvidenceCloseFraction1Multiplier * chargedClusterContact.GetCloseHitFraction1();
        distanceEvidence += m_distanceEvidenceCloseFraction2Multiplier * chargedClusterContact.GetCloseHitFraction2();
    }

    return ((m_contactWeight * contactEvidence) + (m_coneWeight * coneEvidence) + (m_distanceWeight * distanceEvidence) +
        (m_trackExtrapolationWeight * trackExtrapolationEvidence));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetRequiredEvidenceForMerge(const Cluster *const pDaughterCluster, const ChargedClusterContact &chargedClusterContact,
    const unsigned int correctionLayer, const float globalDeltaChi2)
{
    // Primary evidence requirement is obtained from change in chi2.
    const float daughterCorrectedClusterEnergy(pDaughterCluster->GetTrackComparisonEnergy(this->GetPandora()));
    const float parentCorrectedClusterEnergy(chargedClusterContact.GetParentCluster()->GetTrackComparisonEnergy(this->GetPandora()));
    const float parentTrackEnergy(chargedClusterContact.GetParentTrackEnergy());

    const float oldChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), parentCorrectedClusterEnergy, parentTrackEnergy));
    const float newChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), daughterCorrectedClusterEnergy + parentCorrectedClusterEnergy, parentTrackEnergy));

    const float oldChi2(oldChi * oldChi);
    const float newChi2(newChi * newChi);

    const float chi2Evidence(m_chi2Base - (oldChi2 - newChi2));
    const float globalChi2Evidence(m_chi2Base + m_globalChi2Penalty - globalDeltaChi2);
    const bool usingGlobalChi2(((newChi2 > oldChi2) && (newChi2 > m_maxGlobalChi2)) || (globalChi2Evidence < chi2Evidence));

    // Final evidence requirement is corrected to account for following factors:
    // 1. Layer corrections
    float layerCorrection(0.f);

    if (correctionLayer < m_layerCorrectionLayerValue1)
    {
        layerCorrection = m_layerCorrection1;
    }
    else if (correctionLayer < m_layerCorrectionLayerValue2)
    {
        layerCorrection = m_layerCorrection2;
    }
    else if (correctionLayer < m_layerCorrectionLayerValue3)
    {
        layerCorrection = m_layerCorrection3;
    }
    else
    {
        layerCorrection = m_layerCorrection4;
    }

    const unsigned int innerLayer(pDaughterCluster->GetInnerPseudoLayer());
    const unsigned int outerLayer(pDaughterCluster->GetOuterPseudoLayer());

    if ((outerLayer - innerLayer < m_layerCorrectionLayerSpan) && (innerLayer > m_layerCorrectionMinInnerLayer))
        layerCorrection = m_layerCorrection5;

    // 2. Leaving cluster corrections
    float leavingCorrection(0.f);

    if (ClusterHelper::IsClusterLeavingDetector(chargedClusterContact.GetParentCluster()))
        leavingCorrection = m_leavingCorrection;

    // 3. Energy correction
    float energyCorrection(0.f);
    const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

    if (daughterClusterEnergy < m_energyCorrectionThreshold)
        energyCorrection = daughterClusterEnergy - m_energyCorrectionThreshold;

    // 4. Low energy fragment corrections
    float lowEnergyCorrection(0.f);

    if (daughterClusterEnergy < m_lowEnergyCorrectionThreshold)
    {
        const unsigned int nHitLayers(pDaughterCluster->GetOrderedCaloHitList().size());

        if (nHitLayers < m_lowEnergyCorrectionNHitLayers1)
            lowEnergyCorrection += m_lowEnergyCorrection1;

        if (nHitLayers < m_lowEnergyCorrectionNHitLayers2)
            lowEnergyCorrection += m_lowEnergyCorrection2;

        if (correctionLayer > m_lowEnergyCorrectionNHitLayers3)
            lowEnergyCorrection += m_lowEnergyCorrection3;
    }

    // 5. Angular corrections
    float angularCorrection(0.f);
    const float radialDirectionCosine(pDaughterCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pDaughterCluster->GetFitToAllHitsResult().GetRadialDirectionCosine() : 0.f);

    if (radialDirectionCosine < m_angularCorrectionOffset)
        angularCorrection = m_angularCorrectionConstant + (radialDirectionCosine - m_angularCorrectionOffset) * m_angularCorrectionGradient;

    // 6. Photon cluster corrections
    float photonCorrection(0.f);

    if (pDaughterCluster->IsPhotonFast(this->GetPandora()))
    {
        const float showerStart(pDaughterCluster->GetShowerProfileStart(this->GetPandora()));
        const float showerDiscrepancy(pDaughterCluster->GetShowerProfileDiscrepancy(this->GetPandora()));

        if (daughterClusterEnergy > m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart1)
            photonCorrection = m_photonCorrection1;

        if (daughterClusterEnergy > m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection2;

        if (daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection3;

        if (daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2 && showerDiscrepancy < m_photonCorrectionShowerDiscrepancy1)
            photonCorrection = m_photonCorrection4;

        if (daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart > m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection5;

        if (daughterClusterEnergy < m_photonCorrectionEnergy2 && (showerStart > m_photonCorrectionShowerStart2 || showerDiscrepancy > m_photonCorrectionShowerDiscrepancy2))
            photonCorrection = m_photonCorrection6;

        if (daughterClusterEnergy < m_photonCorrectionEnergy3 && showerStart > m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection7;
    }

    const float requiredEvidence(usingGlobalChi2 ?
        globalChi2Evidence + layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection :
        chi2Evidence + layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection + lowEnergyCorrection);

    return std::max(m_minRequiredEvidence, requiredEvidence);
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int MainFragmentRemovalAlgorithm::GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const
{
    float energySum(0.f);
    unsigned int layerCounter(0);

    const float totalClusterEnergy(pDaughterCluster->GetHadronicEnergy());
    const OrderedCaloHitList &orderedCaloHitList(pDaughterCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            energySum += (*hitIter)->GetHadronicEnergy();
        }

        if ((++layerCounter >= m_correctionLayerNHitLayers) || (energySum > m_correctionLayerEnergyFraction * totalClusterEnergy))
        {
            return iter->first;
        }
    }

    return pDaughterCluster->GetInnerPseudoLayer();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetAffectedClusters(const ChargedClusterContactMap &chargedClusterContactMap, const Cluster *const pBestParentCluster,
    const Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const
{
    if (chargedClusterContactMap.end() == chargedClusterContactMap.find(pBestDaughterCluster))
        return STATUS_CODE_FAILURE;

    affectedClusters.clear();
    for (ChargedClusterContactMap::const_iterator iterI = chargedClusterContactMap.begin(), iterIEnd = chargedClusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        // Store addresses of all clusters that were in contact with the newly deleted daughter cluster
        if (iterI->first == pBestDaughterCluster)
        {
            for (ChargedClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
            {
                affectedClusters.insert(iterJ->GetParentCluster());
            }
            continue;
        }

        // Also store addresses of all clusters that contained either the parent or daughter clusters in their own ChargedClusterContactVectors
        for (ChargedClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
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

ChargedClusterContact::ChargedClusterContact(const Pandora &pandora, const Cluster *const pDaughterCluster, const Cluster *const pParentCluster,
        const Parameters &parameters) :
    ClusterContact(pandora, pDaughterCluster, pParentCluster, parameters),
    m_coneFraction2(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle2)),
    m_coneFraction3(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle3)),
    m_meanDistanceToHelix(std::numeric_limits<float>::max()),
    m_closestDistanceToHelix(std::numeric_limits<float>::max())
{
    this->ClusterHelixComparison(pandora, pDaughterCluster, pParentCluster, parameters);
}

ChargedClusterContact::ChargedClusterContact(const Pandora &pandora, const Cluster *const pDaughterCluster, const Cluster *const pParentCluster,
        const Parameters &parameters, const std::unique_ptr<HitKDTree>& hit_tree) :
    ClusterContact(pandora, pDaughterCluster, pParentCluster, parameters, hit_tree),
    m_coneFraction2(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle2)),
    m_coneFraction3(FragmentRemovalHelper::GetFractionOfHitsInCone(pandora, pDaughterCluster, pParentCluster, parameters.m_coneCosineHalfAngle3)),
    m_meanDistanceToHelix(std::numeric_limits<float>::max()),
    m_closestDistanceToHelix(std::numeric_limits<float>::max())
{
    this->ClusterHelixComparison(pandora, pDaughterCluster, pParentCluster, parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ChargedClusterContact::ClusterHelixComparison(const Pandora &pandora, const Cluster *const pDaughterCluster, const Cluster *const pParentCluster,
    const Parameters &parameters)
{
    // Configure range of layers in which daughter cluster will be compared to helix fits
    const bool passMipFractionCut(pParentCluster->GetMipFraction() - parameters.m_helixComparisonMipFractionCut > std::numeric_limits<float>::epsilon());

    const unsigned int startLayer(pDaughterCluster->GetInnerPseudoLayer());
    const unsigned int endLayer(passMipFractionCut ?
        std::max(startLayer + parameters.m_helixComparisonStartOffset, pParentCluster->GetOuterPseudoLayer() + parameters.m_helixComparisonStartOffsetMip) :
        startLayer + parameters.m_helixComparisonStartOffset);

    const float clusterZPosition(pDaughterCluster->GetCentroid(startLayer).GetZ());
    const unsigned int maxOccupiedLayers(passMipFractionCut ? std::numeric_limits<unsigned int>::max() : parameters.m_nHelixComparisonLayers);

    // Calculate closest distance between daughter cluster and helix fits to parent associated tracks
    float trackEnergySum(0.);
    const TrackList &parentTrackList(pParentCluster->GetAssociatedTrackList());

    for (TrackList::const_iterator iter = parentTrackList.begin(), iterEnd = parentTrackList.end(); iter != iterEnd; ++iter)
    {
        // Extract track information
        trackEnergySum += (*iter)->GetEnergyAtDca();
        const Helix *const pHelix = (*iter)->GetHelixFitAtCalorimeter();
        const float trackCalorimeterZPosition((*iter)->GetTrackStateAtCalorimeter().GetPosition().GetZ());

        // Check proximity of track projection and cluster
        if ((std::fabs(trackCalorimeterZPosition) > (std::fabs(clusterZPosition) + parameters.m_maxTrackClusterDeltaZ)) ||
            (trackCalorimeterZPosition * clusterZPosition < 0.f))
        {
            continue;
        }

        // Check number of layers crossed by helix
        const unsigned int nLayersCrossed(FragmentRemovalHelper::GetNLayersCrossed(pandora, pHelix, trackCalorimeterZPosition, clusterZPosition));

        if (nLayersCrossed > parameters.m_maxLayersCrossedByHelix)
            continue;

        // Calculate distance to helix
        float meanDistanceToHelix(std::numeric_limits<float>::max()), closestDistanceToHelix(std::numeric_limits<float>::max());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::GetClusterHelixDistance(pDaughterCluster, pHelix,
            startLayer, endLayer, maxOccupiedLayers, closestDistanceToHelix, meanDistanceToHelix));

        if (closestDistanceToHelix < m_closestDistanceToHelix)
        {
            m_meanDistanceToHelix = meanDistanceToHelix;
            m_closestDistanceToHelix = closestDistanceToHelix;
        }
    }

    m_parentTrackEnergy = trackEnergySum;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
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
        "HelixComparisonMipFractionCut", m_contactParameters.m_helixComparisonMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HelixComparisonStartOffset", m_contactParameters.m_helixComparisonStartOffset));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HelixComparisonStartOffsetMip", m_contactParameters.m_helixComparisonStartOffsetMip));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NHelixComparisonLayers", m_contactParameters.m_nHelixComparisonLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersCrossedByHelix", m_contactParameters.m_maxLayersCrossedByHelix));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackClusterDeltaZ", m_contactParameters.m_maxTrackClusterDeltaZ));

    // Initial daughter cluster selection
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterCaloHits", m_minDaughterCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterHadronicEnergy", m_minDaughterHadronicEnergy));

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
        "ContactCutMeanDistanceToHelix", m_contactCutMeanDistanceToHelix));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutClosestDistanceToHelix", m_contactCutClosestDistanceToHelix));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMaxHitDistance", m_contactCutMaxHitDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMinDaughterInnerLayer", m_contactCutMinDaughterInnerLayer));

    // Track-cluster consistency Chi2 values
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxChi2", m_maxChi2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxGlobalChi2", m_maxGlobalChi2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2Base", m_chi2Base));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GlobalChi2Penalty", m_globalChi2Penalty));

    // Correction layer parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CorrectionLayerNHitLayers", m_correctionLayerNHitLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CorrectionLayerEnergyFraction", m_correctionLayerEnergyFraction));

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

    // Track extrapolation evidence
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence1", m_closestTrackEvidence1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence1d", m_closestTrackEvidence1d));

    if (m_closestTrackEvidence1d < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence2", m_closestTrackEvidence2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence2d", m_closestTrackEvidence2d));

    if (m_closestTrackEvidence2d < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence1", m_meanTrackEvidence1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence1d", m_meanTrackEvidence1d));

    if (m_meanTrackEvidence1d < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence2", m_meanTrackEvidence2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence2d", m_meanTrackEvidence2d));

    if (m_meanTrackEvidence2d < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

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
        "TrackExtrapolationWeight", m_trackExtrapolationWeight));

    // Required evidence: Layer correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayerValue1", m_layerCorrectionLayerValue1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayerValue2", m_layerCorrectionLayerValue2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayerValue3", m_layerCorrectionLayerValue3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection1", m_layerCorrection1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection2", m_layerCorrection2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection3", m_layerCorrection3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection4", m_layerCorrection4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayerSpan", m_layerCorrectionLayerSpan));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionMinInnerLayer", m_layerCorrectionMinInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection5", m_layerCorrection5));

    // Leaving correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LeavingCorrection", m_leavingCorrection));

    // Energy correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyCorrectionThreshold", m_energyCorrectionThreshold));

    // Low energy correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionThreshold", m_lowEnergyCorrectionThreshold));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionNHitLayers1", m_lowEnergyCorrectionNHitLayers1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionNHitLayers2", m_lowEnergyCorrectionNHitLayers2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionNHitLayers3", m_lowEnergyCorrectionNHitLayers3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection1", m_lowEnergyCorrection1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection2", m_lowEnergyCorrection2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection3", m_lowEnergyCorrection3));

    // Angular correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionOffset", m_angularCorrectionOffset));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionConstant", m_angularCorrectionConstant));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionGradient", m_angularCorrectionGradient));

    // Photon correction
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy1", m_photonCorrectionEnergy1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy2", m_photonCorrectionEnergy2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy3", m_photonCorrectionEnergy3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerStart1", m_photonCorrectionShowerStart1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerStart2", m_photonCorrectionShowerStart2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerDiscrepancy1", m_photonCorrectionShowerDiscrepancy1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerDiscrepancy2", m_photonCorrectionShowerDiscrepancy2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection1", m_photonCorrection1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection2", m_photonCorrection2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection3", m_photonCorrection3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection4", m_photonCorrection4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection5", m_photonCorrection5));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection6", m_photonCorrection6));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection7", m_photonCorrection7));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinRequiredEvidence", m_minRequiredEvidence));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
