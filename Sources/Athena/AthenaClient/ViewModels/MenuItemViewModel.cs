using AthenaBase.DataModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WPFToolkit.MVVM;

namespace AthenaClient.ViewModels
{
    /// <summary>
    /// 菜单项ViewModel
    /// </summary>
    public class MenuItemViewModel : HierarchicalViewModel<MenuItemViewModel>
    {
        public MenuItemViewModel()
        { }

        public MenuItemViewModel(MenuDM menu)
        {
            
        }
    }
}
