using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.Actions
{
    /// <summary>
    /// 表示一个抽象的操作
    /// 比如点击菜单后执行的操作
    /// </summary>
    public abstract class AbstractAction
    {
        /// <summary>
        /// 触发该操作的对象
        /// </summary>
        public object Source { get; set; }

        /// <summary>
        /// 执行该操作
        /// </summary>
        /// <returns>执行结果</returns>
        public abstract int Execute();
    }
}
