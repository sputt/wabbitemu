using System;
using System.Collections.Concurrent;
using System.Threading;
using Microsoft.Practices.Unity;

namespace Revsoft.Wabbitcode.Utils
{
    public static class DependencyFactory
    {
        private static readonly UnityContainer UnityContainer = new UnityContainer();
        private static readonly ConcurrentDictionary<Type, CountdownEvent> Latches = new ConcurrentDictionary<Type, CountdownEvent>();

        /// <summary>
        /// Creates and initializes an instance of type T
        /// </summary>
        /// <typeparam name="T">Type of service to initialize</typeparam>
        /// <returns>Instance of the service created</returns>
        public static T Resolve<T>()
        {
            bool isRegistered;
            lock (UnityContainer)
            {
                isRegistered = UnityContainer.IsRegistered<T>();
            }

            if (!isRegistered)
            {
                CountdownEvent latch = new CountdownEvent(1);
                Latches.TryAdd(typeof(T), latch);
                latch.Wait();
            }

            T ret = UnityContainer.Resolve<T>();

            return ret;
        }

        public static void RegisterType<TFrom, TTo>() where TTo : TFrom
        {
            lock (UnityContainer)
            {
                UnityContainer.RegisterType<TFrom, TTo>(new ContainerControlledLifetimeManager());
            }

            CheckReleaseLatch(typeof(TFrom));
        }

        public static void RegisterType<TFrom, TTo>(InjectionConstructor injectionConstructor) where TTo : TFrom
        {
            lock (UnityContainer)
            {
                UnityContainer.RegisterType<TFrom, TTo>(new ContainerControlledLifetimeManager(), injectionConstructor);
            }

            CheckReleaseLatch(typeof(TFrom));
        }

        public static void RegisterType<TFrom, TTo>(LifetimeManager lifetimeManager) where TTo : TFrom
        {
            lock (UnityContainer)
            {
                UnityContainer.RegisterType<TFrom, TTo>(lifetimeManager);
            }

            CheckReleaseLatch(typeof(TFrom));
        }

        private static void CheckReleaseLatch(Type type)
        {
            if (!Latches.ContainsKey(type))
            {
                return;
            }

            CountdownEvent latch = Latches[type];
            latch.Signal();
        }
    }
}