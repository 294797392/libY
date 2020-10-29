using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.DataModels
{
    /// <summary>
    /// 菜单数据模型
    /// </summary>
    [JsonObject("Menu")]
    public class MenuDM : ModelBase
    {
        /// <summary>
        /// 菜单图标
        /// </summary>
        [JsonProperty("Icon")]
        public string Icon { get; set; }

        /// <summary>
        /// 菜单所执行的动作
        /// </summary>
        [JsonProperty("ActionEntry")]
        public string ActionEntry { get; set; }

        /// <summary>
        /// 子菜单
        /// </summary>
        [JsonProperty("SubMenus")]
        public List<MenuDM> SubMenus { get; set; }
    }
}
