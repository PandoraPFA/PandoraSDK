/**
 *  @file   PandoraSDK/include/Persistency/EventReadingAlgorithm.h
 * 
 *  @brief  Header file for the event reading algorithm class.
 * 
 *  $Log: $
 */
#ifndef EVENT_READING_ALGORITHM_H
#define EVENT_READING_ALGORITHM_H 1

#include "Pandora/ExternallyConfiguredAlgorithm.h"

#include "Pandora/PandoraInputTypes.h"

#include "Persistency/PandoraIO.h"

namespace pandora {class FileReader;}

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EventReadingAlgorithm class
 */
class EventReadingAlgorithm : public pandora::ExternallyConfiguredAlgorithm
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
     *  @brief  Default constructor
     */
    EventReadingAlgorithm();

    /**
     *  @brief  Destructor
     */
    ~EventReadingAlgorithm();

    /**
     *  @brief  External event reading parameters class
     */
    class ExternalEventReadingParameters : public pandora::ExternalParameters
    {
    public:
        std::string             m_geometryFileName;             ///< Name of the file containing geometry information
        std::string             m_eventFileNameList;            ///< Colon-separated list of file names to be processed
        pandora::InputUInt      m_skipToEvent;                  ///< Index of first event to consider in input file
    };

protected:
    pandora::StatusCode Initialize();
    pandora::StatusCode Run();

    /**
     *  @brief  Proceed to process next event file named in the input list
     */
    void MoveToNextEventFile();

    /**
     *  @brief  Replace the current event file reader with a new reader for the specified file
     *
     *  @param  fileName the file name
     */
    pandora::StatusCode ReplaceEventFileReader(const std::string &fileName);

    /**
     *  @brief  Analyze a provided file name to extract the file type/extension
     *
     *  @param  fileName the file name
     *
     *  @return the file type
     */
    pandora::FileType GetFileType(const std::string &fileName) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string                 m_geometryFileName;             ///< Name of the file containing geometry information
    std::string                 m_eventFileName;                ///< Name of the current file containing event information
    pandora::StringVector       m_eventFileNameVector;          ///< Vector of file names to be processed

    unsigned int                m_skipToEvent;                  ///< Index of first event to consider in first input file

    pandora::FileReader        *m_pEventFileReader;             ///< Address of the event file reader
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventReadingAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventReadingAlgorithm();
}

#endif // #ifndef EVENT_READING_ALGORITHM_H
