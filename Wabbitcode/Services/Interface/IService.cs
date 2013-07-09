namespace Revsoft.Wabbitcode.Services.Interface
{
    using System;

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