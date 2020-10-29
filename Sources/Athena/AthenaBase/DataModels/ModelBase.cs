using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AthenaBase.DataModels
{
    /// <summary>
    /// 所有数据模型的基类
    /// </summary>
    public class ModelBase
    {
        public string ID { get; set; }

        public string Name { get; set; }

        public string Description { get; set; }
    }
}
