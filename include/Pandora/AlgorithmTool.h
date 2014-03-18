/**
 *  @file   PandoraPFANew/Framework/include/Pandora/AlgorithmTool.h
 * 
 *  @brief  Header file for the algorithm tool class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_TOOL_H
#define PANDORA_ALGORITHM_TOOL_H 1

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  AlgorithmTool class. Algorithm tools will tend to be tailored for specific parent algorithms. Unlike standard pandora
 *          daughter algorithms, algorithm tools can be accessed directly via parent algorithms and there is no change in the 
 *          pandora list-management when a parent algorithm runs its algorithm tools.
 */
class AlgorithmTool
{
public:
    /**
     *  @brief  Default constructor
     */
    AlgorithmTool();

    /**
     *  @brief  Get the algorithm tool type
     * 
     *  @return The algorithm tool type name
     */
    std::string GetAlgorithmToolType() const;

protected:
    /**
     *  @brief  Read the algorithm settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    virtual StatusCode ReadSettings(const TiXmlHandle xmlHandle) = 0;

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual StatusCode Initialize();

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmTool();

    /**
     *  @brief  Register the parent algorithm that will run the tool and the tool type
     * 
     *  @param  pParentAlgorithm address of the parent algorithm that will run the tool
     *  @param  algorithmToolType the algorithm tool type
     */
    StatusCode RegisterDetails(const std::string &algorithmToolType);

    std::string         m_algorithmToolType;    ///< The type of algorithm tool

    friend class AlgorithmManager;
};

typedef std::vector<AlgorithmTool*> AlgorithmToolList;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Factory class for instantiating algorithm tools
 */
class AlgorithmToolFactory
{
public:
    /**
     *  @brief  Create an instance of an algorithm
     * 
     *  @return the address of the algorithm instance
     */
    virtual AlgorithmTool *CreateAlgorithmTool() const = 0;

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmToolFactory();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmToolFactory::~AlgorithmToolFactory()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmTool::AlgorithmTool()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string AlgorithmTool::GetAlgorithmToolType() const
{
    return m_algorithmToolType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmTool::~AlgorithmTool()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode AlgorithmTool::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode AlgorithmTool::RegisterDetails(const std::string &algorithmToolType)
{
    if (algorithmToolType.empty())
        return STATUS_CODE_FAILURE;

    m_algorithmToolType = algorithmToolType;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_TOOL_H
