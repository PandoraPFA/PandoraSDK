/**
 *  @file   PandoraSDK/include/Pandora/ObjectParameters.h
 * 
 *  @brief  Header file for pandora object parameters base class
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_PARAMETERS_H
#define PANDORA_OBJECT_PARAMETERS_H 1

namespace pandora
{

/**
 *  @brief  ObjectParameters class
 */
class ObjectParameters
{
public:
    /**
     *  @param  virtual destructor to allow polymorphic behaviour of api parameters classes
     */
    virtual ~ObjectParameters();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline ObjectParameters::~ObjectParameters()
{
}

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_PARAMETERS_H
