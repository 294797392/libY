using AthenaBase.Configurations;
using AthenaBase.DataModels;
using AthenaClient.ViewModels;
using DotNEToolkit;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace AthenaClient
{
    /// <summary>
    /// 整个应用程序的上下文信息
    /// </summary>
    public class UserContext
    {
        #region 单例对象

        private static UserContext userContext = new UserContext();

        public static UserContext Context { get { return userContext; } }

        #endregion

        #region 属性

        /// <summary>
        /// 快捷菜单ViewModel
        /// </summary>
        public DockMenuViewModel DockMenuVM { get; private set; }

        ///// <summary>
        ///// 系统菜单列表
        ///// </summary>
        //public ObservableCollection<MenuItemViewModel> SystemMenus { get; private set; }

        #endregion

        #region 公开接口

        public void Initialize()
        {
            this.DockMenuVM = new DockMenuViewModel();
            this.InitializeSystemMenus();
        }

        public void Release()
        { }

        #endregion

        #region 实例方法

        private void InitializeSystemMenus()
        {
            //this.SystemMenus = new ObservableCollection<MenuItemViewModel>();

            SystemMenus systemMenus;
            if (!JSONHelper.TryParseFile<SystemMenus>(ConfigFiles.CONFIG_FILE_SYSTEM_MENUS, out systemMenus))
            {
                systemMenus = DefaultConfigurations.DefaultSystemMenus;
            }

            foreach (MenuDM menu in systemMenus.MenuList)
            {
                MenuItemViewModel mivm = new MenuItemViewModel(menu);
                this.LoadSystemMenus(mivm, menu.SubMenus);
                this.DockMenuVM.MenuList.Add(mivm);
            }
        }

        private void LoadSystemMenus(MenuItemViewModel partentVM, IEnumerable<MenuDM> subMenus)
        {
            foreach (MenuDM menu in subMenus)
            {
                MenuItemViewModel mivm = new MenuItemViewModel(menu);
                this.LoadSystemMenus(mivm, menu.SubMenus);
                partentVM.Children.Add(mivm);
            }
        }

        #endregion
    }
}