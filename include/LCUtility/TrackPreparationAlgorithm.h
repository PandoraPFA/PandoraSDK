/**
 *  @file   LCContent/include/LCUtility/TrackPreparationAlgorithm.h
 * 
 *  @brief  Header file for the track preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TRACK_PREPARATION_ALGORITHM_H
#define LC_TRACK_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  TrackPreparationAlgorithm class
 */
class TrackPreparationAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief Default constructor
     */
    TrackPreparationAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Create list of parent tracks to be used in construction of charged pfos
     * 
     *  @param  inputTrackList the input track list
     *  @param  pfoTrackList to receive the list of parent tracks to be used in construction of charged pfos
     */
    pandora::StatusCode CreatePfoTrackList(const pandora::TrackList &inputTrackList, pandora::TrackList &pfoTrackList) const;

    /**
     *  @brief  Whether a track, or any of its daughters or siblings has an associated cluster
     * 
     *  @param  pTrack address of the track
     *  @param  readSiblingInfo whether to read sibling track information (set to false to avoid multiple counting)
     *
     *  @return boolean
     */
    bool HasAssociatedClusters(const pandora::Track *const pTrack, const bool readSiblingInfo = true) const;

    pandora::StringVector   m_candidateListNames;           ///< The list of track list names to use
    std::string             m_mergedCandidateListName;      ///< The name under which to save the full, merged, list of candidate tracks

    bool                    m_shouldMakeAssociations;       ///< Whether to re-make track-cluster associations for candidate tracks
    pandora::StringVector   m_associationAlgorithms;        ///< The ordered list of track-cluster associations algorithm to run

    bool                    m_shouldMakePfoTrackList;       ///< Whether to make pfo track list, containing parent tracks of charged pfos
    std::string             m_pfoTrackListName;             ///< The name of the pfo track list, containing parent tracks of charged pfos
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackPreparationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_TRACK_PREPARATION_ALGORITHM_H
