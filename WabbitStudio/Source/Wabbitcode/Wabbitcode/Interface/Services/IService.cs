using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Interface
{
	public interface IService
	{
		/// <summary>
		/// Initializes the service
		/// </summary>
		void InitService(params Object[] objects);

		/// <summary>
		/// Destroys the service
		/// </summary>
		void DestroyService();
	}
}
