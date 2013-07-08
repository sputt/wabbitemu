namespace Revsoft.Wabbitcode.Services
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Reflection;
    using System.Text;

    using Revsoft.Wabbitcode.Services.Interface;

    public class ServiceFactory : IServiceFactory
    {
        private IDictionary<Type, IService> cachedServices;
        private IDictionary<Type, Type> impDict;

        public ServiceFactory(IDictionary<Type, Type> implementationDictionary)
        {
            this.cachedServices = new Dictionary<Type, IService>();
            this.impDict = implementationDictionary;

            foreach (var item in this.impDict)
            {
                if (item.Value.GetInterface(item.Key.Name) == null)
                {
                    throw new Exception("Type of '" + item.Value + "' does not implement type of '" + item.Key  + "'");
                }
            }
        }

        public static IServiceFactory Instance
        {
            get;
            set;
        }

        /// <summary>
        /// Creates and initializes an instance of type T
        /// </summary>
        /// <typeparam name="T">Type of service to initialize</typeparam>
        /// <returns>Instance of the service created</returns>
        public T CreateServiceInstance<T>()
        where T : IService
        {
            return this.CreateServiceInstance<T>(null);
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

            if (!this.impDict.ContainsKey(typeof(T)))
            {
                throw new Exception("Could not find service type registered");
            }

            Type typeToCreate = this.impDict[typeof(T)];
            T newInstance;
            if (this.cachedServices.ContainsKey(typeof(T)))
            {
                newInstance = (T)this.cachedServices[typeof(T)];
            }
            else
            {
                // Our attribute is used to specify service dependencies
                var attributesList = typeToCreate.GetCustomAttributes(true)
                                     .OfType<Services.ServiceDependencyAttribute>().OrderBy(s => s.ServiceType.Name);
                var serviceList = new List<IService>();

                foreach (var dependency in attributesList)
                {
                    Type serviceType = dependency.ServiceType;
                    if (!this.impDict.ContainsKey(serviceType))
                    {
                        throw new Exception("Could not find service type registered");
                    }

                    // var foundService = ServiceFactory.Instance.GetServiceInstance<instantiatedType>();
                    var genericMethod = typeof(ServiceFactory).GetMethod("GetServiceInstance", Type.EmptyTypes)
                                        .MakeGenericMethod(serviceType);
                    IService foundService = (IService)genericMethod.Invoke(this, null);
                    serviceList.Add(foundService);
                }

                IService[] paramsArray = new IService[serviceList.Count];
                serviceList.CopyTo(paramsArray);
                newInstance = (T)Activator.CreateInstance(typeToCreate, paramsArray);
                this.cachedServices.Add(typeof(T), newInstance);
            }

            newInstance.InitService(objects);
            return newInstance;
        }

        /// <summary>
        /// Destroys the instance of the service passed in
        /// </summary>
        /// <param name="serviceToDestroy">Service instance to destroy</param>
        public void DestroyServiceInstance<T>()
        where T : IService
        {
            if (!this.cachedServices.ContainsKey(typeof(T)))
            {
                return;
            }

            IService serviceToDestroy = this.cachedServices[typeof(T)];
            this.cachedServices.Remove(typeof(T));
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
            return this.GetServiceInstance<T>(null);
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
            if (this.cachedServices.TryGetValue(typeof(T), out service) == true)
            {
                foundService = (T)service;
            }
            else
            {
                foundService = this.CreateServiceInstance<T>(objects);
            }

            return foundService;
        }
    }
}