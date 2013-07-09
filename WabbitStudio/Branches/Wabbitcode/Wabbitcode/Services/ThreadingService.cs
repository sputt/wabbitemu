namespace Revsoft.Wabbitcode.Services
{
    using System.Threading;

    public static class ThreadingService
    {
        public static void RequestNewThread(WaitCallback callback)
        {
            ThreadPool.QueueUserWorkItem(callback);
        }
    }
}