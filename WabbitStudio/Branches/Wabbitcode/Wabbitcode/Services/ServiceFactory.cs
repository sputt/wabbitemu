using System.Reflection;
using System;
using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
	public class ServiceFactory : IServiceFactory
	{
		public static IServiceFactory Instance { get; set; }

		private readonly IDictionary<Type, IService> _cachedServices;
		private readonly IDictionary<Type, Type> _impDict;

	    public event EventHandler<ServiceInitializedEventArgs> OnServiceInitialized;

		public ServiceFactory(IDictionary<Type, Type> implementationDictionary)
		{
			_cachedServices = new Dictionary<Type, IService>();
			_impDict = implementationDictionary;

			foreach (var item in _impDict.Where(item => item.Value.GetInterface(item.Key.Name) == null))
			{
				throw new Exception("Type of '" + item.Value + "' does not implement type of '" + item.Key  + "'");
			}
		}

		/// <summary>
		/// Creates and initializes an instance of type T
		/// </summary>
		/// <typeparam name="T">Type of service to initialize</typeparam>
		/// <returns>Instance of the service created</returns>
		public T CreateServiceInstance<T>()
		where T : IService
		{
			return CreateServiceInstance<T>(new object[] {});
		}

		/// <summary>
		/// Creates and initializes an instance of type T
		/// </summary>
		/// <typeparam name="T">Type of service to initialize</typeparam>
		/// <param name="objects">default objects to pass to the service</param>
		/// <returns>Instance of the service created</returns>
		public T CreateServiceInstance<T>(params Object[] objects)
		where T : IService
		{
			if (!typeof(T).IsInterface)
			{
				throw new Exception("Service requested is not an interface");
			}

			if (!_impDict.ContainsKey(typeof(T)))
			{
				throw new Exception("Could not find service type registered");
			}

			Type typeToCreate = _impDict[typeof(T)];
			T newInstance;
			if (_cachedServices.ContainsKey(typeof(T)))
			{
				newInstance = (T)_cachedServices[typeof(T)];
                newInstance.InitService(objects);
			}
			else
			{
				// Our attribute is used to specify service dependencies
				var attributesList = typeToCreate.GetCustomAttributes(true)
									 .OfType<ServiceDependencyAttribute>().OrderBy(s => s.ServiceType.Name);
				var serviceList = new List<IService>();

				foreach (var attribute in attributesList)
				{
					Type serviceType = attribute.ServiceType;
					if (!_impDict.ContainsKey(serviceType))
					{
						throw new Exception("Could not find service type registered");
					}

					var methodString = attribute.CreateNew ? "CreateServiceInstance" : "GetServiceInstance";
					MethodInfo genericMethod = typeof(ServiceFactory).GetMethod(methodString, Type.EmptyTypes)
						.MakeGenericMethod(serviceType);
					IService foundService = (IService)genericMethod.Invoke(this, null);
					serviceList.Add(foundService);
				}

                newInstance = (T)Activator.CreateInstance(typeToCreate, serviceList.Cast<object>().ToArray());
				_cachedServices.Add(typeof(T), newInstance);

                newInstance.InitService(objects);
                if (OnServiceInitialized != null)
                {
                    OnServiceInitialized(this, new ServiceInitializedEventArgs(typeof(T), newInstance));
                }
			}

			return newInstance;
		}

		/// <summary>
		/// Destroys the instance of the service passed in
		/// </summary>
		public void DestroyServiceInstance<T>()
		where T : IService
		{
			if (!_cachedServices.ContainsKey(typeof(T)))
			{
				return;
			}

			IService serviceToDestroy = _cachedServices[typeof(T)];
			_cachedServices.Remove(typeof(T));
			serviceToDestroy.DestroyService();
		}

		/// <summary>
		/// Returns an instance of the service type T
		/// </summary>
		/// <typeparam name="T">Instance of IService</typeparam>
		/// <returns>IService instance of the specified type</returns>
		public T GetServiceInstance<T>()
		where T : IService
		{
			return GetServiceInstance<T>(null);
		}

		/// <summary>
		/// Returns an instance of the service type T
		/// </summary>
		/// <typeparam name="T">Instance of IService</typeparam>
		/// <returns>IService instance of the specified type</returns>
		public T GetServiceInstance<T>(params Object[] objects)
		where T : IService
		{
			if (!typeof(T).IsInterface)
			{
				throw new Exception("Service requested is not an interface");
			}

			IService service;
			T foundService;
			if (_cachedServices.TryGetValue(typeof(T), out service))
			{
				foundService = (T)service;
			}
			else
			{
				foundService = CreateServiceInstance<T>(objects);
			}

			return foundService;
		}
	}
}