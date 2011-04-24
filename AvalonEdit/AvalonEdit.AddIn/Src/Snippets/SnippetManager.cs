// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace Revsoft.Wabbitcode.AvalonEditExtension.Snippets
{
	/// <summary>
	/// SnippetManager singleton.
	/// </summary>
	public sealed class SnippetManager
	{
		readonly object lockObj = new object();
		internal static readonly List<CodeSnippetGroup> defaultSnippets = new List<CodeSnippetGroup> {
			new CodeSnippetGroup {
				Extensions = ".asm",
				Snippets = {
					new CodeSnippet {
						Name = "for",
						Description = "for loop",
						Text = "for (int ${counter=i} = 0; ${counter} < ${end}; ${counter}++) {\n\t${Selection}\n}",
						Keyword = "for"
					},
				}
			},
		};
		
		public static readonly SnippetManager Instance = new SnippetManager();
		
		readonly List<ISnippetElementProvider> snippetElementProviders;
		
		public List<ISnippetElementProvider> SnippetElementProviders {
			get { return snippetElementProviders; }
		}
		
		private SnippetManager()
		{
			snippetElementProviders = AddInTree.BuildItems<ISnippetElementProvider>("/SharpDevelop/ViewContent/AvalonEdit/SnippetElementProviders", null, false);
		}
		
		/// <summary>
		/// Loads copies of all code snippet groups.
		/// </summary>
		/*public List<CodeSnippetGroup> LoadGroups()
		{
			var savedSnippets = PropertyService.Get("CodeSnippets", new List<CodeSnippetGroup>());
			
			foreach (var group in savedSnippets) {
				var defaultGroup = defaultSnippets.FirstOrDefault(i => i.Extensions == group.Extensions);
				if (defaultGroup != null) {
					var merged = group.Snippets.Concat(
						defaultGroup.Snippets.Except(
							group.Snippets,
							new ByMemberComparer<CodeSnippet, string>(s => s.Name)
						).Select(s => new CodeSnippet(s)) // clone snippets so that defaultGroup is not modified
					).OrderBy(s => s.Name).ToList();
					group.Snippets.Clear();
					group.Snippets.AddRange(merged);
				}
			}
			
			foreach (var group in defaultSnippets.Except(savedSnippets, new ByMemberComparer<CodeSnippetGroup, string>(g => g.Extensions))) {
				savedSnippets.Add(group);
			}
			
			return savedSnippets;
		}*/
		
		sealed class ByMemberComparer<TObject, TMember> : IEqualityComparer<TObject>
		{
			readonly Func<TObject, TMember> selector;
			readonly IEqualityComparer<TMember> memberComparer = EqualityComparer<TMember>.Default;
			
			public ByMemberComparer(Func<TObject, TMember> selector)
			{
				this.selector = selector;
			}
			
			public bool Equals(TObject x, TObject y)
			{
				return memberComparer.Equals(selector(x), selector(y));
			}
			
			public int GetHashCode(TObject obj)
			{
				return memberComparer.GetHashCode(selector(obj));
			}
		}
		
		/// <summary>
		/// Saves the set of groups.
		/// </summary>
		public void SaveGroups(IEnumerable<CodeSnippetGroup> groups)
		{
			lock (lockObj) {
				activeGroups = null;
				List<CodeSnippetGroup> modifiedGroups = new List<CodeSnippetGroup>();
				
				foreach (var group in groups) {
					var defaultGroup = defaultSnippets.FirstOrDefault(i => i.Extensions == group.Extensions);
					
					IEnumerable<CodeSnippet> saveSnippets = group.Snippets;
					
					if (defaultGroup != null) {
						saveSnippets = group.Snippets.Except(defaultGroup.Snippets);
					}
					
					// save all groups, even if they're empty
					var copy = new CodeSnippetGroup() {
						Extensions = group.Extensions
					};
					//copy.Snippets.AddRange(saveSnippets);
					modifiedGroups.Add(copy);
				}
				
				//PropertyService.Set("CodeSnippets", modifiedGroups);
			}
		}
		
		ReadOnlyCollection<CodeSnippetGroup> activeGroups;
		
		public ReadOnlyCollection<CodeSnippetGroup> ActiveGroups {
			get {
				lock (lockObj) {
					/*if (activeGroups == null)
						activeGroups = LoadGroups().AsReadOnly();*/
					return activeGroups;
				}
			}
		}
		
		public CodeSnippetGroup FindGroup(string extension)
		{
			foreach (CodeSnippetGroup g in ActiveGroups) {
				string[] extensions = g.Extensions.Split(';');
				foreach (string gext in extensions) {
					if (gext.Equals(extension, StringComparison.OrdinalIgnoreCase))
						return g;
				}
			}
			return null;
		}
		
		public CodeSnippet FindSnippet(string extension, string name)
		{
			CodeSnippetGroup g = FindGroup(extension);
			if (g != null) {
				return g.Snippets.FirstOrDefault(s => s.Name == name);
			}
			return null;
		}
	}
}
