using AthenaBase.DataModels;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.Configurations
{
    /// <summary>
    /// 系统菜单配置
    /// </summary>
    [JsonObject("SystemMenus")]
    public class SystemMenus
    {
        [JsonProperty("MenuList")]
        public List<MenuDM> MenuList { get; set; }
    }
}
