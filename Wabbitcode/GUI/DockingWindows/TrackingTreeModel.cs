using System;
using System.Collections;
using System.Collections.Generic;
using Aga.Controls.Tree;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public class TrackingTreeModel<T> : ITreeModel
    {
        private readonly List<T> _nodes = new List<T>();

        public List<T> Nodes { get { return _nodes; } }

        public IEnumerable GetChildren(TreePath treePath)
        {
            return _nodes;
        }

        public bool IsLeaf(TreePath treePath)
        {
            return true;
        }

        private static TreePath GetPath(T item)
        {
            return item == null ? TreePath.Empty : new TreePath(new List<object>{ item });
        }

        public event EventHandler<TreeModelEventArgs> NodesChanged;
        public event EventHandler<TreeModelEventArgs> NodesInserted;
        public event EventHandler<TreeModelEventArgs> NodesRemoved;
        public event EventHandler<TreePathEventArgs> StructureChanged;

        internal void OnNodesChanged(T row)
        {
            if (NodesChanged == null)
            {
                return;
            }

            NodesChanged(this, new TreeModelEventArgs(TreePath.Empty, new object[] { row }));
        }

        internal void OnNodesRemoved(T row)
        {
            if (NodesRemoved == null)
            {
                return;
            }

            NodesRemoved(this, new TreeModelEventArgs(TreePath.Empty, new object[] { row }));
        }

        internal void OnStructureChanged()
        {
            if (StructureChanged != null)
            {
                StructureChanged(this, new TreePathEventArgs());
            }
        }
    }
}