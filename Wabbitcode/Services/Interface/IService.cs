namespace Revsoft.Wabbitcode.Services.Interface
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

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