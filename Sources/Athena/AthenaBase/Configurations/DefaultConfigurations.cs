using AthenaBase.DataModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.Configurations
{
    /// <summary>
    /// 存储默认配置的类
    /// </summary>
    public static class DefaultConfigurations
    {
        public static readonly SystemMenus DefaultSystemMenus = new SystemMenus() 
        {
            MenuList = new List<MenuDM>()
        };
    }
}
