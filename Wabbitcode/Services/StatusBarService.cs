using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Windows;

namespace Revsoft.Wabbitcode.Services
{
	public interface IStatusBarService
	{
		/// <summary>
		/// Sets the caret position shown in the status bar.
		/// </summary>
		/// <param name="x">column number</param>
		/// <param name="y">line number</param>
		/// <param name="charOffset">character number</param>
		void SetCaretPosition(int x, int y, int charOffset);
		void SetInsertMode(bool insertMode);

		/// <summary>
		/// Sets the message shown in the left-most pane in the status bar.
		/// </summary>
		/// <param name="message">The message text.</param>
		/// <param name="highlighted">Whether to highlight the text</param>
		void SetMessage(string message, bool highlighted = false);

		/// <summary>
		/// Sets the progress bar to the specified progress, with the specified name
		/// </summary>
		/// <param name="name">Name of the task progressing</param>
		/// <param name="progress">Amount of progress to draw</param>
		/// <param name="status">Operation status determining the color of the bar to draw</param>
		void SetProgress(double progress, string name = "", OperationStatus status = OperationStatus.Normal);

		void ShowProgress();
		void HideProgress();
		
	}

	public sealed class WabbitcodeStatusBarService : IStatusBarService
	{
		readonly WabbitcodeStatusBar statusBar;

		IDockingService dockingService;

		public WabbitcodeStatusBarService(IDockingService dockingService, WabbitcodeStatusBar statusBar)
		{
			if (dockingService == null)
			{
				throw new ArgumentNullException("dockingService");
			}
			if (statusBar == null)
			{
				throw new ArgumentNullException("statusBar");
			}
			this.dockingService = dockingService;
			this.statusBar = statusBar;
			this.statusBar.DockingService = dockingService;
		}

		public bool Visible
		{
			get { return statusBar.Visibility == Visibility.Visible; }
			set { statusBar.Visibility = value ? Visibility.Visible : Visibility.Collapsed; }
		}

		public void SetCaretPosition(int x, int y, int charOffset)
		{
			statusBar.CursorStatusBarPanel.Content = String.Format("Ln {0} Col {1} Ch {2}",
					String.Format("{0,-10}", y), String.Format("{0,-5}", x), String.Format("{0,-5}", charOffset));
		}

		public void SetInsertMode(bool insertMode)
		{
			statusBar.ModeStatusBarPanel.Content = insertMode ? "INS" : "OVR";
		}

		public void SetMessage(string message, bool highlighted)
		{
			statusBar.SetMessage(message, highlighted);
		}

		public void SetProgress(double progress, string name, OperationStatus status)
		{
			statusBar.DisplayProgress(name, progress, status);
		}

		public void ShowProgress()
		{
			statusBar.DisplayProgress("", 0.0, OperationStatus.Normal);
		}

		public void HideProgress()
		{
			statusBar.HideProgress();
		}
	}
}
