using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace System.Windows.Forms
{
    public class MessageBoxExtended
    {
        public enum DialogResultExtended
        {
            None,
            YesNoYesAll,
            YesNoNoAll,
            YesNoYesAllNoAll
        }
        public static DialogResultExtended Show(string text, string caption, MessageBoxButtons buttons)
        {
            return Show(null, text, caption, buttons, MessageBoxIcon.None, MessageBoxDefaultButton.Button1, MessageBoxOptions.ServiceNotification, "", HelpNavigator.Topic, null);
        }

        //
        // Summary:
        //     Displays a message box with the specified text, caption, buttons, icon, default
        //     button, options, and Help button, using the specified Help file, HelpNavigator,
        //     and Help topic.
        //
        // Parameters:
        //   owner:
        //     An implementation of System.Windows.Forms.IWin32Window that will own the
        //     modal dialog box.
        //
        //   text:
        //     The text to display in the message box.
        //
        //   caption:
        //     The text to display in the title bar of the message box.
        //
        //   buttons:
        //     One of the System.Windows.Forms.MessageBoxButtons values that specifies which
        //     buttons to display in the message box.
        //
        //   icon:
        //     One of the System.Windows.Forms.MessageBoxIcon values that specifies which
        //     icon to display in the message box.
        //
        //   defaultButton:
        //     One of the System.Windows.Forms.MessageBoxDefaultButton values that specifies
        //     the default button for the message box.
        //
        //   options:
        //     One of the System.Windows.Forms.MessageBoxOptions values that specifies which
        //     display and association options will be used for the message box. You may
        //     pass in 0 if you wish to use the defaults.
        //
        //   helpFilePath:
        //     The path and name of the Help file to display when the user clicks the Help
        //     button.
        //
        //   navigator:
        //     One of the System.Windows.Forms.HelpNavigator values.
        //
        //   param:
        //     The numeric ID of the Help topic to display when the user clicks the Help
        //     button.
        //
        // Returns:
        //     One of the System.Windows.Forms.DialogResult values.
        //
        // Exceptions:
        //   System.ComponentModel.InvalidEnumArgumentException:
        //     buttons is not a member of System.Windows.Forms.MessageBoxButtons.  -or-
        //     icon is not a member of System.Windows.Forms.MessageBoxIcon.  -or- The defaultButton
        //     specified is not a member of System.Windows.Forms.MessageBoxDefaultButton.
        //
        //   System.InvalidOperationException:
        //     An attempt was made to display the System.Windows.Forms.MessageBox in a process
        //     that is not running in User Interactive mode. This is specified by the System.Windows.Forms.SystemInformation.UserInteractive
        //     property.
        //
        //   System.ArgumentException:
        //     options specified both System.Windows.Forms.MessageBoxOptions.DefaultDesktopOnly
        //     and System.Windows.Forms.MessageBoxOptions.ServiceNotification.  -or- buttons
        //     specified an invalid combination of System.Windows.Forms.MessageBoxButtons.
        public static DialogResultExtended Show(IWin32Window owner, string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultButton, MessageBoxOptions options, string helpFilePath, HelpNavigator navigator, object param)
        {
            return DialogResultExtended.None;
        }
    }
}
