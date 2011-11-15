using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.ToolbarManager
{
	public class ToolBarManager : IMessageFilter, IDisposable
	{
		// Added by mav
		ScrollableControl _dockStation;
		public ScrollableControl DockStation 
		{
			get { return _dockStation; }
			set { _dockStation = value; }
		}

		private Form _mainForm;
		public Form MainForm
		{
			get {  return _mainForm; }
			set { _mainForm = value; }    
		}

		ToolBarDockArea _left; 
		public ToolBarDockArea Left { get { return _left; } }
		ToolBarDockArea _right;
		public ToolBarDockArea Right { get { return _right; } }
		ToolBarDockArea _top;
		public ToolBarDockArea Top { get { return _top; } }
		ToolBarDockArea _bottom;
		public ToolBarDockArea Bottom { get { return _bottom; } }


		public ToolBarManager(ScrollableControl dockStation, Form mainForm)
		{
			DockStation = dockStation;
			MainForm = mainForm;
			_left = new ToolBarDockArea(this, DockStyle.Left);
			_right = new ToolBarDockArea(this, DockStyle.Right);
			_top = new ToolBarDockArea(this, DockStyle.Top);
			_bottom = new ToolBarDockArea(this, DockStyle.Bottom);
			Application.AddMessageFilter(this);
		}


		protected ToolBarDockArea GetClosestArea(Point ptScreen, ToolBarDockArea preferred)
		{
			if(preferred != null) 
			{
				Rectangle p = preferred.RectangleToScreen(preferred.ClientRectangle);
				p.Inflate(8,8);
				if(p.Contains(ptScreen)) return preferred;
			}

			Rectangle l = _left.RectangleToScreen(_left.ClientRectangle); 
			l.Inflate(8,8);
			Rectangle r = _right.RectangleToScreen(_right.ClientRectangle);
			r.Inflate(8,8);
			Rectangle t = _top.RectangleToScreen(_top.ClientRectangle);
			t.Inflate(8,8);
			Rectangle b = _bottom.RectangleToScreen(_bottom.ClientRectangle);
			b.Inflate(8,8);

			if(t.Contains(ptScreen)) return _top;
			if(b.Contains(ptScreen)) return _bottom;
			if(l.Contains(ptScreen)) return _left;
			if(r.Contains(ptScreen)) return _right;

			return null;
		}

		private ArrayList _holders = new ArrayList();

		// Added by mav
		public ToolBarDockHolder GetHolder(Control c)
		{		
			foreach(ToolBarDockHolder holder in _holders) 
				if(holder.Control == c)
					return holder;
			return null;
		}
		public ToolBarDockHolder GetHolder(string title)
		{		
			foreach(ToolBarDockHolder holder in _holders) 
				if(holder.ToolbarTitle == title)
					return holder;
			return null;
		}

		public ArrayList GetControls()
		{
			ArrayList list = new ArrayList();			
			foreach(ToolBarDockHolder holder in _holders) 
				list.Add(holder.Control);
			return list;
		}

		public bool ContainsControl(Control c)
		{
			return GetControls().Contains(c);
		}

		public void ShowControl(Control c, bool show) 
		{
			ToolBarDockHolder holder = GetHolder(c);
			if(holder != null) 
			{
				if(holder.Visible != show) 
				{
					if(IsDocked(holder))
					{
						holder.Visible = show;
					}
					else
					{
						holder.FloatForm.Visible = show;
					}
				}
			}
		}

		// Added by mav
		public ToolBarDockHolder AddControl(Control c, DockStyle site) 
		{
			return AddControl(c, site, null, DockStyle.Right);
		}

		public ToolBarDockHolder AddControl(Control c) 
		{
			return AddControl(c, DockStyle.Top, null, DockStyle.Right);
		}

		public ToolBarDockHolder AddControl(Control c, DockStyle site, Control refControl, DockStyle refSite) 
		{
			if(site == DockStyle.Fill) 
				site = DockStyle.Top;

			ToolBarDockHolder holder = new ToolBarDockHolder(this, c, site);

			if(refControl != null) 
			{
				ToolBarDockHolder refHolder = GetHolder(refControl);
				if(refHolder != null) 
				{
					Point p = refHolder.PreferredDockedLocation;
					switch (refSite)
					{
						case DockStyle.Left:
							p.X -= 1;
							break;
						case DockStyle.Right:
							p.X += refHolder.Width+1;
							break;
						case DockStyle.Bottom:
							p.Y += refHolder.Height+1;
							break;
						default:
							p.Y -= 1;
							break;
					}
					holder.PreferredDockedLocation = p;
				}
			}


			_holders.Add(holder);
			if(site != DockStyle.None) 
			{
				holder.DockStyle = site;
				holder.Parent = holder.PreferredDockedArea;
			} 
			else 
			{
				holder.Parent = holder.FloatForm;
				holder.Location = new Point(0,0);
				holder.DockStyle = DockStyle.None;
				holder.FloatForm.Size = holder.Size;
				holder.FloatForm.Visible = true;
			}

			holder.MouseUp += ToolBarMouseUp;
			holder.DoubleClick += ToolBarDoubleClick;
			holder.MouseMove += ToolBarMouseMove;
			holder.MouseDown += ToolBarMouseDown;

			return holder;
		}

		public void RemoveControl(Control c) 
		{
			ToolBarDockHolder holder = GetHolder(c);
			if (holder == null) 
				return;
			holder.MouseUp -= ToolBarMouseUp;
			holder.DoubleClick -= ToolBarDoubleClick;
			holder.MouseMove -= ToolBarMouseMove;
			holder.MouseDown -= ToolBarMouseDown;
				
			_holders.Remove(holder);
			holder.Parent = null;
			holder.FloatForm.Close();
		}

		ToolBarDockHolder _dragged;
		Point _ptStart;
		Point _ptOffset;

		private void ToolBarMouseDown(object sender, MouseEventArgs e)
		{
			ToolBarDockHolder holder = (ToolBarDockHolder)sender;

			if (_dragged != null || !e.Button.Equals(MouseButtons.Left) || e.Clicks != 1 ||
				!holder.CanDrag(new Point(e.X, e.Y))) return;
			_ptStart = Control.MousePosition;
			_dragged = (ToolBarDockHolder)sender;
			_ptOffset = new Point(e.X, e.Y);
		}

		private bool IsDocked(Control holder)
		{
			return holder.Parent == Top
				   || holder.Parent == Left
				   || holder.Parent == Right
				   || holder.Parent == Bottom;
		}

		private ToolBarDockArea GetDockedArea(ToolBarDockHolder holder)
		{
			if(holder.Parent == Top) return Top;
			if(holder.Parent == Left) return Left;
			if(holder.Parent == Right) return Right;
			return holder.Parent == Bottom ? Bottom : null;
		}

		private void ToolBarMouseMove(object sender, MouseEventArgs e)
		{
			if (_dragged == null) 
				return;
			Point ptDelta = new Point(_ptStart.X - Control.MousePosition.X, _ptStart.Y - Control.MousePosition.Y);

			Point newLoc = _dragged.PointToScreen(new Point(0,0));
			newLoc = new Point(newLoc.X - ptDelta.X, newLoc.Y - ptDelta.Y);
			ToolBarDockArea closest = GetClosestArea(Control.MousePosition, _dragged.PreferredDockedArea);
			// Added by mav
			if(closest != null && !_dragged.IsAllowed(closest.Dock))
				closest = null;

			ToolBarDockArea docked = GetDockedArea(_dragged);

			if(_ctrlDown)
				closest = null;

			if(docked != null)
			{
				if(closest == null) 
				{
					docked.SuspendLayout();
					_dragged.Parent = _dragged.FloatForm;
					_dragged.Location = new Point(0,0);
					_dragged.DockStyle = DockStyle.None;
					_dragged.FloatForm.Visible = true;
					_dragged.FloatForm.Location = new Point(Control.MousePosition.X-_ptOffset.X, Control.MousePosition.Y-8);
					_dragged.FloatForm.Size = _dragged.Size;
					docked.ResumeLayout();
					docked.PerformLayout();
				} 
				else if(closest != docked) 
				{
					closest.SuspendLayout();
					newLoc = closest.PointToClient(Control.MousePosition);
					_dragged.DockStyle = closest.Dock;
					_dragged.Parent = closest;
					_dragged.PreferredDockedLocation = newLoc;
					_dragged.FloatForm.Visible = false;
					_dragged.PreferredDockedArea = closest;
					closest.ResumeLayout();
					closest.PerformLayout();
				} 
				else 
				{
					closest.SuspendLayout();
					newLoc = closest.PointToClient(Control.MousePosition);
//						if(closest.Horizontal)
//							newLoc = new Point(newLoc.X - 4, newLoc.Y - _dragged.Height/2);
//						else
//							newLoc = new Point(newLoc.X - _dragged.Width/2, newLoc.Y - 4);
					_dragged.PreferredDockedLocation = newLoc;
					closest.ResumeLayout();
					closest.PerformLayout();
				}
			}
			else
			{
				if(closest == null) 
				{
					_dragged.FloatForm.Location = newLoc;
				}
				else
				{
					closest.SuspendLayout();
					newLoc = closest.PointToClient(Control.MousePosition);
					_dragged.DockStyle = closest.Dock;
					_dragged.Parent = closest;
					_dragged.PreferredDockedLocation = newLoc;
					_dragged.FloatForm.Visible = false;
					_dragged.PreferredDockedArea = closest;
					closest.ResumeLayout();
					closest.PerformLayout();
				}
			}
			_ptStart = Control.MousePosition;
		}

		private void ToolBarMouseUp(object sender, MouseEventArgs e)
		{
			if (_dragged == null) return;
			_dragged = null;
			_ptOffset.X = 8;
			_ptOffset.Y = 8;
		}

		private void ToolBarDoubleClick(object sender, EventArgs e)
		{
			ToolBarDockHolder holder = (ToolBarDockHolder)sender;
			if(IsDocked(holder))
			{
				ToolBarDockArea docked = GetDockedArea(holder);
				docked.SuspendLayout();
				holder.Parent = holder.FloatForm;
				holder.Location = new Point(0,0);
				holder.DockStyle = DockStyle.None;
				holder.FloatForm.Visible = true;
				holder.FloatForm.Size = holder.Size;
				docked.ResumeLayout();
				docked.PerformLayout();
			}
			else
			{
				ToolBarDockArea area = holder.PreferredDockedArea;
				area.SuspendLayout();
				Point newLoc = holder.PreferredDockedLocation;
				holder.DockStyle = area.Dock;
				holder.Parent = area;
				holder.PreferredDockedLocation = newLoc;
				holder.FloatForm.Visible = false;
				holder.PreferredDockedArea = area;
				area.ResumeLayout();				
				area.PerformLayout();
			}
		}


		const int WM_KEYDOWN = 0x100;
		const int WM_KEYUP = 0x101; 
		bool _ctrlDown;

		public bool PreFilterMessage(ref Message m)
		{
			switch (m.Msg)
			{
				case WM_KEYDOWN:
					{
						Keys keyCode = (Keys)(int)m.WParam & Keys.KeyCode;
						if(keyCode == Keys.ControlKey) 
						{
							if(!_ctrlDown && _dragged!=null && IsDocked(_dragged)) 
							{
								ToolBarDockArea docked = GetDockedArea(_dragged);
								docked.SuspendLayout();
								_dragged.Parent = _dragged.FloatForm;
								_dragged.Location = new Point(0,0);
								_dragged.DockStyle = DockStyle.None;
								_dragged.FloatForm.Visible = true;
								_dragged.FloatForm.Location = new Point(Control.MousePosition.X-_ptOffset.X, Control.MousePosition.Y-8);
								_dragged.FloatForm.Size = _dragged.Size;
								docked.ResumeLayout();
								docked.PerformLayout();
							}
							_ctrlDown = true;
						}
					}
					break;
				case WM_KEYUP:
					{
						Keys keyCode = (Keys)(int)m.WParam & Keys.KeyCode;
						if(keyCode == Keys.ControlKey) 
						{
							if(_ctrlDown && _dragged!=null && !IsDocked(_dragged)) 
							{
								ToolBarDockArea closest = GetClosestArea(Control.MousePosition, _dragged.PreferredDockedArea);
								if(closest != null)  
								{
									closest.SuspendLayout();
									Point newLoc = closest.PointToClient(Control.MousePosition);
									_dragged.DockStyle = closest.Dock;
									_dragged.Parent = closest;
									_dragged.PreferredDockedLocation = newLoc;
									_dragged.FloatForm.Visible = false;
									_dragged.PreferredDockedArea = closest;
									closest.ResumeLayout();
									closest.PerformLayout();
								}
							}
							_ctrlDown = false;
						}
					}
					break;
			}
			return false;
		}

		class MyMenuItem : MenuItem
		{
			public Control Control;
		}

		// Added by mav
		public virtual void ShowContextMenu(Point ptScreen) 
		{
			ContextMenu cm = new ContextMenu();
			ArrayList al = new ArrayList();
			al.AddRange(_holders);
			al.Sort(new HolderSorter());

			MyMenuItem [] items = new MyMenuItem[al.Count];
			for(int i=0; i<al.Count; i++) 
			{	
				ToolBarDockHolder holder = al[i] as ToolBarDockHolder;
				if (holder != null)
				{
					Control c = holder.Control;
					items[i] = new MyMenuItem();
					items[i].Checked = c.Visible;
					items[i].Text = holder.ToolbarTitle;
					items[i].Click += MenuClickEventHandler;
					items[i].Control = c;
				}
				cm.MenuItems.Add(items[i]);
			}
			cm.Show(DockStation, DockStation.PointToClient(ptScreen));
		}

		protected void MenuClickEventHandler(object sender, EventArgs e) 
		{
			MyMenuItem item = (MyMenuItem)sender;
			ShowControl(item.Control, !item.Control.Visible);
		}

		private class HolderSorter : IComparer
		{
			#region IComparer Member

			public int Compare(object x, object y)
			{
				ToolBarDockHolder h1 = x as ToolBarDockHolder;
				ToolBarDockHolder h2 = y as ToolBarDockHolder;

// ReSharper disable PossibleNullReferenceException
				return h1.ToolbarTitle.CompareTo(h2.ToolbarTitle);
// ReSharper restore PossibleNullReferenceException
			}

			#endregion
		}

		public void Dispose()
		{
			if (_left != null)
				_left.Dispose();
			if (_right != null)
				_right.Dispose();
			if (_top != null)
				_top.Dispose();
			if (_bottom != null)
				_bottom.Dispose();
		}
	}
}