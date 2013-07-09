namespace Revsoft.Wabbitcode.Services
{
    using System;

    [AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
    public class ServiceDependencyAttribute : Attribute
    {
        private Type serviceType;

        public ServiceDependencyAttribute(Type serviceType)
        {
            this.serviceType = serviceType;
        }

        public Type ServiceType
        {
            get
            {
                return this.serviceType;
            }
        }
    }
}