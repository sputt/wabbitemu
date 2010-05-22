using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Classes
{
	public static class ExtensionMethods
	{
		public static Bitmap ResizeImage(this Bitmap img, int width, int height)
		{
			Bitmap resized = new Bitmap(width + 1, height + 1);
			Graphics g = Graphics.FromImage(resized);
			g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			g.DrawImage(img, 0, 0, width + 1, height + 1);
			g.Dispose();

			return resized;
		}

		public static bool TryGetKey(this IDictionary<ListFileKey, ListFileValue> lookup, ListFileValue value, out ListFileKey key)
		{
			key = null;
			foreach (var pair in lookup)
			{
				if (pair.Value.Equals(value))
					key = pair.Key;
			}
			return key != null;
		}
	}
}
