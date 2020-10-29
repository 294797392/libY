using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.Modules
{
    public interface IModuleInstance
    {
        int Initialize(IDictionary setting);

        int Release();

        /// <summary>
        /// 模块启动
        /// </summary>
        /// <returns></returns>
        int Start();

        /// <summary>
        /// 模块停止运行
        /// </summary>
        /// <returns></returns>
        int Stop();
    }
}