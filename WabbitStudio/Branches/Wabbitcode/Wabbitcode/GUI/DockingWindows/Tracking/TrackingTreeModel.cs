using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Aga.Controls.Tree;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public class TrackingTreeModel : ITreeModel
    {
        private readonly List<TrackingVariableRowModel> _nodes = new List<TrackingVariableRowModel>();
        public List<TrackingVariableRowModel> Nodes
        {
            get
            {
                return _nodes;
            }
        }

        public event EventHandler<TreeModelEventArgs> NodesChanged;
        public event EventHandler<TreeModelEventArgs> NodesInserted;
        public event EventHandler<TreeModelEventArgs> NodesRemoved;
        public event EventHandler<TreePathEventArgs> StructureChanged;

        public IEnumerable GetChildren(TreePath treePath)
        {
            if (treePath.IsEmpty())
            {
                return _nodes;
            }

            var lastNode = (TrackingVariableRowModel)treePath.LastNode;
            if (_nodes.Contains(lastNode))
            {
                return lastNode.Children;
            }

            return new List<TrackingVariableRowModel>();
        }

        public bool IsLeaf(TreePath treePath)
        {
            return !((TrackingVariableRowModel)treePath.LastNode).Children.Any();
        }

        internal void OnNodesChanged(TrackingVariableRowModel row)
        {
            if (NodesChanged == null)
            {
                return;
            }

            NodesChanged(this, new TreeModelEventArgs(TreePath.Empty, new object[] { row }));
        }

        internal void OnNodesRemoved(TrackingVariableRowModel row)
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