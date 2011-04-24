// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Media;

using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Rendering;
using ICSharpCode.AvalonEdit;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
	public class BracketHighlightRenderer : IBackgroundRenderer
	{
        int? openBracketOffset;
        int? closeBracketOffset;
		Pen borderPen;
		Brush backgroundBrush;
		TextView textView;
		
		public static readonly Color DefaultBackground = Color.FromArgb(22, 0, 0, 255);
		public static readonly Color DefaultBorder = Color.FromArgb(52, 0, 0, 255);
		
		public const string BracketHighlight = "Bracket highlight";
		
		public void SetHighlight(int? openBracketOffset, int? closeBracketOffset)
		{
            if (this.openBracketOffset != openBracketOffset || this.closeBracketOffset != closeBracketOffset)
            {
                this.openBracketOffset = openBracketOffset;
                this.closeBracketOffset = closeBracketOffset;
				textView.InvalidateLayer(this.Layer);
			}
		}
		
		public BracketHighlightRenderer(TextView textView)
		{
			if (textView == null)
				throw new ArgumentNullException("textView");
			
			this.textView = textView;
			
			this.textView.BackgroundRenderers.Add(this);
		}

		void UpdateColors(Color background, Color foreground)
		{
			this.borderPen = new Pen(new SolidColorBrush(foreground), 1);
			this.borderPen.Freeze();

			this.backgroundBrush = new SolidColorBrush(background);
			this.backgroundBrush.Freeze();
		}
		
		public KnownLayer Layer {
			get {
				return KnownLayer.Selection;
			}
		}
		
		public void Draw(TextView textView, DrawingContext drawingContext)
		{
            if (this.openBracketOffset == null || this.closeBracketOffset == null)
				return;
			
			BackgroundGeometryBuilder builder = new BackgroundGeometryBuilder();
			
			builder.CornerRadius = 1;
			builder.AlignToMiddleOfPixels = true;

            builder.AddSegment(textView, new TextSegment() { StartOffset = openBracketOffset, Length = 1 });
			builder.CloseFigure(); // prevent connecting the two segments
			builder.AddSegment(textView, new TextSegment() { StartOffset = closeBracketOffset, Length = 1 });
			
			Geometry geometry = builder.CreateGeometry();
			if (geometry != null) {
				drawingContext.DrawGeometry(backgroundBrush, borderPen, geometry);
			}
		}
		
		public static void ApplyCustomizationsToRendering(BracketHighlightRenderer renderer, IEnumerable<CustomizedHighlightingColor> customizations)
		{
			renderer.UpdateColors(DefaultBackground, DefaultBorder);
			foreach (CustomizedHighlightingColor color in customizations) {
				if (color.Name == BracketHighlight) {
					renderer.UpdateColors(color.Background ?? Colors.Blue, color.Foreground ?? Colors.Blue);
				}
			}
		}
	}
}
