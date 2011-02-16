using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Revsoft.Wabbitcode.Services
{
    public static class ThreadingService
    {
        public static void RequestNewThread(WaitCallback callback)
        {
            ThreadPool.QueueUserWorkItem(callback);
        }
    }
}
