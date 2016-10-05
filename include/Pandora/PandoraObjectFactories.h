/**
 *  @file   PandoraSDK/include/Pandora/PandoraObjectFactories.h
 * 
 *  @brief  Header file for the pandora object factories classes.
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_FACTORIES_H
#define PANDORA_OBJECT_FACTORIES_H 1

#include "Pandora/ObjectFactory.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

/**
 *  @brief  PandoraObjectFactory class
 */
template <typename PARAMETERS, typename OBJECT>
class PandoraObjectFactory : public ObjectFactory<PARAMETERS, OBJECT>
{
public:
    typedef PARAMETERS Parameters;
    typedef OBJECT Object;

    Parameters *NewParameters() const;
    StatusCode Read(Parameters &parameters, FileReader &fileReader) const;
    StatusCode Write(const Object *const pObject, FileWriter &fileWriter) const;

private:
    StatusCode Create(const Parameters &parameters, const Object *&pObject) const;
};

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_FACTORY_H
