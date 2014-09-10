/**
 *  @file   LCContent/src/LCReclustering/ReclusteringParentAlgorithm.cc
 * 
 *  @brief  Implementation of the reclustering parent algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCReclustering/ReclusteringParentAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode ReclusteringParentAlgorithm::Run()
{
    // Run reclustering daughter algorithms
    for (StringVector::const_iterator iter = m_ReclusteringParentAlgorithms.begin(), iterEnd = m_ReclusteringParentAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusteringParentAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "reclusteringAlgorithms",
        m_ReclusteringParentAlgorithms));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
