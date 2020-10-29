using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WPFToolkit.MVVM;

namespace AthenaClient.ViewModels
{
    /// <summary>
    /// 附加在电脑屏幕边缘的菜单
    /// </summary>
    public class DockMenuViewModel : ViewModelBase
    {
        public ObservableCollection<MenuItemViewModel> MenuList { get; private set; }

        public DockMenuViewModel()
        {
            this.MenuList = new ObservableCollection<MenuItemViewModel>();
        }
    }
}
