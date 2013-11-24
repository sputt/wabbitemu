using System;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Actions
{
    public class CheckForUpdateAction : AbstractUiAction
    {
        public override void Execute()
        {
            try
            {
                if (UpdateService.CheckForUpdate())
                {
                    var result = MessageBox.Show("New version available. Download now?",
                        "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
                    if (result == DialogResult.Yes)
                    {
                        UpdateService.StartUpdater();
                    }
                }
                else
                {
                    MessageBox.Show("No new updates");
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error updating", ex);
            }
        }
    }

    public class AboutDialogAction : AbstractUiAction
    {
        public override void Execute()
        {
            using (AboutBox box = new AboutBox())
            {
                box.ShowDialog();
            }
        }
    }
}
