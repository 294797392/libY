#ifndef __MODULEFACTORY_H__
#define __MODULEFACTORY_H__


namespace libYpp
{
	namespace Modular
	{
		class ModuleFactory
		{
		public:
			static ModuleFactory *CreateDefault();

		private:
			/* data */
		public:
			ModuleFactory(/* args */);
			~ModuleFactory();
		};
	}
}


#endif