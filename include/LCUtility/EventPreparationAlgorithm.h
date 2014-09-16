/**
 *  @file   LCContent/include/LCUtility/EventPreparationAlgorithm.h
 * 
 *  @brief  Header file for the track selection algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_EVENT_PREPARATION_ALGORITHM_H
#define LC_EVENT_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  EventPreparationAlgorithm class
 */
class EventPreparationAlgorithm : public pandora::Algorithm
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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string     m_outputTrackListName;          ///< The output track list name
    std::string     m_outputCaloHitListName;        ///< The output calo hit list name
    std::string     m_outputMuonCaloHitListName;    ///< The output muon calo hit list name

    std::string     m_replacementTrackListName;     ///< The replacement track list name
    std::string     m_replacementCaloHitListName;   ///< The replacement calo hit list name
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventPreparationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_EVENT_PREPARATION_ALGORITHM_H
