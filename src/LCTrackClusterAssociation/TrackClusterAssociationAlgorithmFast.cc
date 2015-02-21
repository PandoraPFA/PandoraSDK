/**
 *  @file   LCContent/src/LCTrackClusterAssociation/TrackClusterAssociationAlgorithm.cc
 * 
 *  @brief  Implementation of the track-cluster association algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCUtility/KDTreeLinkerAlgoT.h"
#include <unordered_map>

#include "LCTrackClusterAssociation/TrackClusterAssociationAlgorithmFast.h"

// setup templates for tracking track:pseudolayer pair
namespace std {
  template<>
  struct hash<std::pair<const pandora::Track*,unsigned> > {
    std::size_t operator()(std::pair<const pandora::Track*,unsigned> const& tps) const 
    {
      std::size_t h1 = std::hash<const pandora::Track*>()(tps.first);
      std::size_t h2 = std::hash<unsigned>()(tps.second);
      return h1 ^ (h2 << 1);
    }
  };
}

namespace {
  typedef KDTreeLinkerAlgo<pandora::CaloHit*,4> HitKDTree;
  typedef KDTreeNodeInfoT<pandora::CaloHit*,4> HitKDNode;
  typedef std::unordered_multimap<std::pair<const pandora::Track*,unsigned>,pandora::CaloHit*> TracksToHitsInPseudoLayerMap;
  typedef std::unordered_map<pandora::CaloHit*,pandora::Cluster*> HitsToClustersMap;
}

using namespace pandora;
using lc_content::SortingHelper;
using lc_content::ClusterHelper;

namespace lc_content_fast
{

TrackClusterAssociationAlgorithm::TrackClusterAssociationAlgorithm() :
    m_lowEnergyCut(0.2f),
    m_maxTrackClusterDistance(10.f),
    m_maxSearchLayer(9),
    m_parallelDistanceCut(100.f),
    m_minTrackClusterCosAngle(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackClusterAssociationAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), SortingHelper::SortTracksByEnergy);

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Clear any existing track - cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

    HitKDTree hits_kdtree;
    std::vector<HitKDNode> hit_nodes;
    CaloHitList hit_list;
    TracksToHitsInPseudoLayerMap tracks_to_hits;
    HitsToClustersMap hits_to_clusters;

    // build the kd-tree of hits from the input clusters
    // save the map of hits to clusters
    for(ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterIterEnd = pClusterList->end();
	clusterIter != clusterIterEnd; ++clusterIter) {
      CaloHitList temp;
      (*clusterIter)->GetOrderedCaloHitList().GetCaloHitList(temp);
      for( auto* hit : temp ) {
	hit_list.insert(hit);
	hits_to_clusters.emplace(hit, *clusterIter);
      }
    }
    KDTreeTesseract hitsBoundingRegion = 
      fill_and_bound_4d_kd_tree(this,hit_list,hit_nodes,true);
    hit_list.clear();
    hits_kdtree.build(hit_nodes,hitsBoundingRegion);
    hit_nodes.clear();

    // Look to make new associations
    for (TrackVector::const_iterator trackIter = trackVector.begin(), trackIterEnd = trackVector.end(); trackIter != trackIterEnd; ++trackIter)
    {
        Track *pTrack = *trackIter;

        // Use only tracks that can be used to form a pfo
        if (!pTrack->CanFormPfo())
            continue;

        if (!pTrack->GetDaughterTrackList().empty())
            continue;

	const TrackState &trackState(pTrack->GetTrackStateAtCalorimeter());
	const CartesianVector &trackPosition(trackState.GetPosition());

        Cluster *pBestCluster = NULL;
        Cluster *pBestLowEnergyCluster = NULL;

        float minDistance(m_maxTrackClusterDistance);
        float minLowEnergyDistance(m_maxTrackClusterDistance);

        float minEnergyDifference(std::numeric_limits<float>::max());
        float minLowEnergyDifference(std::numeric_limits<float>::max());

	// short circuit this loop with a kd-tree search beforehand
	ClusterList nearby_clusters;
	// iterating over a std::map is stupidly expensive, avoid where possible
	for (unsigned iPseudoLayer = 0; iPseudoLayer <= m_maxSearchLayer; ++iPseudoLayer) {
	  // save the hash key since we may use it a few times
	  auto hash_key = std::make_pair(pTrack,iPseudoLayer);
	  // see if we have a cached search, otherwise do the search and cache	  
	  auto cached_result = tracks_to_hits.equal_range(hash_key);
	  if( cached_result.first != tracks_to_hits.end() ) {
	    for( auto iter = cached_result.first; iter != cached_result.second; ++iter ) {
	      // build a list of nearby clusters
	      nearby_clusters.insert(hits_to_clusters.find(iter->second)->second);	      
	    } // loop over cached associated hits
	  } else { // do search and cache result
	    KDTreeTesseract searchRegionHits = 
	      build_4d_kd_search_region(trackPosition,
					m_parallelDistanceCut,
					m_parallelDistanceCut,
					m_parallelDistanceCut,
					iPseudoLayer);
	    std::vector<HitKDNode> found_hits;
	    hits_kdtree.search(searchRegionHits,found_hits);
	    for( auto& hit : found_hits ) {
	      auto assc_cluster = hits_to_clusters.find(hit.data);
	      if( assc_cluster != hits_to_clusters.end() ) {
		// cache all hits that are nearby the track
		tracks_to_hits.emplace(hash_key,hit.data);
		// add to the list of nearby clusters
		nearby_clusters.insert(assc_cluster->second);		
	      } // hit is in a cluster?
	    } // kd-tree found hits	
	  }// if/else for cache
	} // loop over allowed pseudolayers

        // Identify the closest cluster and also the closest cluster below a specified hadronic energy threshold
        for (ClusterList::const_iterator clusterIter = nearby_clusters.begin(), clusterIterEnd = nearby_clusters.end();
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            Cluster *pCluster = *clusterIter;

            if (0 == pCluster->GetNCaloHits())
                continue;

            float trackClusterDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut,
                m_minTrackClusterCosAngle, trackClusterDistance))
            {
                continue;
            }

            const float energyDifference(std::fabs(pCluster->GetHadronicEnergy() - pTrack->GetEnergyAtDca()));

            if (pCluster->GetHadronicEnergy() > m_lowEnergyCut)
            {
                if ((trackClusterDistance < minDistance) || ((trackClusterDistance == minDistance) && (energyDifference < minEnergyDifference)))
                {
                    minDistance = trackClusterDistance;
                    pBestCluster = pCluster;
                    minEnergyDifference = energyDifference;
                }
            }
            else
            {
                if ((trackClusterDistance < minLowEnergyDistance) || ((trackClusterDistance == minLowEnergyDistance) && (energyDifference < minLowEnergyDifference)))
                {
                    minLowEnergyDistance = trackClusterDistance;
                    pBestLowEnergyCluster = pCluster;
                    minLowEnergyDifference = energyDifference;
                }
            }
        }

        // Apply a final track-cluster association distance cut
        Cluster *pMatchedCluster = nullptr;

        if (nullptr != pBestCluster)
        {
            pMatchedCluster = pBestCluster;
        }
        else if (nullptr != pBestLowEnergyCluster)
        {
            pMatchedCluster = pBestLowEnergyCluster;
        }

        // Now make the association
        if (nullptr != pMatchedCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pMatchedCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackClusterAssociationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCut", m_lowEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackClusterDistance", m_maxTrackClusterDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxSearchLayer", m_maxSearchLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ParallelDistanceCut", m_parallelDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackClusterCosAngle", m_minTrackClusterCosAngle));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
