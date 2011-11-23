using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Utilities
{
	public abstract class AbstractServiceFactory
	{
		public abstract T GetServiceInstance<T>();

		public abstract T GetServiceInstance<T>(params Object[] objects);

		public abstract T CreateServiceInstance<T>();

		public abstract T CreateServiceInstance<T>(params Object[] objects);

		public abstract void DestroyServiceInstance(IService serviceToDestroy);
	}
}
