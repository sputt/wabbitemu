using Microsoft.Practices.Unity;

namespace Revsoft.Wabbitcode.Services
{
	public static class DependencyFactory
	{
	    private static readonly UnityContainer UnityContainer = new UnityContainer();

		/// <summary>
		/// Creates and initializes an instance of type T
		/// </summary>
		/// <typeparam name="T">Type of service to initialize</typeparam>
		/// <returns>Instance of the service created</returns>
		public static T Resolve<T>()
		{
            T ret = default(T);

            if (UnityContainer.IsRegistered(typeof(T)))
            {
                ret = UnityContainer.Resolve<T>();
            }

            return ret;
		}

        public static void RegisterType<TFrom, TTo>() where TTo : TFrom
        {
            UnityContainer.RegisterType<TFrom, TTo>(new ContainerControlledLifetimeManager());
        }

        public static void RegisterType<TFrom, TTo>(InjectionConstructor injectionConstructor) where TTo : TFrom
	    {
            UnityContainer.RegisterType<TFrom, TTo>(new ContainerControlledLifetimeManager(), injectionConstructor);
	    }
	}
}