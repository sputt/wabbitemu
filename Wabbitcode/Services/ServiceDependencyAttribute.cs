using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services
{
	[AttributeUsage(AttributeTargets.Class, AllowMultiple=true)]
	public class ServiceDependencyAttribute : Attribute
	{
		string dependencyName;
		public ServiceDependencyAttribute(string dependencyName)
		{
			this.dependencyName = dependencyName;
		}

		public string DependencyName
		{
			get { return dependencyName; }
		}
	}
}
