namespace Revsoft.Wabbitcode.Services
{
	using System;

	[AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
	public class ServiceDependencyAttribute : Attribute
	{
		public ServiceDependencyAttribute(Type serviceType)
		{
			ServiceType = serviceType;
			CreateNew = false;
		}

		public ServiceDependencyAttribute(Type serviceType, bool createNew)
		{
			ServiceType = serviceType;
			CreateNew = createNew;
		}

		public Type ServiceType
		{
			get;
			private set;
		}

		public bool CreateNew
		{
			get;
			private set;
		}
	}
}