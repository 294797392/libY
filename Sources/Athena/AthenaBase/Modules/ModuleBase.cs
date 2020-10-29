using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.IO;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.Modules
{
    public abstract class ModuleBase : IModuleInstance
    {
        public int Initialize()
        {
            throw new NotImplementedException();
        }

        public int Release()
        {
            throw new NotImplementedException();
        }

        public int Start()
        {
            throw new NotImplementedException();
        }

        public int Stop()
        {
            throw new NotImplementedException();
        }
    }
}
