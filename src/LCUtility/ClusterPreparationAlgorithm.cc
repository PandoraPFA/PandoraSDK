/**
 *  @file   LCContent/src/LCUtility/ClusterPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the cluster preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCUtility/ClusterPreparationAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode ClusterPreparationAlgorithm::Run()
{
    // Merge all candidate clusters in specified input lists, to create e.g. final pfo cluster list
    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, *iter, pClusterList))
            continue;

        ClusterList clustersToSave;

        for (ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterIterEnd = pClusterList->end(); clusterIter != clusterIterEnd; ++clusterIter)
        {
            if ((*clusterIter)->IsAvailable())
                clustersToSave.insert(*clusterIter);
        }

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::SaveList(*this,
            *iter, m_mergedCandidateListName, clustersToSave));
    }

    // Save the merged list and set it to be the current list for future algorithms
    if (STATUS_CODE_SUCCESS != PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_mergedCandidateListName))
    {
        std::cout << "ClusterPreparationAlgorithm: empty cluster list for subsequent pfo construction." << std::endl;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DropCurrentList<Cluster>(*this));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
