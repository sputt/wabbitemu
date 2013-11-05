using System;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IService
    {
        /// <summary>
        /// Destroys the service
        /// </summary>
        void DestroyService();

        /// <summary>
        /// Initializes the service
        /// </summary>
        void InitService(params Object[] objects);
    }
}