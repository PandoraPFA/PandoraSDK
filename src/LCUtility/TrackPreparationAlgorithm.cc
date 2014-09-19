/**
 *  @file   LCContent/src/LCUtility/TrackPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the track preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCUtility/TrackPreparationAlgorithm.h"

using namespace pandora;

namespace lc_content
{

TrackPreparationAlgorithm::TrackPreparationAlgorithm() :
    m_shouldMakeAssociations(true),
    m_shouldMakePfoTrackList(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::Run()
{
    // Create candidate track list, containing all tracks that could be associated to clusters and so used in final pfo creation
    TrackList candidateTrackList;

    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const TrackList *pTrackList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, *iter, pTrackList))
            continue;

        for (TrackList::const_iterator trackIter = pTrackList->begin(), trackIterEnd = pTrackList->end(); trackIter != trackIterEnd; ++trackIter)
        {
            if ((*trackIter)->IsAvailable())
                candidateTrackList.insert(*trackIter);
        }
    }

    // Set this list of candidate pfo tracks to be the current track list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, candidateTrackList, m_mergedCandidateListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Track>(*this, m_mergedCandidateListName));

    if (m_shouldMakeAssociations)
    {
        // Remove existing and calculate new track-cluster associations
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

        for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end();
            iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
        }
    }

    if (m_shouldMakePfoTrackList)
    {
        // Filter the candidate track list to identify the parent tracks of charged pfos
        TrackList pfoTrackList;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreatePfoTrackList(candidateTrackList, pfoTrackList));

        // Save the filtered list and set it to be the current list for future algorithms
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, pfoTrackList, m_pfoTrackListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Track>(*this, m_pfoTrackListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::CreatePfoTrackList(const TrackList &inputTrackList, TrackList &pfoTrackList) const
{
    TrackList siblingTracks;

    for (TrackList::const_iterator iter = inputTrackList.begin(), iterEnd = inputTrackList.end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        if (!pTrack->GetParentTrackList().empty())
            continue;

        // Sibling tracks as first evidence of pfo target
        const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        if (!siblingTrackList.empty())
        {
            if (siblingTracks.end() != siblingTracks.find(pTrack))
                continue;

            if (this->HasAssociatedClusters(pTrack))
            {
                pfoTrackList.insert(pTrack);
                siblingTracks.insert(siblingTrackList.begin(), siblingTrackList.end());
            }
        }
        // Single parent track as pfo target
        else if (this->HasAssociatedClusters(pTrack))
        {
            pfoTrackList.insert(pTrack);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool TrackPreparationAlgorithm::HasAssociatedClusters(const Track *const pTrack, const bool readSiblingInfo) const
{
    if ((pTrack->CanFormPfo() && pTrack->HasAssociatedCluster()) || (pTrack->CanFormClusterlessPfo()))
        return true;

    if (!pTrack->IsAvailable())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    // Consider any sibling tracks
    if (readSiblingInfo)
    {
        const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        for (TrackList::const_iterator iter = siblingTrackList.begin(), iterEnd = siblingTrackList.end(); iter != iterEnd; ++iter)
        {
            if (this->HasAssociatedClusters(*iter, false))
                return true;
        }
    }

    // Consider any daughter tracks
    const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

    for (TrackList::const_iterator iter = daughterTrackList.begin(), iterEnd = daughterTrackList.end(); iter != iterEnd; ++iter)
    {
        if (this->HasAssociatedClusters(*iter))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldMakeAssociations", m_shouldMakeAssociations));

    if (m_shouldMakeAssociations)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
            "trackClusterAssociationAlgorithms", m_associationAlgorithms));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldMakePfoTrackList", m_shouldMakePfoTrackList));

    if (m_shouldMakePfoTrackList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "PfoTrackListName", m_pfoTrackListName));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
