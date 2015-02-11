/**
 *  @file   PandoraSDK/include/Managers/AlgorithmManager.h
 * 
 *  @brief  Header file for the algorithm manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_MANAGER_H
#define PANDORA_ALGORITHM_MANAGER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class Algorithm;
class AlgorithmTool;
class AlgorithmFactory;
class AlgorithmToolFactory;
class Pandora;
class TiXmlElement;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief AlgorithmManager class
 */
class AlgorithmManager
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    AlgorithmManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~AlgorithmManager();

    /**
     *  @brief  Get the list of algorithms to be run by pandora
     *
     *  @return address of the list pandora algorithms
     */
    const StringVector &GetPandoraAlgorithms() const;

private:
    /**
     *  @brief  Register an algorithm factory
     * 
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    StatusCode RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory);

    /**
     *  @brief  Register an algorithm tool factory
     * 
     *  @param  algorithmToolType the type of algorithm tool that the factory will create
     *  @param  pAlgorithmToolFactory the address of an algorithm tool factory instance
     */
    StatusCode RegisterAlgorithmToolFactory(const std::string &algorithmToolType, AlgorithmToolFactory *const pAlgorithmToolFactory);

    /**
     *  @brief  Initialize algorithms
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializeAlgorithms(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Create an algorithm, via one of the algorithm factories registered with pandora
     * 
     *  @param  pXmlElement address of the xml element describing the algorithm type and settings
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    StatusCode CreateAlgorithm(TiXmlElement *const pXmlElement, std::string &algorithmName);

    /**
     *  @brief  Create an algorithm tool, via one of the algorithm tool factories registered with pandora
     * 
     *  @param  pXmlElement address of the xml element describing the algorithm tool type and settings
     *  @param  pAlgorithmTool to receive the address of the algorithm tool instance
     */
    StatusCode CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool);

    /**
     *  @brief  Find the name of a specific algorithm instance, so that it can be re-used
     * 
     *  @param  pXmlElement address of the xml element describing the algorithm type and settings
     *  @param  algorithmName to receive the name of the algorithm instance
     *  @param  instanceLabel to receive the label referring to a specific algorithm instance
     */
    StatusCode FindSpecificAlgorithmInstance(TiXmlElement *const pXmlElement, std::string &algorithmName, std::string &instanceLabel) const;

    typedef std::map<const std::string, Algorithm *const> AlgorithmMap;
    typedef std::map<const std::string, AlgorithmFactory *const> AlgorithmFactoryMap;
    typedef std::map<const std::string, const std::string> SpecificAlgorithmInstanceMap;

    AlgorithmMap                    m_algorithmMap;                     ///< The algorithm map
    AlgorithmFactoryMap             m_algorithmFactoryMap;              ///< The algorithm factory map
    SpecificAlgorithmInstanceMap    m_specificAlgorithmInstanceMap;     ///< The specific algorithm instance map
    StringVector                    m_pandoraAlgorithms;                ///< The ordered list of names of top-level algorithms, to be run by pandora

    typedef std::vector<AlgorithmTool*> AlgorithmToolList;
    typedef std::map<const std::string, AlgorithmToolFactory *const> AlgorithmToolFactoryMap;

    AlgorithmToolList               m_algorithmToolList;                ///< The algorithm tool list
    AlgorithmToolFactoryMap         m_algorithmToolFactoryMap;          ///< The algorithm tool factory map

    const Pandora *const            m_pPandora;                         ///< The pandora instance that will run the algorithms

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const StringVector &AlgorithmManager::GetPandoraAlgorithms() const
{
    return m_pandoraAlgorithms;
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_MANAGER_H
