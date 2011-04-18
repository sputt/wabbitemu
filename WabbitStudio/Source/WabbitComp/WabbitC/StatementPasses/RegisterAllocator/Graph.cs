using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;

namespace WabbitC.StatementPasses.RegisterAllocator
{
	class Graph
	{
		public List<Node> Nodes { get; private set; }

		public Graph()
		{
			Nodes = new List<Node>();
		}

		public Node AddNode(Declaration decl)
		{
			var node = new Node(decl); 
			Nodes.Add(node);
			return node;
		}

		public static void ColorGraph(Graph g)
		{
			
		}
	}

	class Node
	{
		public List<Node> InterferenceNodes { get; private set; }
		public List<Node> ConnectedNodes { get; private set; }
		public Declaration Decl { get; private set; }

		public Node(Declaration decl)
		{
			InterferenceNodes = new List<Node>();
			ConnectedNodes = new List<Node>();
			Decl = decl;
		}

		public void AddConnectedNode(Node node)
		{
			ConnectedNodes.Add(node);
		}

		public void AddInterferingNode(Node node)
		{
			InterferenceNodes.Add(node);
		}

		public override string ToString()
		{
			return Decl.ToString();
		}
	}
}
