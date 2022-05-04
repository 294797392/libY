#include <string>

#include "Modular/ModuleFactory.h"

using namespace libYpp::Modular;

ModuleFactory *ModuleFactory::CreateDefault()
{
	return new ModuleFactory();
}

ModuleFactory::ModuleFactory(/* args */)
{
}

ModuleFactory::~ModuleFactory()
{
}

