using System;
using System.Collections;
using System.Collections.Generic;
using Aga.Controls.Tree;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public class TrackingTreeModel : ITreeModel
    {
        private readonly List<TrackingVariableRowModel> _nodes = new List<TrackingVariableRowModel>();

        public List<TrackingVariableRowModel> Nodes { get { return _nodes; } }

        public IEnumerable GetChildren(TreePath treePath)
        {
            return _nodes;
        }

        public bool IsLeaf(TreePath treePath)
        {
            return true;
        }

        private static TreePath GetPath(TrackingVariableRowModel item)
        {
            return item == null ? TreePath.Empty : new TreePath(new List<object>{ item });
        }

        public event EventHandler<TreeModelEventArgs> NodesChanged;
        public event EventHandler<TreeModelEventArgs> NodesInserted;
        public event EventHandler<TreeModelEventArgs> NodesRemoved;
        public event EventHandler<TreePathEventArgs> StructureChanged;

        internal void OnNodesChanged(TrackingVariableRowModel row)
        {
            if (NodesChanged == null)
            {
                return;
            }

            NodesChanged(this, new TreeModelEventArgs(TreePath.Empty, new object[] { row}));
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