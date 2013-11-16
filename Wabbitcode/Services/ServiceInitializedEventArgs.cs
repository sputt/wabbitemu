using System;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    public class ServiceInitializedEventArgs : EventArgs
    {
        public Type ServiceType { get; private set; }
        public IService Service { get; private set; }

        public ServiceInitializedEventArgs(Type serviceType, IService service)
        {
            ServiceType = serviceType;
            Service = service;
        }
    }
}