using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaClient
{
    /// <summary>
    /// 存储所有配置文件的完整路径
    /// </summary>
    public static class ConfigFiles
    {
        private static readonly string ConfigDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Configurations");

        public static readonly string CONFIG_FILE_SYSTEM_MENUS = Path.Combine(ConfigDir, "Configurations/SystemMenus.json");
    }
}
