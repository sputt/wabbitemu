// *****************************************************************************
//
//  Copyright 2004, Coder's Lab
//  All rights reserved. The software and associated documentation
//  supplied hereunder are the proprietary information of Coder's Lab
//  and are supplied subject to licence terms.
//
//
//  You can use this control freely in your projects, but let me know if you
//  are using it so I can add you to a list of references.
//
//  Email: ludwig.stuyck@coders-lab.be
//  Home page: http://www.coders-lab.be
//
//  History
//        18/07/2004
//            - Control creation
//        24/07/2004
//            - Implemented rubberband selection; also combination keys work:
//              ctrl, shift, ctrl+shift
//        25/08/2004
//            - Rubberband selection temporary removed due to scrolling problems.
//            - Renamed TreeViewSelectionMode property to SelectionMode.
//            - Renamed SelectionModes enumeration to TreeViewSelectionMode.
//            - Added MultiSelectSameParent selection mode.
//            - Added keyboard functionality.
//            - Enhanced selection drawing.
//            - Added SelectionBackColor property.
//        02/09/2004
//            - When shift/ctrl was pressed, treeview scrolled to last selected
//              node. Fixed.
//            - Moved TreeViewSelectionMode outside the TreeView class.
//            - BeforeSelect was fired multiple times, AfterSelect was never
//              fired. Fixed.
//            - Collapsing/Expanding node changed selection. This does not happen
//              anymore, except if a node that has selected descendants is
//              collapsed; then all descendants are unselected and the collapsed
//              node becomes selected.
//            - If in the BeforeSelect event, e.Cancel is set to true, then node
//              will not be selected
//            - SHIFT selection sometimes didn�t behave correctly. Fixed.
//        04/09/2004
//            - SelectedNodes is no longer an array of tree nodes, but a
//              SelectedNodesCollection
//            - In the AfterSelect event, the SelectedNodes contained two tree
//              nodes; the old one and the new one. Fixed.
//        05/09/2004
//            - Added Home, End, PgUp and PgDwn keys functionality
//        08/10/2004
//            - SelectedNodeCollection renamed to NodeCollection
//            - Fixes by GKM
//
//        18/8/2005
//            - Added events BeforeDeselect and AfterDeselect
//        09/5/2007
//            - Added an InvokeRequired check to Flashnode()
//        16/5/2007
//            - Gave the document a consistant format
//            - Created a new event 'SelectionsChanged'
//
// *****************************************************************************

using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services.Project;
using Timer = System.Windows.Forms.Timer;

namespace Revsoft.Wabbitcode.Extensions
{
    /// <summary>
    /// Selection mode for the treeview.
    /// </summary>
    /// <remarks>
    /// The Selection mode determines how treeview nodes can be selected.
    /// </remarks>
    public enum TreeViewSelectionMode
    {
        /// <summary>
        /// Only one node can be selected at a time.
        /// </summary>
        SingleSelect,

        /// <summary>
        /// Multiple nodes can be selected at the same time without restriction.
        /// </summary>
        MultiSelect,

        /// <summary>
        /// Multiple nodes that belong to the same root branch can be selected at the same time.
        /// </summary>
        MultiSelectSameRootBranch,

        /// <summary>
        /// Multiple nodes that belong to the same level can be selected at the same time.
        /// </summary>
        MultiSelectSameLevel,

        /// <summary>
        /// Multiple nodes that belong to the same level and same root branch can be selected at the same time.
        /// </summary>
        MultiSelectSameLevelAndRootBranch,

        /// <summary>
        /// Only nodes that belong to the same direct parent can be selected at the same time.
        /// </summary>
        MultiSelectSameParent
    }

    /// <summary>
    /// Delegate used for tree node events.
    /// </summary>
    public delegate void TreeNodeEventHandler(TreeNode tn);

    /// <summary>
    /// Collection of selected nodes.
    /// </summary>
    public class NodesCollection : CollectionBase
    {
        /// <summary>
        /// Event fired the collection has been cleared.
        /// </summary>
        internal event EventHandler SelectedNodesCleared;

        /// <summary>
        /// Event fired when a tree node has been added to the collection.
        /// </summary>
        internal event TreeNodeEventHandler TreeNodeAdded;

        /// <summary>
        /// Event fired when a tree node has been inserted to the collection.
        /// </summary>
        internal event TreeNodeEventHandler TreeNodeInserted;

        /// <summary>
        /// Event fired when a tree node has been removed to the collection.
        /// </summary>
        internal event TreeNodeEventHandler TreeNodeRemoved;

        /// <summary>
        /// Gets tree node at specified index.
        /// </summary>
        public TreeNode this[int index]
        {
            get
            {
                return (TreeNode)List[index];
            }
        }

        /// <summary>
        /// Adds a tree node to the collection.
        /// </summary>
        /// <param name="treeNode">Tree node to add.</param>
        /// <returns>The position into which the new element was inserted.</returns>
        public int Add(TreeNode treeNode)
        {
            if (TreeNodeAdded != null)
            {
                TreeNodeAdded(treeNode);
            }

            return List.Add(treeNode);
        }

        /// <summary>
        /// Determines whether treenode belongs to the collection.
        /// </summary>
        /// <param name="treeNode">Tree node to check.</param>
        /// <returns>True if tree node belongs to the collection, false if not.</returns>
        public bool Contains(TreeNode treeNode)
        {
            return List.Contains(treeNode);
        }

        /// <summary>
        /// Gets index of tree node in the collection.
        /// </summary>
        /// <param name="treeNode">Tree node to get index of.</param>
        /// <returns>Index of tree node in the collection.</returns>
        public int IndexOf(TreeNode treeNode)
        {
            return List.IndexOf(treeNode);
        }

        /// <summary>
        /// Inserts a tree node at specified index.
        /// </summary>
        /// <param name="index">The position into which the new element has to be inserted.</param>
        /// <param name="treeNode">Tree node to insert.</param>
        public void Insert(int index, TreeNode treeNode)
        {
            if (TreeNodeInserted != null)
            {
                TreeNodeInserted(treeNode);
            }

            List.Add(treeNode);
        }

        /// <summary>
        /// Removed a tree node from the collection.
        /// </summary>
        /// <param name="treeNode">Tree node to remove.</param>
        public void Remove(TreeNode treeNode)
        {
            if (TreeNodeRemoved != null)
            {
                TreeNodeRemoved(treeNode);
            }

            List.Remove(treeNode);
        }

        /// <summary>
        /// Occurs when collection is being cleared.
        /// </summary>
        protected override void OnClear()
        {
            if (SelectedNodesCleared != null)
            {
                SelectedNodesCleared(this, EventArgs.Empty);
            }

            base.OnClear();
        }
    }

    /// <summary>
    /// The TreeView control is a regular treeview with multi-selection capability.
    /// </summary>
    [ToolboxItem(true)]
    public class TreeView : System.Windows.Forms.TreeView
    {
	    /// <summary>
        /// Max time between keypresses to allow when typing to find something on the tree. Measured in ticks (ms * 10k)
        /// </summary>
        private const int MAX_TIME_BETWEEN_KEYPRESS = 750 * 10000;
        private const int WM_ERASEBKGND = 0x0014;

	    /// <summary>
        /// Used to make sure that SelectedNode can only be used from within this class.
        /// </summary>
        private bool _blnInternalCall;

        /// <summary>
        /// Keeps track whether a node click has been handled by the mouse down event. This is almost always the
        /// case, except when a selected node has been clicked again. Then, it will not be handled in the mouse
        /// down event because we might want to drag the node and if that's the case, node should not go in edit
        /// mode.
        /// </summary>
        private bool _blnNodeProcessedOnMouseDown;

        /// <summary>
        /// Track whether the total SelectedNodes changed across multiple operations
        /// for SelectionsChanged event
        /// </summary>
        private bool _blnSelectionChanged;

        /// <summary>
        /// Remembers whether mouse click on a node was single or double click.
        /// </summary>
        private bool _blnWasDoubleClick;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private Container _components;

        // Node being dragged
        private TreeNode _dragNode;

        /// <summary>
        /// Hashtable that contains all selected nodes.
        /// </summary>
        private readonly Hashtable _htblSelectedNodes = new Hashtable();

        /// <summary>
        /// Hashtable to preserve Node's original colors (colors can be set on the TreeView, or individual nodes)
        /// (GKM)
        /// </summary>
        private readonly Hashtable _htblSelectedNodesOrigColors = new Hashtable();

        // ImageList for ghosting
        private ImageList _imageListDrag;
        private ImageList _imageListTreeView;

        /// <summary>
        /// Keeps track of the number of mouse clicks.
        /// </summary>
        private int _intMouseClicks;
        private long _lastKeyPress;
        private TreeNode _nodePointedTo;
        private Color _oldBackColor;
        private Color _oldForeColor;

        /// <summary>
        /// Backcolor for selected nodes.
        /// </summary>
        private Color _selectionBackColor = SystemColors.Highlight;

        /// <summary>
        /// Selection mode.
        /// </summary>
        private TreeViewSelectionMode _selectionMode = TreeViewSelectionMode.SingleSelect;

        // Timer for scrolling
        private readonly Timer timer = new Timer();

        /// <summary>
        /// Keeps track of the first selected node when selection has begun with the keyboard.
        /// </summary>
        private TreeNode _tnKeysStartNode;

        /// <summary>
        /// Keeps track of most recent selected node.
        /// </summary>
        private TreeNode _tnMostRecentSelectedNode;

        /// <summary>
        /// Keeps track of node that has to be pu in edit mode.
        /// </summary>
        private TreeNode _tnNodeToStartEditOn;

        /// <summary>
        /// Keeps track of the selection mirror point; this is the last selected node without SHIFT key pressed.
        /// It is used as the mirror node during SHIFT selection.
        /// </summary>
        private TreeNode _tnSelectionMirrorPoint;

        /// <summary>
        /// Holds node that needs to be flashed.
        /// </summary>
        private TreeNode _tnToFlash;
        private string _word = string.Empty;

        public event TreeViewEventHandler AfterDeselect;

        public event TreeViewEventHandler BeforeDeselect;

        public event EventHandler SelectionsChanged;

        /// <summary>
        /// This property is for internal use only. Use SelectedNodes instead.
        /// </summary>
        public new TreeNode SelectedNode
        {
            get
            {
	            if (!_blnInternalCall)
                {
                    if (SelectedNodes.Count > 0)
                    {
                        return SelectedNodes[0];
                    }

                    return null;
                }
	            return base.SelectedNode;
            }

	        private set
	        {
		        if (!_blnInternalCall)
                {
                    throw new NotSupportedException("Use SelectedNodes instead of SelectedNode.");
                }
		        base.SelectedNode = value;
	        }
        }

        /// <summary>
        /// Gets selected nodes.
        /// </summary>
        public NodesCollection SelectedNodes
        {
            get
            {
                // Create a SelectedNodesCollection to return, and add event handlers to catch actions on it
                NodesCollection selectedNodesCollection = new NodesCollection();
                foreach (TreeNode tn in _htblSelectedNodes.Values)
                {
                    selectedNodesCollection.Add(tn);
                }

                selectedNodesCollection.TreeNodeAdded += SelectedNodes_TreeNodeAdded;
                selectedNodesCollection.TreeNodeInserted += SelectedNodes_TreeNodeInserted;
                selectedNodesCollection.TreeNodeRemoved += SelectedNodes_TreeNodeRemoved;
                selectedNodesCollection.SelectedNodesCleared += SelectedNodes_SelectedNodesCleared;

                return selectedNodesCollection;
            }
        }

        /// <summary>
        /// Gets/sets backcolor for selected nodes.
        /// </summary>
        public Color SelectionBackColor
        {
	        private get
            {
                return _selectionBackColor;
            }

            set
            {
                _selectionBackColor = value;
            }
        }

        /// <summary>
        /// Gets/sets selection mode.
        /// </summary>
        public TreeViewSelectionMode SelectionMode
        {
            get
            {
                return _selectionMode;
            }

            set
            {
                _selectionMode = value;
            }
        }

        /// <summary>
        /// Gets level of specified node.
        /// </summary>
        /// <param name="node">Node.</param>
        /// <returns>Level of node.</returns>
        public int GetNodeLevel(TreeNode node)
        {
            int level = 0;
            while ((node = node.Parent) != null)
            {
                level++;
            }

            return level;
        }

        /// <summary>
        /// Gets root parent of specified node.
        /// </summary>
        /// <param name="child">Node.</param>
        /// <returns>Root parent of specified node.</returns>
        public TreeNode GetRootParent(TreeNode child)
        {
            TreeNode tnParent = child;

            while (tnParent.Parent != null)
            {
                tnParent = tnParent.Parent;
            }

            return tnParent;
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_components != null)
                {
                    _components.Dispose();
                }
            }

            base.Dispose(disposing);
        }

        /// <summary>
        /// Occurs after a node is collapsed.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnAfterCollapse(TreeViewEventArgs e)
        {
            _blnSelectionChanged = false;

            // All child nodes should be deselected
            bool blnChildSelected = false;
            foreach (TreeNode tn in e.Node.Nodes)
            {
                if (IsNodeSelected(tn))
                {
                    blnChildSelected = true;
                }

                UnselectNodesRecursively(tn, TreeViewAction.Collapse);
            }

            if (blnChildSelected)
            {
                SelectNode(e.Node, true, TreeViewAction.Collapse);
            }

            OnSelectionsChanged();

            base.OnAfterCollapse(e);
        }

        protected void OnAfterDeselect(TreeNode tn)
        {
            if (AfterDeselect != null)
            {
                AfterDeselect(this, new TreeViewEventArgs(tn));
            }
        }

        protected void OnBeforeDeselect(TreeNode tn)
        {
            if (BeforeDeselect != null)
            {
                BeforeDeselect(this, new TreeViewEventArgs(tn));
            }
        }

        /// <summary>
        /// Occurs before node goes into edit mode.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnBeforeLabelEdit(NodeLabelEditEventArgs e)
        {
            _blnSelectionChanged = false; // prepare for OnSelectionsChanged

            // Make sure that it's the only selected node
            SelectNode(e.Node, true, TreeViewAction.ByMouse);
            UnselectAllNodesExceptNode(e.Node, TreeViewAction.ByMouse);

            OnSelectionsChanged();

            base.OnBeforeLabelEdit(e);
        }

        /// <summary>
        /// occurs when a node is about to be selected.
        /// </summary>
        /// <param name="e">TreeViewCancelEventArgs.</param>
        protected override void OnBeforeSelect(TreeViewCancelEventArgs e)
        {
            // We don't want the base TreeView to handle the selection, because it can only handle single selection.
            // Instead, we'll handle the selection ourselves by keeping track of the selected nodes and drawing the
            // selection ourselves.
            e.Cancel = true;
        }

        protected override void OnDragDrop(DragEventArgs e)
        {
            // Unlock updates
            // DragHelper.ImageList_DragLeave(Handle);

            // Get drop node
            TreeNode dropNode = GetNodeAt(PointToClient(new Point(e.X, e.Y)));

            // If drop node isn't equal to drag node, add drag node as child of drop node
            if (_dragNode != dropNode)
            {
                // Remove drag node from parent
                /*if (dragNode.ParentFolder == null)
                    Nodes.Remove(dragNode);
                else
                    dragNode.ParentFolder.Nodes.Remove(dragNode);

                // Add drag node to drop node
                dropNode.Nodes.Add(dragNode);
                dropNode.ExpandAll();*/

                // Set drag node to null
                _dragNode = null;

                // Disable scroll timer
                timer.Enabled = false;
            }

            if (_nodePointedTo != null)
            {
                _nodePointedTo.BackColor = _oldBackColor;
                _nodePointedTo.ForeColor = _oldForeColor;
            }

            base.OnDragDrop(e);
        }

        protected override void OnDragEnter(DragEventArgs e)
        {
            timer.Interval = 200;
            timer.Tick += new EventHandler(timer_Tick);

            // Enable timer for scrolling dragged item
            timer.Enabled = true;

            base.OnDragEnter(e);
        }

        protected override void OnDragLeave(EventArgs e)
        {
            // DragHelper.ImageList_DragLeave(Handle);

            // Disable timer for scrolling dragged item
            timer.Enabled = false;
            if (_nodePointedTo != null)
            {
                _nodePointedTo.BackColor = _oldBackColor;
                _nodePointedTo.ForeColor = _oldForeColor;
            }

            base.OnDragLeave(e);
        }

        protected override void OnDragOver(DragEventArgs e)
        {
            // SetStyle(ControlStyles.UserPaint, true);
            // SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            // SetStyle(ControlStyles.DoubleBuffer, true);
            // SendMessage(Handle, WM_SETREDRAW, 0, IntPtr.Zero);
            // eventMask = SendMessage(Handle, EM_GETEVENTMASK, 0, IntPtr.Zero);
            // Compute drag position and move image

            // Get actual drop node
            TreeNode dropNode = GetNodeAt(PointToClient(new Point(e.X, e.Y)));

            // DragHelper.ImageList_DragShowNolock(false);
            if (_nodePointedTo != null)
            {
                _nodePointedTo.BackColor = _oldBackColor;
                _nodePointedTo.ForeColor = _oldForeColor;
            }

            _nodePointedTo = dropNode;
            e.Effect = SelectedNodes.Contains(_nodePointedTo)  ? DragDropEffects.None : DragDropEffects.All;
            if (_nodePointedTo == null)
            {
                return;
            }

            if (_nodePointedTo.Tag is ProjectFile)
            {
                _nodePointedTo = _nodePointedTo.Parent;
            }

            _oldBackColor = _nodePointedTo.BackColor;
            _oldForeColor = _nodePointedTo.ForeColor;
            _nodePointedTo.BackColor = SystemColors.Highlight;
            _nodePointedTo.ForeColor = SystemColors.HighlightText;

            // DragHelper.ImageList_DragShowNolock(true);
            // if mouse is on a new node select it
            /*if (tempDropNode != dropNode)
            {
                DragHelper.ImageList_DragShowNolock(false);
                blnInternalCall = true;
                SelectedNode = dropNode;
                blnInternalCall = false;
                DragHelper.ImageList_DragShowNolock(true);
                tempDropNode = dropNode;
            }*/

            // Point formP = PointToClient(new Point(e.X, e.Y));
            // DragHelper.ImageList_DragMove(formP.X - Left, formP.Y - Top);

            // Avoid that drop node is child of drag node
            TreeNode tmpNode = dropNode;
            while (tmpNode.Parent != null)
            {
                if (tmpNode.Parent == _dragNode)
                {
                    e.Effect = DragDropEffects.None;
                }

                tmpNode = tmpNode.Parent;
            }

            base.OnDragOver(e);
            Refresh();

            // SendMessage(Handle, EM_SETEVENTMASK, 0, eventMask);
            // SendMessage(Handle, WM_SETREDRAW, 1, IntPtr.Zero);
        }

        protected override void OnGiveFeedback(GiveFeedbackEventArgs e)
        {
            if (e.Effect == DragDropEffects.Move)
            {
                // Show pointer cursor while dragging
                e.UseDefaultCursors = false;
                Cursor = Cursors.Default;
            }
            else
            {
                e.UseDefaultCursors = true;
            }

            base.OnGiveFeedback(e);
        }

        /// <summary>
        /// Occurs when an item is being dragged.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnItemDrag(ItemDragEventArgs e)
        {
            // SendMessage(Handle, WM_SETREDRAW, 0, IntPtr.Zero);
            // eventMask = SendMessage(Handle, EM_GETEVENTMASK, 0, IntPtr.Zero);
            e = new ItemDragEventArgs(MouseButtons.Left, SelectedNodes);

            // Get drag node and select it
            _dragNode = ((NodesCollection)e.Item)[0];
            NodesCollection nodes = (NodesCollection)e.Item;

            _blnInternalCall = true;
            SelectedNode = _dragNode;
            _blnInternalCall = false;

            // components = new System.ComponentModel.Container();
            /*imageListTreeView = new ImageList(this.components);
            imageListDrag = new ImageList(this.components);

            // Reset image list used for drag image
            imageListDrag.Images.Clear();
            imageListDrag.ImageSize = new Size(dragNode.Bounds.Size.Width + Indent, dragNode.Bounds.Height * nodes.Count);

            // Create new bitmap
            // This bitmap will contain the tree node image to be dragged
            Bitmap bmp = new Bitmap(dragNode.Bounds.Width + Indent, dragNode.Bounds.Height * nodes.Count);

            // Get graphics from bitmap
            Graphics gfx = Graphics.FromImage(bmp);

            // Draw node icon into the bitmap
            gfx.DrawImage(bmp, 0, 0);
            float counter = 0;
            for (int i = 0; i < nodes.Count; i++ )
            {
                // Draw node label into bitmap
                gfx.DrawString(nodes[i].Text, Font, new SolidBrush(ForeColor), Indent, counter * nodes[i].Bounds.Height);
                counter++;
            }

            // Add bitmap to imagelist
            imageListDrag.Images.Add(bmp);*/

            // Get mouse position in client coordinates
            Point p = PointToClient(MousePosition);

            // Compute delta between mouse position and node bounds
            int dx = p.X + Indent - _dragNode.Bounds.Left;
            int dy = p.Y - _dragNode.Bounds.Top;

            base.OnItemDrag(e);

            // Begin dragging image
            // if (DragHelper.ImageList_BeginDrag(imageListDrag.Handle, 0, dx, dy))
            // {
            // Begin dragging
            DoDragDrop(SelectedNodes, DragDropEffects.Move); // DoDragDrop(bmp, DragDropEffects.Move);
            // End dragging image
            // DragHelper.ImageList_EndDrag();
            // }
            // SendMessage(Handle, EM_SETEVENTMASK, 0, eventMask);
            // SendMessage(Handle, WM_SETREDRAW, 1, IntPtr.Zero);
        }

        /// <summary>
        /// occurs when a key is down.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnKeyDown(KeyEventArgs e)
        {
            Keys kMod = Keys.None;
            switch (e.Modifiers)
            {
            case Keys.Shift:
            case Keys.Control:
            case Keys.Control | Keys.Shift:
                kMod = Keys.Shift;
                if (_tnKeysStartNode == null)
                {
                    _tnKeysStartNode = _tnMostRecentSelectedNode;
                }

                break;
            default:
                _tnKeysStartNode = null;
                break;
            }

            int intNumber = 0;

            TreeNode tnNewlySelectedNodeWithKeys = null;
            switch (e.KeyCode)
            {
            case Keys.Down:
                if (_tnMostRecentSelectedNode != null)
                {
                    tnNewlySelectedNodeWithKeys = _tnMostRecentSelectedNode.NextVisibleNode;
                }

                break;

            case Keys.Up:
                if (_tnMostRecentSelectedNode != null)
                {
                    tnNewlySelectedNodeWithKeys = _tnMostRecentSelectedNode.PrevVisibleNode;
                }

                break;

            case Keys.Left:
                if (_tnMostRecentSelectedNode != null && _tnMostRecentSelectedNode.IsExpanded)
                {
                    _tnMostRecentSelectedNode.Collapse();
                }
                else
                {
                    tnNewlySelectedNodeWithKeys = _tnMostRecentSelectedNode.Parent;
                }

                break;

            case Keys.Right:
                if (_tnMostRecentSelectedNode != null && !_tnMostRecentSelectedNode.IsExpanded)
                {
                    _tnMostRecentSelectedNode.Expand();
                }
                else if (_tnMostRecentSelectedNode.Nodes != null)
                {
                    tnNewlySelectedNodeWithKeys = _tnMostRecentSelectedNode.Nodes[0];
                }

                break;

            case Keys.Home:
                if (Nodes != null)
                {
                    tnNewlySelectedNodeWithKeys = Nodes[0];
                }

                break;

            case Keys.End:
                tnNewlySelectedNodeWithKeys = GetLastVisibleNode();
                break;

            case Keys.PageDown:

                intNumber = GetNumberOfVisibleNodes();
                tnNewlySelectedNodeWithKeys = GetNextTreeNode(_tnMostRecentSelectedNode, true, intNumber);
                break;

            case Keys.PageUp:

                intNumber = GetNumberOfVisibleNodes();
                tnNewlySelectedNodeWithKeys = GetNextTreeNode(_tnMostRecentSelectedNode, false, intNumber);
                break;

            default:
                base.OnKeyDown(e); // GKM
                return;
            }

            if (tnNewlySelectedNodeWithKeys != null)
            {
                SetFocusToNode(_tnMostRecentSelectedNode, false);
                ProcessNodeRange(_tnKeysStartNode, tnNewlySelectedNodeWithKeys, new MouseEventArgs(MouseButtons.Left, 1, Cursor.Position.X, Cursor.Position.Y, 0), kMod, TreeViewAction.ByKeyboard, false);
                _tnMostRecentSelectedNode = tnNewlySelectedNodeWithKeys;
                SetFocusToNode(_tnMostRecentSelectedNode, true);
            }

            // Ensure visibility
            if (_tnMostRecentSelectedNode != null)
            {
                TreeNode tnToMakeVisible = null;
                switch (e.KeyCode)
                {
                case Keys.Down:
                case Keys.Right:
                    tnToMakeVisible = GetNextTreeNode(_tnMostRecentSelectedNode, true, 5);
                    break;

                case Keys.Up:
                case Keys.Left:
                    tnToMakeVisible = GetNextTreeNode(_tnMostRecentSelectedNode, false, 5);
                    break;

                case Keys.Home:
                case Keys.End:
                    tnToMakeVisible = _tnMostRecentSelectedNode;
                    break;

                case Keys.PageDown:
                    tnToMakeVisible = GetNextTreeNode(_tnMostRecentSelectedNode, true, intNumber - 2);
                    break;

                case Keys.PageUp:
                    tnToMakeVisible = GetNextTreeNode(_tnMostRecentSelectedNode, false, intNumber - 2);
                    break;
                }

                if (tnToMakeVisible != null)
                {
                    tnToMakeVisible.EnsureVisible();
                }
            }

            base.OnKeyDown(e);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            long time = DateTime.Now.Ticks;
            if (time - _lastKeyPress > MAX_TIME_BETWEEN_KEYPRESS)
            {
                _word = string.Empty;
            }

            _lastKeyPress = time;
            _word += e.KeyChar;
            var node = SelectedNode;
            if (node == null)
            {
                node = Nodes[0];
            }

            var foundNode = FindVisibleNodeStartsWith(node, _word);
            if (foundNode != null)
            {
                SelectNode(node, false, TreeViewAction.ByKeyboard);
                SelectNode(foundNode, true, TreeViewAction.ByKeyboard);
                SetFocusToNode(foundNode, true);
                foundNode.EnsureVisible();
            }
            else
            {
                foundNode = FindVisibleNodeStartsWith(node, _word.Substring(_word.Length - 1));
                if (foundNode != null)
                {
                    SelectNode(node, false, TreeViewAction.ByKeyboard);
                    SelectNode(foundNode, true, TreeViewAction.ByKeyboard);
                    SetFocusToNode(foundNode, true);
                    foundNode.EnsureVisible();
                }
            }

            base.OnKeyPress(e);
        }

        /// <summary>
        /// Occurs when mouse is down.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnMouseDown(MouseEventArgs e)
        {
            _tnKeysStartNode = null;

            // Store number of mouse clicks in OnMouseDown event, because here we also get e.Clicks = 2 when an item was doubleclicked
            // in OnMouseUp we seem to get always e.Clicks = 1, also when item is doubleclicked
            _intMouseClicks = e.Clicks;

            TreeNode tn = GetNodeAt(e.X, e.Y);

            if (tn == null)
            {
                return;
            }

            // Preserve colors here, because if you do it later then it will already have selected colors
            // Don't know why...!
            PreserveNodeColors(tn);

            // If +/- was clicked, we should not process the node.
            if (!IsPlusMinusClicked(tn, e))
            {
                // If mouse down on a node that is already selected, then we should process this node in the mouse up event, because we
                // might want to drag it and it should not be put in edit mode.
                // Also, only process node if click was in node's bounds.
                if ((tn != null) && IsClickOnNode(tn, e) && (!IsNodeSelected(tn)))
                {
                    // Flash node. In case the node selection is cancelled by the user, this gives the effect that it
                    // was selected and unselected again.
                    _tnToFlash = tn;

                    _blnNodeProcessedOnMouseDown = true;

                    Debug.WriteLine("*** " + tn.BackColor);
                    ProcessNodeRange(_tnMostRecentSelectedNode, tn, e, ModifierKeys, TreeViewAction.ByMouse, true);
                }
            }

            base.OnMouseDown(e);
        }

        /// <summary>
        /// Occurs when mouse button is up after a click.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnMouseUp(MouseEventArgs e)
        {
#if DEBUG
            try
            {
#endif
                if (!_blnNodeProcessedOnMouseDown)
                {
                    TreeNode tn = GetNodeAt(e.X, e.Y);

                    // Mouse click has not been handled by the mouse down event, so do it here. This is the case when
                    // a selected node was clicked again; in that case we handle that click here because in case the
                    // user is dragging the node, we should not put it in edit mode.
                    if (IsClickOnNode(tn, e))
                    {
                        ProcessNodeRange(_tnMostRecentSelectedNode, tn, e, ModifierKeys, TreeViewAction.ByMouse, true);
                    }
                }

                _blnNodeProcessedOnMouseDown = false;

                base.OnMouseUp(e);
#if DEBUG
            }
            catch (Exception ex)
            {
                // GKM - Untrapped exceptions were killing me for debugging purposes.
                // It probably shouldn't be here permanently, but it was causing real trouble for me.
                MessageBox.Show(this, ex.ToString());
            }
#endif
        }

        protected void OnSelectionsChanged()
        {
            if (_blnSelectionChanged)
            {
                if (SelectionsChanged != null)
                {
                    SelectionsChanged(this, new EventArgs());
                }
            }
        }

        protected override void WndProc(ref Message msg)
        {
            if (msg.Msg == WM_ERASEBKGND)
            {
                return;
            }

            base.WndProc(ref msg);
        }

        /// <summary>
        /// Finds a visible node that starts with the specified text
        /// </summary>
        /// <param name="node"></param>
        /// <param name="text"></param>
        /// <returns></returns>
        private TreeNode FindVisibleNodeStartsWith(TreeNode startNode, string text)
        {
            var node = startNode;
            if (node == null || Nodes.Count < 1)
            {
                return null;
            }

            bool firstPass = true;
            if (text.Length == 1)
            {
                node = node.NextVisibleNode;
                firstPass = false;
            }

            while (!node.Text.ToLower().StartsWith(text) && (node != startNode || firstPass))
            {
                firstPass = false;
                node = node.NextVisibleNode;
                if (node == null)
                {
                    node = Nodes[0];
                }
            }

            if (node == startNode && firstPass == false)
            {
                return null;
            }

            return node;
        }

        /// <summary>
        /// Flashes node.
        /// </summary>
        private void FlashNode()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(FlashNode));
                return;
            }

            TreeNode tn = _tnToFlash;

            // Only flash node is it's not yet selected
            if (!IsNodeSelected(tn))
            {
                tn.BackColor = SelectionBackColor;
                tn.ForeColor = BackColor;
                Invalidate();
                Refresh();
                Application.DoEvents();
                Thread.Sleep(200);
            }

            // If node is not selected yet, restore default colors to end flashing
            if (!IsNodeSelected(tn))
            {
                tn.BackColor = BackColor;
                tn.ForeColor = ForeColor;
            }
        }

        /// <summary>
        /// Gets last visible node.
        /// </summary>
        /// <returns>Last visible node.</returns>
        private TreeNode GetLastVisibleNode()
        {
            TreeNode tnTemp = Nodes[0];

            while (tnTemp.NextVisibleNode != null)
            {
                tnTemp = tnTemp.NextVisibleNode;
            }

            return tnTemp;
        }

        /// <summary>
        /// Gets next tree node(s), starting from the specified node and direction.
        /// </summary>
        /// <param name="start">Node to start from.</param>
        /// <param name="down">True to go down, false to go up.</param>
        /// <param name="intNumber">Number of nodes to go down or up.</param>
        /// <returns>Next node.</returns>
        private TreeNode GetNextTreeNode(TreeNode start, bool down, int intNumber)
        {
            int intCounter = 0;
            TreeNode tnTemp = start;
            while (intCounter < intNumber)
            {
                if (down)
                {
                    if (tnTemp.NextVisibleNode != null)
                    {
                        tnTemp = tnTemp.NextVisibleNode;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (tnTemp.PrevVisibleNode != null)
                    {
                        tnTemp = tnTemp.PrevVisibleNode;
                    }
                    else
                    {
                        break;
                    }
                }

                intCounter++;
            }

            return tnTemp;
        }

        /// <summary>
        /// Gets number of visible nodes.
        /// </summary>
        /// <returns>Number of visible nodes.</returns>
        private int GetNumberOfVisibleNodes()
        {
            int intCounter = 0;

            TreeNode tnTemp = Nodes[0];

            while (tnTemp != null)
            {
                if (tnTemp.IsVisible)
                {
                    intCounter++;
                }

                tnTemp = tnTemp.NextVisibleNode;
            }

            return intCounter;
        }

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            _components = new Container();

            timer.Interval = 200;
            timer.Tick += new EventHandler(timer_Tick);
            _imageListTreeView = new ImageList(_components);
            _imageListDrag = new ImageList(_components);
        }

        /// <summary>
        /// Determines whether the specified node is a child (indirect or direct) of the specified parent.
        /// </summary>
        /// <param name="child">Node to check.</param>
        /// <param name="parent">ParentFolder node.</param>
        /// <returns>True if specified node is a direct or indirect child of parent node, false if not.</returns>
        private bool IsChildOf(TreeNode child, TreeNode parent)
        {
            bool blnChild = false;

            TreeNode tnTemp = child;
            while (tnTemp != null)
            {
                if (tnTemp == parent)
                {
                    blnChild = true;
                    break;
                }
                else
                {
                    tnTemp = tnTemp.Parent;
                }
            }

            return blnChild;
        }

        /// <summary>
        /// Determines whether a mouse click was inside the node bounds or outside the node bounds..
        /// </summary>
        /// <param name="tn">TreeNode to check.</param>
        /// <param name="e">MouseEventArgs.</param>
        /// <returns>True is mouse was clicked inside the node bounds, false if it was clicked ouside the node bounds.</returns>
        private bool IsClickOnNode(TreeNode tn, MouseEventArgs e)
        {
            if (tn == null)
            {
                return false;
            }

            // GKM
            // Determine the rightmost position we'll process clicks (so that the click has to be on the node's bounds,
            // like the .NET treeview
            int rightMostX = tn.Bounds.X + tn.Bounds.Width;
            return tn != null && e.X < rightMostX; // GKM
        }

        private bool IsIconClicked(TreeNode tn, MouseEventArgs e)
        {
            int intNodeLevel = GetNodeLevel(tn);
            bool blnIconClicked = false;
            if (e.X < 4 + (intNodeLevel * 4))
            {
                blnIconClicked = true;
            }

            return blnIconClicked;
        }

        /// <summary>
        /// Determines whether the specified node is selected or not.
        /// </summary>
        /// <param name="tn">Node to check.</param>
        /// <returns>True if specified node is selected, false if not.</returns>
        private bool IsNodeSelected(TreeNode tn)
        {
            if (tn != null)
            {
                return _htblSelectedNodes.ContainsKey(tn.GetHashCode());
            }

            return false;
        }

        private bool IsPlusMinusClicked(TreeNode tn, MouseEventArgs e)
        {
            int intNodeLevel = GetNodeLevel(tn);
            bool blnPlusMinusClicked = false;
            if (e.X < 20 + (intNodeLevel * 20))
            {
                blnPlusMinusClicked = true;
            }

            return blnPlusMinusClicked;
        }

        private bool IsTextClicked(TreeNode tn, MouseEventArgs e)
        {
            int intNodeLevel = GetNodeLevel(tn);
            bool blnIconClicked = false;
            if (e.X < (tn.Text.Length * 4 + ((intNodeLevel + 1) * 4)))
            {
                blnIconClicked = true;
            }

            return blnIconClicked;
        }

        private void PreserveNodeColors(TreeNode tn)
        {
            if (tn == null)
            {
                return;
            }

            Debug.WriteLine(tn.BackColor.ToString());

            if (_htblSelectedNodesOrigColors.ContainsKey(tn.GetHashCode()))
            {
                // Color[] color = (Color[])htblSelectedNodesOrigColors[tn.GetHashCode()];
                //				color[0]=tn.BackColor;
                //				color[1]=tn.ForeColor;
            }
            else
            {
                _htblSelectedNodesOrigColors.Add(tn.GetHashCode(), new Color[] { tn.BackColor, tn.ForeColor });
            }
        }

        /// <summary>
        /// Processes a node range.
        /// </summary>
        /// <param name="startNode">Start node of range.</param>
        /// <param name="endNode">End node of range.</param>
        /// <param name="e">MouseEventArgs.</param>
        /// <param name="keys">Keys.</param>
        /// <param name="tva">TreeViewAction.</param>
        /// <param name="allowStartEdit">True if node can go to edit mode, false if not.</param>
        private void ProcessNodeRange(TreeNode startNode, TreeNode endNode, MouseEventArgs e, Keys keys, TreeViewAction tva, bool allowStartEdit)
        {
            _blnSelectionChanged = false; // prepare for OnSelectionsChanged

            if (e.Button == MouseButtons.Left)
            {
                _blnWasDoubleClick = _intMouseClicks == 2;

                TreeNode tnTemp = null;
                int intNodeLevelStart;

                if (((keys & Keys.Control) == 0) && ((keys & Keys.Shift) == 0))
                {
                    // CTRL and SHIFT not held down
                    _tnSelectionMirrorPoint = endNode;
                    int intNumberOfSelectedNodes = SelectedNodes.Count;

                    // If it was a double click, select node and suspend further processing
                    if (_blnWasDoubleClick)
                    {
                        base.OnMouseDown(e);
                        return;
                    }

                    if (!IsPlusMinusClicked(endNode, e))
                    {
                        bool blnNodeWasSelected = false;
                        if (IsNodeSelected(endNode))
                        {
                            blnNodeWasSelected = true;
                        }

                        UnselectAllNodesExceptNode(endNode, tva);
                        SelectNode(endNode, true, tva);

                        if (blnNodeWasSelected && LabelEdit && allowStartEdit && !_blnWasDoubleClick &&
                            intNumberOfSelectedNodes <= 1 && !IsIconClicked(endNode, e))
                        {
                            // Node should be put in edit mode
                            _tnNodeToStartEditOn = endNode;
                            StartEdit();

                            // System.Threading.Thread t = new System.Threading.Thread(new System.Threading.ThreadStart(StartEdit));
                            // t.Start();
                        }
                    }
                }
                else if (((keys & Keys.Control) != 0) && ((keys & Keys.Shift) == 0))
                {
                    // CTRL held down
                    _tnSelectionMirrorPoint = null;

                    if (!IsNodeSelected(endNode))
                    {
                        switch (_selectionMode)
                        {
                        case TreeViewSelectionMode.SingleSelect:
                            UnselectAllNodesExceptNode(endNode, tva);
                            break;

                        case TreeViewSelectionMode.MultiSelectSameRootBranch:
                            TreeNode tnAbsoluteParent2 = GetRootParent(endNode);
                            UnselectAllNodesNotBelongingToParent(tnAbsoluteParent2, tva);
                            break;

                        case TreeViewSelectionMode.MultiSelectSameLevel:
                            UnselectAllNodesNotBelongingToLevel(GetNodeLevel(endNode), tva);
                            break;

                        case TreeViewSelectionMode.MultiSelectSameLevelAndRootBranch:
                            TreeNode tnAbsoluteParent = GetRootParent(endNode);
                            UnselectAllNodesNotBelongingToParent(tnAbsoluteParent, tva);
                            UnselectAllNodesNotBelongingToLevel(GetNodeLevel(endNode), tva);
                            break;

                        case TreeViewSelectionMode.MultiSelectSameParent:
                            TreeNode tnParent = endNode.Parent;
                            UnselectAllNodesNotBelongingDirectlyToParent(tnParent, tva);
                            break;
                        }

                        SelectNode(endNode, true, tva);
                    }
                    else
                    {
                        SelectNode(endNode, false, tva);
                    }
                }
                else if (((keys & Keys.Control) == 0) && ((keys & Keys.Shift) != 0))
                {
                    // SHIFT pressed
                    if (_tnSelectionMirrorPoint == null)
                    {
                        _tnSelectionMirrorPoint = startNode;
                    }

                    switch (_selectionMode)
                    {
                    case TreeViewSelectionMode.SingleSelect:
                        UnselectAllNodesExceptNode(endNode, tva);
                        SelectNode(endNode, true, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameRootBranch:
                        TreeNode tnAbsoluteParentStartNode = GetRootParent(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                TreeNode tnAbsoluteParent = GetRootParent(tnTemp);
                                if (tnAbsoluteParent == tnAbsoluteParentStartNode)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToParent(tnAbsoluteParentStartNode, tva);
                        UnselectNodesOutsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameLevel:
                        intNodeLevelStart = GetNodeLevel(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                int intNodeLevel = GetNodeLevel(tnTemp);
                                if (intNodeLevel == intNodeLevelStart)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToLevel(intNodeLevelStart, tva);
                        UnselectNodesOutsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameLevelAndRootBranch:
                        TreeNode tnAbsoluteParentStart = GetRootParent(startNode);
                        intNodeLevelStart = GetNodeLevel(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                int intNodeLevel = GetNodeLevel(tnTemp);
                                TreeNode tnAbsoluteParent = GetRootParent(tnTemp);
                                if ((intNodeLevel == intNodeLevelStart) && (tnAbsoluteParent == tnAbsoluteParentStart))
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToParent(tnAbsoluteParentStart, tva);
                        UnselectAllNodesNotBelongingToLevel(intNodeLevelStart, tva);
                        UnselectNodesOutsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelect:
                        SelectNodesInsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        UnselectNodesOutsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameParent:
                        if (startNode == null)
                        {
                            break;
                        }

                        TreeNode tnParentStartNode = startNode.Parent;
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                TreeNode tnParent = tnTemp.Parent;
                                if (tnParent == tnParentStartNode)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingDirectlyToParent(tnParentStartNode, tva);
                        UnselectNodesOutsideRange(_tnSelectionMirrorPoint, endNode, tva);
                        break;
                    }
                }
                else if (((keys & Keys.Control) != 0) && ((keys & Keys.Shift) != 0))
                {
                    // SHIFT AND CTRL pressed
                    switch (_selectionMode)
                    {
                    case TreeViewSelectionMode.SingleSelect:
                        UnselectAllNodesExceptNode(endNode, tva);
                        SelectNode(endNode, true, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameRootBranch:
                        TreeNode tnAbsoluteParentStartNode = GetRootParent(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                TreeNode tnAbsoluteParent = GetRootParent(tnTemp);
                                if (tnAbsoluteParent == tnAbsoluteParentStartNode)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToParent(tnAbsoluteParentStartNode, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameLevel:
                        intNodeLevelStart = GetNodeLevel(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                int intNodeLevel = GetNodeLevel(tnTemp);
                                if (intNodeLevel == intNodeLevelStart)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToLevel(intNodeLevelStart, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelectSameLevelAndRootBranch:
                        TreeNode tnAbsoluteParentStart = GetRootParent(startNode);
                        intNodeLevelStart = GetNodeLevel(startNode);
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                int intNodeLevel = GetNodeLevel(tnTemp);
                                TreeNode tnAbsoluteParent = GetRootParent(tnTemp);
                                if ((intNodeLevel == intNodeLevelStart) && (tnAbsoluteParent == tnAbsoluteParentStart))
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingToParent(tnAbsoluteParentStart, tva);
                        UnselectAllNodesNotBelongingToLevel(intNodeLevelStart, tva);
                        break;

                    case TreeViewSelectionMode.MultiSelect:
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                SelectNode(tnTemp, true, tva);
                            }
                        }

                        break;

                    case TreeViewSelectionMode.MultiSelectSameParent:
                        TreeNode tnParentStartNode = startNode.Parent;
                        tnTemp = startNode;

                        // Check each visible node from startNode to endNode and select it if needed
                        while ((tnTemp != null) && (tnTemp != endNode))
                        {
                            if (startNode.Bounds.Y > endNode.Bounds.Y)
                            {
                                tnTemp = tnTemp.PrevVisibleNode;
                            }
                            else
                            {
                                tnTemp = tnTemp.NextVisibleNode;
                            }

                            if (tnTemp != null)
                            {
                                TreeNode tnParent = tnTemp.Parent;
                                if (tnParent == tnParentStartNode)
                                {
                                    SelectNode(tnTemp, true, tva);
                                }
                            }
                        }

                        UnselectAllNodesNotBelongingDirectlyToParent(tnParentStartNode, tva);
                        break;
                    }
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                // if right mouse button clicked, clear selection and select right-clicked node
                if (!IsNodeSelected(endNode))
                {
                    UnselectAllNodes(tva);
                    SelectNode(endNode, true, tva);
                }
            }

            OnSelectionsChanged();
        }

        /// <summary>
        /// Occurs when the SelectedNodes collection was cleared.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SelectedNodes_SelectedNodesCleared(object sender, EventArgs e)
        {
            _blnSelectionChanged = false;

            UnselectAllNodes(TreeViewAction.Unknown);

            OnSelectionsChanged();
        }

        /// <summary>
        /// Occurs when a tree node is added to the SelectedNodes collection.
        /// </summary>
        /// <param name="tn">Tree node that was added.</param>
        private void SelectedNodes_TreeNodeAdded(TreeNode tn)
        {
            _blnSelectionChanged = false;

            SelectNode(tn, true, TreeViewAction.Unknown);

            // ProcessNodeRange(null, tn, new MouseEventArgs(MouseButtons.Left, 1, Cursor.Position.X,  Cursor.Position.Y, 0), Keys.None, TreeViewAction.ByKeyboard, false);
            OnSelectionsChanged();
        }

        /// <summary>
        /// Occurs when a tree node is inserted to the SelectedNodes collection.
        /// </summary>
        /// <param name="tn">tree node that was inserted.</param>
        private void SelectedNodes_TreeNodeInserted(TreeNode tn)
        {
            _blnSelectionChanged = false;

            SelectNode(tn, true, TreeViewAction.Unknown);

            OnSelectionsChanged();
        }

        /// <summary>
        /// Occurs when a tree node is removed from the SelectedNodes collection.
        /// </summary>
        /// <param name="tn">Tree node that was removed.</param>
        private void SelectedNodes_TreeNodeRemoved(TreeNode tn)
        {
            _blnSelectionChanged = false;

            SelectNode(tn, false, TreeViewAction.Unknown);

            OnSelectionsChanged();
        }

        /// <summary>
        /// (Un)selects the specified node.
        /// </summary>
        /// <param name="tn">Node to (un)select.</param>
        /// <param name="select">True to select node, false to unselect node.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        /// <returns>True if node was selected, false if not.</returns>
        private bool SelectNode(TreeNode tn, bool select, TreeViewAction tva)
        {
            bool blnSelected = false;

            if (tn == null)
            {
                return false;
            }

            if (select)
            {
                // Only try to select node if it was not already selected
                if (!IsNodeSelected(tn))
                {
                    // Check if node selection is cancelled
                    TreeViewCancelEventArgs tvcea = new TreeViewCancelEventArgs(tn, false, tva);
                    base.OnBeforeSelect(tvcea);
                    if (tvcea.Cancel)
                    {
                        // This node selection was cancelled!
                        return false;
                    }

                    PreserveNodeColors(tn);

                    tn.BackColor = SelectionBackColor; // GKM moved from above
                    tn.ForeColor = BackColor; // GKM moved from above

                    _htblSelectedNodes.Add(tn.GetHashCode(), tn);
                    blnSelected = true;
                    _blnSelectionChanged = true;

                    base.OnAfterSelect(new TreeViewEventArgs(tn, tva));
                }

                _tnMostRecentSelectedNode = tn;
            }
            else
            {
                // Only unselect node if it was selected
                if (IsNodeSelected(tn))
                {
                    OnBeforeDeselect(tn);

                    Color[] originalColors = (Color[])_htblSelectedNodesOrigColors[tn.GetHashCode()];
                    if (originalColors != null)
                    {
                        _htblSelectedNodes.Remove(tn.GetHashCode());
                        _blnSelectionChanged = true;
                        _htblSelectedNodesOrigColors.Remove(tn.GetHashCode());

                        // GKM - Restore original node colors
                        tn.BackColor = originalColors[0]; // GKM - was BackColor;
                        tn.ForeColor = originalColors[1]; // GKM - was ForeColor;
                    }

                    OnAfterDeselect(tn);
                }
            }

            return blnSelected;
        }

        /// <summary>
        /// Selects nodes within the specified range.
        /// </summary>
        /// <param name="startNode">Start node.</param>
        /// <param name="endNode">End Node.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void SelectNodesInsideRange(TreeNode startNode, TreeNode endNode, TreeViewAction tva)
        {
            // Calculate start node and end node
            TreeNode firstNode = null;
            TreeNode lastNode = null;
            if (startNode.Bounds.Y < endNode.Bounds.Y)
            {
                firstNode = startNode;
                lastNode = endNode;
            }
            else
            {
                firstNode = endNode;
                lastNode = startNode;
            }

            // Select each node in range
            SelectNode(firstNode, true, tva);
            TreeNode tnTemp = firstNode;
            while (tnTemp != lastNode)
            {
                tnTemp = tnTemp.NextVisibleNode;
                if (tnTemp != null)
                {
                    SelectNode(tnTemp, true, tva);
                }
            }

            SelectNode(lastNode, true, tva);
        }

        /// <summary>
        /// makes focus rectangle visible or hides it.
        /// </summary>
        /// <param name="tn">Node to make focus rectangle (in)visible for.</param>
        /// <param name="visible">True to make focus rectangle visible, false to hide it.</param>
        private void SetFocusToNode(TreeNode tn, bool visible)
        {
            Graphics g = CreateGraphics();
            Rectangle rect = new Rectangle(tn.Bounds.X, tn.Bounds.Y, tn.Bounds.Width, tn.Bounds.Height);
            if (visible)
            {
                Invalidate(rect, false);
                Update();
                if (tn.BackColor != SelectionBackColor)
                {
                    var sb = new SolidBrush(SelectionBackColor);
                    Pen p = new Pen(sb, 1);
                    g.DrawRectangle(p, rect);
                    p.Dispose();
                    sb.Dispose();
                }
            }
            else
            {
                if (tn.BackColor != SelectionBackColor)
                {
                    var sb = new SolidBrush(BackColor);
                    Pen p = new Pen(sb, 1);
                    g.DrawRectangle(p, _tnMostRecentSelectedNode.Bounds.X, _tnMostRecentSelectedNode.Bounds.Y, _tnMostRecentSelectedNode.Bounds.Width, _tnMostRecentSelectedNode.Bounds.Height);
                    p.Dispose();
                    sb.Dispose();
                }

                Invalidate(rect, false);
                Update();
            }
        }

        /// <summary>
        /// Starts edit on a node.
        /// </summary>
        private void StartEdit()
        {
            Thread.Sleep(200);
            if (!_blnWasDoubleClick)
            {
                _blnInternalCall = true;
                SelectedNode = _tnNodeToStartEditOn;
                _blnInternalCall = false;
                _tnNodeToStartEditOn.BeginEdit();
            }
            else
            {
                _blnWasDoubleClick = false;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            // get node at mouse position
            Point pt = PointToClient(MousePosition);
            TreeNode node = GetNodeAt(pt);

            if (node == null)
            {
                return;
            }

            if (pt.Y < 30)                              // if mouse is near to the top, scroll up
            {
                // set actual node to the upper one
                if (node.PrevVisibleNode != null)
                {
                    node = node.PrevVisibleNode;

                    // hide drag image
                    NativeMethods.ImageList_DragShowNolock(false);

                    // scroll and refresh
                    node.EnsureVisible();
                    Refresh();

                    // show drag image
                    NativeMethods.ImageList_DragShowNolock(true);
                }
            }
            else if (pt.Y > Size.Height - 30)           // if mouse is near to the bottom, scroll down
            {
                if (node.NextVisibleNode != null)
                {
                    node = node.NextVisibleNode;

                    NativeMethods.ImageList_DragShowNolock(false);
                    node.EnsureVisible();
                    Refresh();
                    NativeMethods.ImageList_DragShowNolock(true);
                }
            }
        }

        /// <summary>
        /// Unselects all selected nodes.
        /// </summary>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectAllNodes(TreeViewAction tva)
        {
            UnselectAllNodesExceptNode(null, tva);
        }

        /// <summary>
        /// Unselects all selected nodes, except for the specified node which should not be touched.
        /// </summary>
        /// <param name="nodeKeepSelected">Node not to touch.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectAllNodesExceptNode(TreeNode nodeKeepSelected, TreeViewAction tva)
        {
            // First, build list of nodes that need to be unselected
            ArrayList arrNodesToDeselect = new ArrayList();
            foreach (TreeNode selectedTreeNode in _htblSelectedNodes.Values)
            {
                if (nodeKeepSelected == null)
                {
                    arrNodesToDeselect.Add(selectedTreeNode);
                }
                else if ((nodeKeepSelected != null) && (selectedTreeNode != nodeKeepSelected))
                {
                    arrNodesToDeselect.Add(selectedTreeNode);
                }
            }

            // Do the actual unselect
            foreach (TreeNode tnToDeselect in arrNodesToDeselect)
            {
                SelectNode(tnToDeselect, false, tva);
            }
        }

        /// <summary>
        /// Unselects all selected nodes that don't belong directly to the specified parent.
        /// </summary>
        /// <param name="parent">ParentFolder node.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectAllNodesNotBelongingDirectlyToParent(TreeNode parent, TreeViewAction tva)
        {
            // First, build list of nodes that need to be unselected
            ArrayList arrNodesToDeselect = new ArrayList();
            foreach (TreeNode selectedTreeNode in _htblSelectedNodes.Values)
            {
                if (selectedTreeNode.Parent != parent)
                {
                    arrNodesToDeselect.Add(selectedTreeNode);
                }
            }

            // Do the actual unselect
            foreach (TreeNode tnToDeselect in arrNodesToDeselect)
            {
                SelectNode(tnToDeselect, false, tva);
            }
        }

        /// <summary>
        /// Unselects all selected nodes that don't belong to the specified level.
        /// </summary>
        /// <param name="level">Node level.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectAllNodesNotBelongingToLevel(int level, TreeViewAction tva)
        {
            // First, build list of nodes that need to be unselected
            ArrayList arrNodesToDeselect = new ArrayList();
            foreach (TreeNode selectedTreeNode in _htblSelectedNodes.Values)
            {
                if (GetNodeLevel(selectedTreeNode) != level)
                {
                    arrNodesToDeselect.Add(selectedTreeNode);
                }
            }

            // Do the actual unselect
            foreach (TreeNode tnToDeselect in arrNodesToDeselect)
            {
                SelectNode(tnToDeselect, false, tva);
            }
        }

        /// <summary>
        /// Unselects all selected nodes that don't belong directly or indirectly to the specified parent.
        /// </summary>
        /// <param name="parent">ParentFolder node.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectAllNodesNotBelongingToParent(TreeNode parent, TreeViewAction tva)
        {
            // First, build list of nodes that need to be unselected
            ArrayList arrNodesToDeselect = new ArrayList();
            foreach (TreeNode selectedTreeNode in _htblSelectedNodes.Values)
            {
                if (!IsChildOf(selectedTreeNode, parent))
                {
                    arrNodesToDeselect.Add(selectedTreeNode);
                }
            }

            // Do the actual unselect
            foreach (TreeNode tnToDeselect in arrNodesToDeselect)
            {
                SelectNode(tnToDeselect, false, tva);
            }
        }

        /// <summary>
        /// Unselects nodes outside the specified range.
        /// </summary>
        /// <param name="startNode">Start node.</param>
        /// <param name="endNode">End node.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectNodesOutsideRange(TreeNode startNode, TreeNode endNode, TreeViewAction tva)
        {
            // Calculate start node and end node
            TreeNode firstNode;
            TreeNode lastNode;
            if (startNode.Bounds.Y < endNode.Bounds.Y)
            {
                firstNode = startNode;
                lastNode = endNode;
            }
            else
            {
                firstNode = endNode;
                lastNode = startNode;
            }

            // Unselect each node outside range
            TreeNode tnTemp = firstNode;
            while (tnTemp != null)
            {
                tnTemp = tnTemp.PrevVisibleNode;
                if (tnTemp != null)
                {
                    SelectNode(tnTemp, false, tva);
                }
            }

            tnTemp = lastNode;
            while (tnTemp != null)
            {
                tnTemp = tnTemp.NextVisibleNode;
                if (tnTemp != null)
                {
                    SelectNode(tnTemp, false, tva);
                }
            }
        }

        /// <summary>
        /// Recursively unselect node.
        /// </summary>
        /// <param name="tn">Node to recursively unselect.</param>
        /// <param name="tva">Specifies the action that caused the selection change.</param>
        private void UnselectNodesRecursively(TreeNode tn, TreeViewAction tva)
        {
            SelectNode(tn, false, tva);
            foreach (TreeNode child in tn.Nodes)
            {
                UnselectNodesRecursively(child, tva);
            }
        }
    }
}