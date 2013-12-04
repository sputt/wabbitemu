// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3064 $</version>
// </file>

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.TextEditor.Util;

namespace Revsoft.TextEditor.Gui.InsightWindow
{
	public sealed class InsightWindow : AbstractCompletionWindow
	{
	    private readonly ToolTip _toolTip = new ToolTip();
	    private bool _toolTipVisible;

		public InsightWindow(Form parentForm, TextEditorControl control) : base(parentForm, control)
		{
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            Font = new Font(FontFamily.GenericSansSerif, 10);
		}

	    private new void Close()
	    {
            _toolTip.Hide(control);
            _toolTipVisible = false;
	    }
		
		public void ShowInsightWindow()
		{
            var description = GetDescription();

		    bool oldToolTipVisible = _toolTipVisible;
            SetLocation();
		    var point = control.PointToClient(new Point(Bounds.X, Bounds.Y));
            _toolTip.Show(description, control, point);
		    _toolTipVisible = true;

		    if (_insightDataProviderStack.Count <= 0 || oldToolTipVisible)
		    {
		        return;
		    }

		    control.Focus();

		    if (ParentForm != null)
		    {
		        ParentForm.LocationChanged += (args, e) => SetLocation();
		    }

		    control.ActiveTextAreaControl.VScrollBar.ValueChanged += (args, e) => SetLocation();
		    control.ActiveTextAreaControl.HScrollBar.ValueChanged += (args, e) => SetLocation();
		    control.ActiveTextAreaControl.TextArea.DoProcessDialogKey += ProcessTextAreaKey;
		    control.ActiveTextAreaControl.Caret.PositionChanged += CaretOffsetChanged;
		    control.ActiveTextAreaControl.TextArea.LostFocus += TextEditorLostFocus;
		    control.Resize += (args, e) => SetLocation();
		}

	    private string GetDescription()
	    {
	        string methodCountMessage = null;
	        if (DataProvider == null || DataProvider.InsightDataCount < 1)
	        {
	            Close();
	            return string.Empty;
	        }
	        
            if (DataProvider.InsightDataCount > 1)
	        {
	            methodCountMessage = control.GetRangeDescription(CurrentData + 1, DataProvider.InsightDataCount);
	        }
	        string description = DataProvider.GetInsightData(CurrentData);

	        if (!string.IsNullOrEmpty(methodCountMessage))
	        {
	            string[] array =
	            {
	                char.ConvertFromUtf32(0x25B2), methodCountMessage,
	                char.ConvertFromUtf32(0x25BC), description
	            };

	            description = string.Join(" ", array);
	        }
	        return description;
	    }

	    #region Event handling routines
		protected override bool ProcessTextAreaKey(Keys keyData)
		{
			if (!_toolTip.Active) {
				return false;
			}
			switch (keyData) {
				case Keys.Down:
					if (DataProvider != null && DataProvider.InsightDataCount > 0) {
						CurrentData = (CurrentData + 1) % DataProvider.InsightDataCount;
						ShowInsightWindow();
					}
					return true;
				case Keys.Up:
					if (DataProvider != null && DataProvider.InsightDataCount > 0) {
						CurrentData = (CurrentData + DataProvider.InsightDataCount - 1) % DataProvider.InsightDataCount;
                        ShowInsightWindow();
					}
					return true;
			}
			return base.ProcessTextAreaKey(keyData);
		}
		
		protected override void CaretOffsetChanged(object sender, EventArgs e)
		{
			// move the window under the caret (don't change the x position)
			TextLocation caretPos  = control.ActiveTextAreaControl.Caret.Position;

		    int xpos = control.ActiveTextAreaControl.TextArea.TextView.GetDrawingXPos(caretPos.Line, caretPos.Column);
			int ypos = (control.ActiveTextAreaControl.Document.GetVisibleLine(caretPos.Y) + 1) * control.ActiveTextAreaControl.TextArea.TextView.FontHeight
				- control.ActiveTextAreaControl.TextArea.VirtualTop.Y;
			int rulerHeight = control.TextEditorProperties.ShowHorizontalRuler ? control.ActiveTextAreaControl.TextArea.TextView.FontHeight : 0;

		    string description = GetDescription();
            _toolTip.Show(description, control, new Point(xpos, ypos + rulerHeight));
			
			while (DataProvider != null && DataProvider.CaretOffsetChanged()) {
				CloseCurrentDataProvider();
			}
		}
		
		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);
			control.ActiveTextAreaControl.TextArea.Focus();
			if (TipPainterTools.DrawingRectangle1.Contains(e.X, e.Y)) {
				CurrentData = (CurrentData + DataProvider.InsightDataCount - 1) % DataProvider.InsightDataCount;
				Refresh();
			}
			if (TipPainterTools.DrawingRectangle2.Contains(e.X, e.Y)) {
				CurrentData = (CurrentData + 1) % DataProvider.InsightDataCount;
				Refresh();
			}
		}
		
		#endregion

	    readonly MouseWheelHandler _mouseWheelHandler = new MouseWheelHandler();
		
		public void HandleMouseWheel(MouseEventArgs e)
		{
			if (DataProvider != null && DataProvider.InsightDataCount > 0) {
				int distance = _mouseWheelHandler.GetScrollAmount(e);
				if (control.TextEditorProperties.MouseWheelScrollDown)
					distance = -distance;
				if (distance > 0) {
					CurrentData = (CurrentData + 1) % DataProvider.InsightDataCount;
				} else if (distance < 0) {
					CurrentData = (CurrentData + DataProvider.InsightDataCount - 1) % DataProvider.InsightDataCount;
				}
				Refresh();
			}
		}
		
		#region Insight Window Drawing routines
		protected override void OnPaint(PaintEventArgs pe)
		{
			string methodCountMessage = null, description;
			if (DataProvider == null || DataProvider.InsightDataCount < 1) {
				description = "Unknown Method";
			} else {
				if (DataProvider.InsightDataCount > 1) {
					methodCountMessage = control.GetRangeDescription(CurrentData + 1, DataProvider.InsightDataCount);
				}
				description = DataProvider.GetInsightData(CurrentData);
			}
			
			drawingSize = TipPainterTools.GetDrawingSizeHelpTipFromCombinedDescription(this,
			                                                                           pe.Graphics,
			                                                                           Font,
			                                                                           methodCountMessage,
			                                                                           description);
			if (drawingSize != Size) {
				SetLocation();
			} else {
				TipPainterTools.DrawHelpTipFromCombinedDescription(this, pe.Graphics, Font, methodCountMessage, description);
			}
		}
		
		protected override void OnPaintBackground(PaintEventArgs pe)
		{
			pe.Graphics.FillRectangle(SystemBrushes.Info, pe.ClipRectangle);
		}
		#endregion
		
		#region InsightDataProvider handling

	    readonly Stack<InsightDataProviderStackElement> _insightDataProviderStack = new Stack<InsightDataProviderStackElement>();
		
		int CurrentData {
			get {
				return _insightDataProviderStack.Peek().CurrentData;
			}
			set {
				_insightDataProviderStack.Peek().CurrentData = value;
			}
		}
		
		IInsightDataProvider DataProvider {
			get {
				if (_insightDataProviderStack.Count == 0) {
					return null;
				}
				return _insightDataProviderStack.Peek().DataProvider;
			}
		}
		
		public void AddInsightDataProvider(IInsightDataProvider provider, string fileName)
		{
			provider.SetupDataProvider(fileName, control.ActiveTextAreaControl.TextArea);
			if (provider.InsightDataCount > 0) {
				_insightDataProviderStack.Push(new InsightDataProviderStackElement(provider));
			}
		}
		
		void CloseCurrentDataProvider()
		{
			_insightDataProviderStack.Pop();
			if (_insightDataProviderStack.Count == 0) {
				Close();
			} else {
				Refresh();
			}
		}
		
		class InsightDataProviderStackElement
		{
		    public int                  CurrentData;
			public readonly IInsightDataProvider DataProvider;
			
			public InsightDataProviderStackElement(IInsightDataProvider dataProvider)
			{
				CurrentData  = Math.Max(dataProvider.DefaultIndex, 0);
				DataProvider = dataProvider;
			}
		}
		#endregion
	}
}
