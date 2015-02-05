/**
 *  @file   LCContent/src/LCClustering/ConeClusteringAlgorithmFast.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCClustering/ConeClusteringAlgorithmFast.h"

#include <list>

using namespace pandora;

//#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(X) std::cout << X << std::endl;
#else
#define DEBUG_PRINT(X) 
#endif

// make those protected calls look less huge / more descriptive
#define RETURN_IF_NOT_SUCCESS(FUNC) \
  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, \
			   !=, \
			   FUNC)

#define RETURN_IF_NOT_SUCCESS_OR_UNCHANGED(FUNC) \
  PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, \
				  STATUS_CODE_UNCHANGED, \
				  !=, FUNC)

#define RETURN_IF_GOOD_OR_BAD_READ(FUNC)		       \
  PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, \
				  STATUS_CODE_NOT_FOUND, \
				  !=, \
				  FUNC)
namespace lc_content_fast
{

ConeClusteringAlgorithm::ConeClusteringAlgorithm() :
    m_clusterSeedStrategy(2),
    m_shouldUseOnlyECalHits(false),
    m_shouldUseIsolatedHits(false),
    m_layersToStepBackFine(3),
    m_layersToStepBackCoarse(3),
    m_clusterFormationStrategy(0),
    m_genericDistanceCut(1.f),
    m_minHitTrackCosAngle(0.f),
    m_minHitClusterCosAngle(0.f),
    m_shouldUseTrackSeed(true),
    m_trackSeedCutOffLayer(0),
    m_shouldFollowInitialDirection(false),
    m_sameLayerPadWidthsFine(2.8f),
    m_sameLayerPadWidthsCoarse(1.8f),
    m_coneApproachMaxSeparation2(1000.f * 1000.f),
    m_tanConeAngleFine(0.3f),
    m_tanConeAngleCoarse(0.5f),
    m_additionalPadWidthsFine(2.5f),
    m_additionalPadWidthsCoarse(2.5f),
    m_maxClusterDirProjection(200.f),
    m_minClusterDirProjection(-10.f),
    m_trackPathWidth(2.f),
    m_maxTrackSeedSeparation2(250.f * 250.f),
    m_maxLayersToTrackSeed(3),
    m_maxLayersToTrackLikeHit(3),
    m_nLayersSpannedForFit(6),
    m_nLayersSpannedForApproxFit(10),
    m_nLayersToFit(8),
    m_nLayersToFitLowMipCut(0.5f),
    m_nLayersToFitLowMipMultiplier(2),
    m_fitSuccessDotProductCut1(0.75f),
    m_fitSuccessChi2Cut1(5.0f),
    m_fitSuccessDotProductCut2(0.50f),
    m_fitSuccessChi2Cut2(2.5f),
    m_mipTrackChi2Cut(2.5f)
{
  DEBUG_PRINT("Built ConeClusteringAlgorithmFast!");
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::Run()
{
  m_firstLayer = (PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());

  DEBUG_PRINT("Start ConeClusteringAlgorithmFast!");
    const CaloHitList *pCaloHitList = nullptr;
    RETURN_IF_NOT_SUCCESS(PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    if (pCaloHitList->empty())
        return STATUS_CODE_SUCCESS;

    const TrackList *pTrackList = nullptr;
    if( 0 != m_clusterSeedStrategy ) {
      RETURN_IF_NOT_SUCCESS(PandoraContentApi::GetCurrentList(*this, pTrackList));
    }

    DEBUG_PRINT("Building KD-trees");
    this->InitializeKDTrees(pTrackList,pCaloHitList);
    DEBUG_PRINT("Built KD-trees");

    OrderedCaloHitList orderedCaloHitList;
    RETURN_IF_NOT_SUCCESS(orderedCaloHitList.Add(*pCaloHitList));
    
    DEBUG_PRINT("Seeding clusters with tracks");
    ClusterVector clusterVector;
    m_tracksToClusters.clear();
    RETURN_IF_NOT_SUCCESS(this->SeedClustersWithTracks(pTrackList,clusterVector));
    DEBUG_PRINT(" there are " << m_tracksToClusters.size() << " tracks mapped to clusters!");
    DEBUG_PRINT("Seeded clusters with tracks");

    // do the clustering
    m_hitsToClusters.clear();    
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const unsigned int pseudoLayer(iter->first);
	DEBUG_PRINT("\trunning ConeClusteringAlgorithmFast on pseudolayer: " << pseudoLayer);
        CustomSortedCaloHitList customSortedCaloHitList;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            if ((m_shouldUseIsolatedHits || !pCaloHit->IsIsolated()) &&
                (!m_shouldUseOnlyECalHits || (ECAL == pCaloHit->GetHitType())) &&
                (PandoraContentApi::IsAvailable(*this, pCaloHit)))
	    {
	        auto pos = std::upper_bound(customSortedCaloHitList.begin(), customSortedCaloHitList.end(), pCaloHit);
                customSortedCaloHitList.insert(pos,pCaloHit);
            }
        }
	
	DEBUG_PRINT("\tbuilt list in PS: " << pseudoLayer << " with " << customSortedCaloHitList.size() << " hits");

        ClusterFitResultMap clusterFitResultMap;
	DEBUG_PRINT("\tGetting ClusterFitResults");
        RETURN_IF_NOT_SUCCESS(this->GetCurrentClusterFitResults(clusterVector, clusterFitResultMap));
	DEBUG_PRINT("\tGot ClusterFitResults");
	DEBUG_PRINT("\tFinding HitsInPreviousLayers");
        RETURN_IF_NOT_SUCCESS(this->FindHitsInPreviousLayers(pseudoLayer, &customSortedCaloHitList, clusterFitResultMap, clusterVector));
	DEBUG_PRINT("\tFound HitsInPreviousLayers");
	DEBUG_PRINT("\tFinding HitsInSameLayer");
        RETURN_IF_NOT_SUCCESS(this->FindHitsInSameLayer(pseudoLayer, &customSortedCaloHitList, clusterFitResultMap, clusterVector));
	DEBUG_PRINT("\tFound HitsInSameLayer");
    }

    RETURN_IF_NOT_SUCCESS(this->RemoveEmptyClusters(clusterVector));

    //reset our kd trees and maps if everything turned out well
    m_tracksKdTree.clear();
    m_hitsKdTree.clear();
    m_hitsToClusters.clear();
    m_tracksToClusters.clear();

    DEBUG_PRINT("End ConeClusteringAlgorithmFast!");

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::InitializeKDTrees(const TrackList* pTrackList, const CaloHitList* pCaloHitList) 
{
    // load the kd-tree of tracks that we will use
    m_tracksKdTree.clear();
    m_trackNodes.clear();
    if( nullptr != pTrackList ) {
      KDTreeCube tracksBoundingRegion = 
	fill_and_bound_3d_kd_tree(this,*pTrackList,m_trackNodes);
      m_tracksKdTree.build(m_trackNodes,tracksBoundingRegion);
      m_trackNodes.clear();
    }

    // make sure the hit kd tree is ready
    m_hitsKdTree.clear();
    m_hitNodes.clear();    
    KDTreeTesseract hitsBoundingRegion = 
      fill_and_bound_4d_kd_tree(this,*pCaloHitList,m_hitNodes);
    m_hitsKdTree.build(m_hitNodes,hitsBoundingRegion);
    m_hitNodes.clear();
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::SeedClustersWithTracks(const TrackList* pTrackList, ClusterVector &clusterVector)
{
    if (0 == m_clusterSeedStrategy)
        return STATUS_CODE_SUCCESS;

    // if we are known to be seeding with tracks we must have a track list
    if( nullptr == pTrackList ) 
      return STATUS_CODE_FAILURE; 
    
    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        if (!pTrack->CanFormPfo())
            continue;

        bool useTrack(false);

        if (2 == m_clusterSeedStrategy)
        {
            useTrack = true;
        }
        else if ((1 == m_clusterSeedStrategy) && pTrack->IsProjectedToEndCap())
        {
            useTrack = true;
        }

        if (useTrack)
        {
            Cluster *pCluster = nullptr;
            PandoraContentApi::Cluster::Parameters parameters;
            parameters.m_pTrack = pTrack;
            RETURN_IF_NOT_SUCCESS(PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
            clusterVector.push_back(pCluster);
	    m_tracksToClusters.emplace(pTrack,pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetCurrentClusterFitResults(ClusterVector &clusterVector, ClusterFitResultMap &clusterFitResultMap) const
{
    if (!clusterFitResultMap.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        ClusterFitResult clusterFitResult;

        if (pCluster->GetNCaloHits() > 1)
        {
            const unsigned int innerLayer(pCluster->GetInnerPseudoLayer());
            const unsigned int outerLayer(pCluster->GetOuterPseudoLayer());
            const unsigned int nLayersSpanned(outerLayer - innerLayer);

            if (nLayersSpanned > m_nLayersSpannedForFit) {
                unsigned int nLayersToFit(m_nLayersToFit);

                if (pCluster->GetMipFraction() - m_nLayersToFitLowMipCut < std::numeric_limits<float>::epsilon())
                    nLayersToFit *= m_nLayersToFitLowMipMultiplier;

                const unsigned int startLayer( (nLayersSpanned > nLayersToFit) ? (outerLayer - nLayersToFit) : innerLayer);
                (void) ClusterFitHelper::FitLayerCentroids(pCluster, startLayer, outerLayer, clusterFitResult);

                if (clusterFitResult.IsFitSuccessful())
                {
                    const float dotProduct(clusterFitResult.GetDirection().GetDotProduct(pCluster->GetInitialDirection()));
                    const float chi2(clusterFitResult.GetChi2());

                    if (((dotProduct < m_fitSuccessDotProductCut1) && (chi2 > m_fitSuccessChi2Cut1)) ||
                        ((dotProduct < m_fitSuccessDotProductCut2) && (chi2 > m_fitSuccessChi2Cut2)) )
                    {
                        clusterFitResult.SetSuccessFlag(false);
                    }
                }
            } else if (nLayersSpanned > m_nLayersSpannedForApproxFit) {
                const CartesianVector centroidChange(pCluster->GetCentroid(outerLayer) - pCluster->GetCentroid(innerLayer));
                clusterFitResult.Reset();
                clusterFitResult.SetDirection(centroidChange.GetUnitVector());
                clusterFitResult.SetSuccessFlag(true);
            }
        }

        if (!clusterFitResultMap.insert(ClusterFitResultMap::value_type(pCluster, clusterFitResult)).second)
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// inprogress
StatusCode ConeClusteringAlgorithm::FindHitsInPreviousLayers(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
								 const ClusterFitResultMap &clusterFitResultMap, ClusterVector & /*clusterVector*/)
{
    const float maxTrackSeedSeparation = std::sqrt(m_maxTrackSeedSeparation2);
    //const float maxConeApproachSeparation = std::sqrt(m_coneApproachMaxSeparation2);
    //const unsigned int firstLayer(PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());
    
    for (CustomSortedCaloHitList::iterator iter = pCustomSortedCaloHitList->begin();
	 iter != pCustomSortedCaloHitList->end();)
    {
        CaloHit *pCaloHit = *iter;
	if (!PandoraContentApi::IsAvailable(*this, pCaloHit)) continue;

	const float additionalPadWidths = ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
					  m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale() : 
					  m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale());
	const float largestAllowedDistanceForSearch = std::max(maxTrackSeedSeparation,m_maxClusterDirProjection+additionalPadWidths);

        Cluster *pBestCluster = nullptr;
        float bestClusterEnergy(0.f);
        float smallestGenericDistance(m_genericDistanceCut);
        const unsigned int layersToStepBack((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            m_layersToStepBackFine : m_layersToStepBackCoarse);

        // Associate with existing clusters in stepBack layers. If stepBackLayer == pseudoLayer, will examine track projections
        for (unsigned int stepBackLayer = 1; (stepBackLayer <= layersToStepBack) && (stepBackLayer <= pseudoLayer); ++stepBackLayer)
        {
            const unsigned int searchLayer(pseudoLayer - stepBackLayer);

	    // need to reorganize this to use a kd-tree 
	    // on rechits comprising clusters we are mutating
	    // goal -> determine search distances for KD-tree 
	    //         from cut values and associated scalings
	    ClusterList nearby_clusters;
	    // search for tracks that would satisfy the search criteria
	    // in GetGenericDistanceToHit()	    
	    KDTreeCube searchRegionTks = 
	      build_3d_kd_search_region(pCaloHit,
					largestAllowedDistanceForSearch,
					largestAllowedDistanceForSearch,
					largestAllowedDistanceForSearch);
	    std::vector<TrackKDNode> found_tracks;
	    m_tracksKdTree.search(searchRegionTks,found_tracks);
	    for( auto& track : found_tracks ) {
	      auto assc_cluster = m_tracksToClusters.find(track.data);
	      if( assc_cluster != m_tracksToClusters.end() ) {
		nearby_clusters.insert(assc_cluster->second);
	      }
	    }
	    found_tracks.clear();
	    // now search for hits-in-clusters that would also satisfy the criteria
	    KDTreeTesseract searchRegionHits = 
	      build_4d_kd_search_region(pCaloHit,
					largestAllowedDistanceForSearch,
					largestAllowedDistanceForSearch,
					largestAllowedDistanceForSearch,
					searchLayer);
	    std::vector<HitKDNode> found_hits;
	    m_hitsKdTree.search(searchRegionHits,found_hits);
	    for( auto& hit : found_hits ) {
	      auto assc_cluster = m_hitsToClusters.find(hit.data);
	      if( assc_cluster != m_hitsToClusters.end() ) {
		nearby_clusters.insert(assc_cluster->second);
	      }
	    }
	    found_hits.clear();
	    
	    // instead of using the full cluster list we use only those clusters
	    // that are found to be nearby according to the KD-tree
	    // ---- This can be optimized further for sure. 
	    // (for instance having a match by KD-tree qualifies a ton of the loops
	    // later
            // See if hit should be associated with any existing clusters
            for (ClusterList::iterator clusterIter = nearby_clusters.begin(), clusterIterEnd = nearby_clusters.end();
		 clusterIter != clusterIterEnd; ++clusterIter)
            {
                Cluster *pCluster = *clusterIter;
                float genericDistance(std::numeric_limits<float>::max());
                const float clusterEnergy(pCluster->GetHadronicEnergy());

                RETURN_IF_NOT_SUCCESS_OR_UNCHANGED(this->GetGenericDistanceToHit(pCluster,
                    pCaloHit, searchLayer, clusterFitResultMap, genericDistance));

                if ( (genericDistance < smallestGenericDistance) || 
		     ( (genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy) ) )
                {
                    pBestCluster = pCluster;
                    bestClusterEnergy = clusterEnergy;
                    smallestGenericDistance = genericDistance;
                }
            }

            // Add best hit found after completing examination of a stepback layer
            if ((0 == m_clusterFormationStrategy) && (nullptr != pBestCluster))
            {
                RETURN_IF_NOT_SUCCESS(PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
		m_hitsToClusters.emplace(pCaloHit,pBestCluster);
                break;
            }
        }

        // Add best hit found after examining all stepback layers
        if ((1 == m_clusterFormationStrategy) && (nullptr != pBestCluster))
        {
            RETURN_IF_NOT_SUCCESS(PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
	    m_hitsToClusters.emplace(pCaloHit,pBestCluster);
        }

	// there is no need to downsize the list when using a vector
	// performance gains are elsewhere, just be careful to not
	// process used hits and guard with IsAvailable()
	++iter;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::FindHitsInSameLayer(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
    const ClusterFitResultMap &clusterFitResultMap, ClusterVector &clusterVector)
{
    const float maxTrackSeedSeparation = std::sqrt(m_maxTrackSeedSeparation2);
    //const float maxConeApproachSeparation = std::sqrt(m_coneApproachMaxSeparation2);

    //keep a list of available hits with the most energetic available hit at the back
    std::list<unsigned> available_hits_in_layer;
    for(unsigned i = 0; i < pCustomSortedCaloHitList->size(); ++i ) {
      if (!PandoraContentApi::IsAvailable(*this, (*pCustomSortedCaloHitList)[i])) continue;
      available_hits_in_layer.push_back(i);
    }
    
    //tactical cache hits -> tracks
    std::unordered_multimap<CaloHit*,Track*> hitsToTracksLocal;

    //tactical cache hits -> hits
    std::unordered_multimap<CaloHit*,CaloHit*> hitsToHitsLocal;

    while (!available_hits_in_layer.empty())
    {
        bool clustersModified = true;

        while (clustersModified)
        {
            clustersModified = false;

            for (auto iter = available_hits_in_layer.begin(); iter != available_hits_in_layer.end();)
            {	      
	        // this his is assured to be usable by the lines above and algorithm course
	        CaloHit *pCaloHit = (*pCustomSortedCaloHitList)[*iter];		

		const float pad_search_width = ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
						(m_sameLayerPadWidthsFine * pCaloHit->GetCellLengthScale()) :
						(m_sameLayerPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );
		/*
		const float additionalPadWidths = ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
						m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale() : 
						m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale());
		*/
		const float track_search_width = maxTrackSeedSeparation;
		const float hit_search_width = pad_search_width;
		

                Cluster *pBestCluster = nullptr;
                float bestClusterEnergy(0.f);
                float smallestGenericDistance(m_genericDistanceCut);

		ClusterList nearby_clusters;
		// search for tracks that would satisfy the search criteria
		// in GetGenericDistanceToHit()	
		auto track_assc_cache = hitsToTracksLocal.find(pCaloHit);
		if( track_assc_cache != hitsToTracksLocal.end() ) {
		  auto range = hitsToTracksLocal.equal_range(pCaloHit);
		  for( auto itr = range.first; itr != range.second; ++itr ) {
		    auto assc_cluster = m_tracksToClusters.find(itr->second);
		    if( assc_cluster != m_tracksToClusters.end() ) {
		      nearby_clusters.insert(assc_cluster->second);
		    }
		  }
		} else {
		  KDTreeCube searchRegionTks = 
		    build_3d_kd_search_region(pCaloHit,
					      track_search_width,
					      track_search_width,
					      track_search_width);
		  std::vector<TrackKDNode> found_tracks;
		  m_tracksKdTree.search(searchRegionTks,found_tracks);
		  for( auto& track : found_tracks ) {
		    hitsToTracksLocal.emplace(pCaloHit,track.data);
		    auto assc_cluster = m_tracksToClusters.find(track.data);
		    if( assc_cluster != m_tracksToClusters.end() ) {
		      nearby_clusters.insert(assc_cluster->second);
		    }
		  }
		  found_tracks.clear();
		}		
		// now search for hits-in-clusters that would also satisfy the criteria
		auto hits_assc_cache = hitsToHitsLocal.find(pCaloHit);
		if( hits_assc_cache != hitsToHitsLocal.end() ) {
		  auto range = hitsToHitsLocal.equal_range(pCaloHit);
		  for( auto itr = range.first; itr != range.second; ++itr ) {
		    auto assc_cluster = m_hitsToClusters.find(itr->second);
		    if( assc_cluster != m_hitsToClusters.end() ) {
		      nearby_clusters.insert(assc_cluster->second);
		    }
		  }
		} else {
		  KDTreeTesseract searchRegionHits = 
		    build_4d_kd_search_region(pCaloHit,
					      hit_search_width,
					      hit_search_width,
					      hit_search_width,
					      pseudoLayer);		
		  std::vector<HitKDNode> found_hits;
		  m_hitsKdTree.search(searchRegionHits,found_hits);
		  for( auto& hit : found_hits ) {
		    hitsToHitsLocal.emplace(pCaloHit,hit.data);
		    auto assc_cluster = m_hitsToClusters.find(hit.data);
		    if( assc_cluster != m_hitsToClusters.end() ) {
		      nearby_clusters.insert(assc_cluster->second);
		    }
		  }
		  found_hits.clear();
		}

                // See if hit should be associated with any existing clusters
                for (ClusterList::iterator clusterIter = nearby_clusters.begin(), clusterIterEnd = nearby_clusters.end();
		     clusterIter != clusterIterEnd; ++clusterIter)
                {
		    Cluster *pCluster = *clusterIter;
                    float genericDistance(std::numeric_limits<float>::max());
                    const float clusterEnergy(pCluster->GetHadronicEnergy());

		    RETURN_IF_NOT_SUCCESS_OR_UNCHANGED(this->GetGenericDistanceToHit(pCluster,
                        pCaloHit, pseudoLayer, clusterFitResultMap, genericDistance));

                    if ((genericDistance < smallestGenericDistance) || ((genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy)))
                    {
                        pBestCluster = pCluster;
                        bestClusterEnergy = clusterEnergy;
                        smallestGenericDistance = genericDistance;
                    }
                }

                if (nullptr != pBestCluster)
                {
                    RETURN_IF_NOT_SUCCESS(PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
		    m_hitsToClusters.emplace(pCaloHit,pBestCluster);
		    // remove this hit and advance to the next
		    iter = available_hits_in_layer.erase(iter);
                    clustersModified = true;
                } else { // otherwise advance the iterator
		  ++iter; 
		}               
            }
        }

	// if there is no cluster within the search radius
        // Seed a new cluster with this hit
        if (!available_hits_in_layer.empty())
        {	    	    
	    unsigned index = *(available_hits_in_layer.begin());
	    available_hits_in_layer.pop_front();
	    CaloHit* pCaloHit = (*pCustomSortedCaloHitList)[index];
	    // hit is assured to be valid
            Cluster *pCluster = nullptr;
            PandoraContentApi::Cluster::Parameters parameters;
            parameters.m_caloHitList.insert(pCaloHit);
            RETURN_IF_NOT_SUCCESS(PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
            clusterVector.push_back(pCluster);
	    m_hitsToClusters.emplace(pCaloHit,pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetGenericDistanceToHit(Cluster *const pCluster, CaloHit *const pCaloHit, const unsigned int searchLayer,
    const ClusterFitResultMap &clusterFitResultMap, float &genericDistance) const
{
    const unsigned int firstLayer = m_firstLayer;

    // Use position of track projection at calorimeter. Proceed only if projection is reasonably compatible with calo hit
    if (((searchLayer == 0) || (searchLayer < firstLayer)) && pCluster->IsTrackSeeded())
    {
        const TrackState &trackState(pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter());
        const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

        if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(trackDirection) < m_minHitTrackCosAngle)
            return STATUS_CODE_UNCHANGED;

        return this->GetConeApproachDistanceToHit(pCaloHit, trackState.GetPosition(), trackDirection, genericDistance);
    }

    // Check that cluster is occupied in the searchlayer and is reasonably compatible with calo hit
    OrderedCaloHitList::const_iterator clusterHitListIter = pCluster->GetOrderedCaloHitList().find(searchLayer);

    if (pCluster->GetOrderedCaloHitList().end() == clusterHitListIter)
        return STATUS_CODE_UNCHANGED;

    ClusterFitResultMap::const_iterator fitResultIter(clusterFitResultMap.find(pCluster));
    const ClusterFitResult* clusterFitResult((clusterFitResultMap.end() != fitResultIter) ? &(fitResultIter->second) : nullptr);
    const CartesianVector &clusterDirection( (clusterFitResult != nullptr && clusterFitResult->IsFitSuccessful()) ?  
					     clusterFitResult->GetDirection() : pCluster->GetInitialDirection()     );

    if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(clusterDirection) < m_minHitClusterCosAngle)
        return STATUS_CODE_UNCHANGED;

    // Cone approach measurements
    float initialDirectionDistance(std::numeric_limits<float>::max());
    float currentDirectionDistance(std::numeric_limits<float>::max());
    float trackSeedDistance(std::numeric_limits<float>::max());

    const bool useTrackSeed(m_shouldUseTrackSeed && pCluster->IsTrackSeeded());
    const bool followInitialDirection(m_shouldFollowInitialDirection && pCluster->IsTrackSeeded() && (searchLayer > m_trackSeedCutOffLayer));

    if (!useTrackSeed || (searchLayer > m_trackSeedCutOffLayer))
    {
        const CaloHitList *pClusterCaloHitList = clusterHitListIter->second;

        if (searchLayer == pCaloHit->GetPseudoLayer())
        {
            return this->GetDistanceToHitInSameLayer(pCaloHit, pClusterCaloHitList, genericDistance);
        }

        // Measurement using initial cluster direction
        StatusCode initialStatusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList, pCluster->GetInitialDirection(),
            initialDirectionDistance);

        if (STATUS_CODE_SUCCESS == initialStatusCode)
        {
            if (followInitialDirection)
                initialDirectionDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != initialStatusCode)
        {
            return initialStatusCode;
        }

        // Measurement using current cluster direction
        if (clusterFitResult != nullptr && clusterFitResult->IsFitSuccessful())
        {
            StatusCode currentStatusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList, clusterFitResult->GetDirection(),
                currentDirectionDistance);

            if (STATUS_CODE_SUCCESS == currentStatusCode)
            {
                if ((currentDirectionDistance < m_genericDistanceCut) && pCluster->IsTrackSeeded())
                {
                    try
                    {
                        if (clusterFitResult->GetChi2() < m_mipTrackChi2Cut)
                            currentDirectionDistance /= 5.;
                    }
                    catch (StatusCodeException &) {}
                }
            }
            else if (STATUS_CODE_UNCHANGED != currentStatusCode)
            {
                return currentStatusCode;
            }
        }
    }

    // Seed track distance measurements
    if (useTrackSeed && !followInitialDirection)
    {
        StatusCode trackStatusCode = this->GetDistanceToTrackSeed(pCluster, pCaloHit, searchLayer, trackSeedDistance);

        if (STATUS_CODE_SUCCESS == trackStatusCode)
        {
            if (trackSeedDistance < m_genericDistanceCut)
                trackSeedDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != trackStatusCode)
        {
            return trackStatusCode;
        }
    }

    // Identify best measurement of generic distance
    const float smallestDistance(std::min(trackSeedDistance, std::min(initialDirectionDistance, currentDirectionDistance)));
    if (smallestDistance < genericDistance)
    {
        genericDistance = smallestDistance;

        if (std::numeric_limits<float>::max() != genericDistance)
            return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToHitInSameLayer(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    float &distance) const
{
    const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        (m_sameLayerPadWidthsFine * pCaloHit->GetCellLengthScale()) :
        (m_sameLayerPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

    if (dCut < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_FAILURE;

    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

    bool hitFound(false);
    float smallestDistanceSquared(std::numeric_limits<float>::max());
    const float rDCutSquared(1.f / (dCut * dCut));

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        const CartesianVector &hitInClusterPosition(pHitInCluster->GetPositionVector());
        const float separationSquared((hitPosition - hitInClusterPosition).GetMagnitudeSquared());
        const float hitDistanceSquared(separationSquared * rDCutSquared);

        if (hitDistanceSquared < smallestDistanceSquared)
        {
            smallestDistanceSquared = hitDistanceSquared;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = std::sqrt(smallestDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    const CartesianVector &clusterDirection, float &distance) const
{
    bool hitFound(false);
    float smallestDistance(std::numeric_limits<float>::max());

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        float hitDistance(std::numeric_limits<float>::max());

	RETURN_IF_NOT_SUCCESS_OR_UNCHANGED(this->GetConeApproachDistanceToHit(pCaloHit,
            pHitInCluster->GetPositionVector(), clusterDirection, hitDistance));

        if (hitDistance < smallestDistance)
        {
            smallestDistance = hitDistance;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = smallestDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CartesianVector &clusterPosition,
    const CartesianVector &clusterDirection, float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector positionDifference(hitPosition - clusterPosition);

    if (positionDifference.GetMagnitudeSquared() > m_coneApproachMaxSeparation2)
        return STATUS_CODE_UNCHANGED;

    const float dAlong(clusterDirection.GetDotProduct(positionDifference));

    if ((dAlong < m_maxClusterDirProjection) && (dAlong > m_minClusterDirProjection))
    {
        const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            (std::fabs(dAlong) * m_tanConeAngleFine) + (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            (std::fabs(dAlong) * m_tanConeAngleCoarse) + (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (dCut < std::numeric_limits<float>::epsilon())
            return STATUS_CODE_FAILURE;

        const float dPerp (clusterDirection.GetCrossProduct(positionDifference).GetMagnitude());

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, unsigned int searchLayer,
    float &distance) const
{
    if (searchLayer < m_maxLayersToTrackSeed)
        return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);

    const int searchLayerInt(static_cast<int>(searchLayer));
    const int startLayer(std::max(0, searchLayerInt - static_cast<int>(m_maxLayersToTrackLikeHit)));

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();

    for (int iLayer = startLayer; iLayer < searchLayerInt; ++iLayer)
    {
        OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.find(iLayer);
        if (orderedCaloHitList.end() == listIter)
            continue;

        for (CaloHitList::const_iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
        {
            float tempDistance(std::numeric_limits<float>::max());
	    RETURN_IF_NOT_SUCCESS_OR_UNCHANGED(this->GetDistanceToTrackSeed(pCluster, *iter,
                tempDistance));

            if (tempDistance < m_genericDistanceCut)
                return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);
        }
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector &trackSeedPosition(pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter().GetPosition());

    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separationSquared(positionDifference.GetMagnitudeSquared());

    if (separationSquared < m_maxTrackSeedSeparation2)
    {
        const float flexibility(1.f + (m_trackPathWidth * std::sqrt(separationSquared / m_maxTrackSeedSeparation2)));

        const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            flexibility * (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            flexibility * (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (dCut < std::numeric_limits<float>::epsilon())
            return STATUS_CODE_FAILURE;

        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::RemoveEmptyClusters(ClusterVector &clusterVector) const
{
    ClusterList clusterDeletionList;

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        if (0 == (*iter)->GetNCaloHits())
        {
            clusterDeletionList.insert(*iter);
            (*iter) = nullptr;
        }
    }

    if (!clusterDeletionList.empty())
    {
        RETURN_IF_NOT_SUCCESS(PandoraContentApi::Delete(*this, &clusterDeletionList));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Track seeding parameters
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ClusterSeedStrategy", m_clusterSeedStrategy));

    // High level clustering parameters
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ShouldUseOnlyECalHits", m_shouldUseOnlyECalHits));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackFine", m_layersToStepBackFine));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackCoarse", m_layersToStepBackCoarse));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ClusterFormationStrategy", m_clusterFormationStrategy));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MinHitTrackCosAngle", m_minHitTrackCosAngle));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MinHitClusterCosAngle", m_minHitClusterCosAngle));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ShouldUseTrackSeed", m_shouldUseTrackSeed));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "TrackSeedCutOffLayer", m_trackSeedCutOffLayer));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ShouldFollowInitialDirection", m_shouldFollowInitialDirection));

    // Same layer distance parameters
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsFine", m_sameLayerPadWidthsFine));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsCoarse", m_sameLayerPadWidthsCoarse));

    // Cone approach distance parameters
    float coneApproachMaxSeparation = std::sqrt(m_coneApproachMaxSeparation2);
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "ConeApproachMaxSeparation", coneApproachMaxSeparation));
    m_coneApproachMaxSeparation2 = coneApproachMaxSeparation * coneApproachMaxSeparation;

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleFine", m_tanConeAngleFine));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleCoarse", m_tanConeAngleCoarse));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsFine", m_additionalPadWidthsFine));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsCoarse", m_additionalPadWidthsCoarse));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDirProjection", m_maxClusterDirProjection));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MinClusterDirProjection", m_minClusterDirProjection));

    // Track seed distance parameters
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    float maxTrackSeedSeparation = std::sqrt(m_maxTrackSeedSeparation2);
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeedSeparation", maxTrackSeedSeparation));
    m_maxTrackSeedSeparation2 = maxTrackSeedSeparation * maxTrackSeedSeparation;

    if (m_shouldUseTrackSeed && (m_maxTrackSeedSeparation2 < std::numeric_limits<float>::epsilon()))
        return STATUS_CODE_INVALID_PARAMETER;

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackSeed", m_maxLayersToTrackSeed));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackLikeHit", m_maxLayersToTrackLikeHit));

    // Cluster current direction and mip track parameters
    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForFit", m_nLayersSpannedForFit));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForApproxFit", m_nLayersSpannedForApproxFit));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "NLayersToFit", m_nLayersToFit));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipCut", m_nLayersToFitLowMipCut));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipMultiplier", m_nLayersToFitLowMipMultiplier));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut1", m_fitSuccessDotProductCut1));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut1", m_fitSuccessChi2Cut1));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut2", m_fitSuccessDotProductCut2));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut2", m_fitSuccessChi2Cut2));

    RETURN_IF_GOOD_OR_BAD_READ(XmlHelper::ReadValue(xmlHandle,
        "MipTrackChi2Cut", m_mipTrackChi2Cut));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
