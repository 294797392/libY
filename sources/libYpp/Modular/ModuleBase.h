#include <string>
#include <iostream>

#include "libYpp.h"

using namespace std;

namespace libYpp
{
    namespace Modular
    {
        class ModuleBase
        {
        private:
            YString id;
            YString name;
            YString description;

        public:
            const YString &ID() { return this->id; }
            const YString &Name() { return this->name; }
            const YString &Description() { return this->description; }

        public:
            ModuleBase(/* args */);
            ~ModuleBase();

        public:
            virtual int Initialize() = 0;
            virtual void Release() = 0;
        };
    }
}

