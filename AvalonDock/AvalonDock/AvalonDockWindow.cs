using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace AvalonDock
{
    public class AvalonDockWindow : Window
    {
        static AvalonDockWindow()
        {
            ShowInTaskbarProperty.OverrideMetadata(typeof(AvalonDockWindow), new FrameworkPropertyMetadata(false));
        
        }

	internal AvalonDockWindow()
        {
#if NET4
            System.Windows.Media.TextOptions.SetTextFormattingMode(this, System.Windows.Media.TextFormattingMode.Display);
            UseLayoutRounding = true;
#endif
        }
    }
}
