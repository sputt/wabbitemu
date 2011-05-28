using System;
using System.Threading;
using System.Collections;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode
{
	public sealed class WorkItem
	{
		private WaitCallback _callback;
		private object _state;
		private ExecutionContext _ctx;

		internal WorkItem(WaitCallback wc, object state, ExecutionContext ctx)
		{
			_callback = wc;
			_state = state;
			_ctx = ctx;
		}

		internal WaitCallback Callback { get { return _callback; } }
		internal object State { get { return _state; } }
		internal ExecutionContext Context { get { return _ctx; } }
	}

	public enum WorkItemStatus
	{
		Completed, Queued, Executing, Aborted
	}

	public static class AbortableThreadPool
	{
		private static LinkedList<WorkItem> _callbacks = new LinkedList<WorkItem>();
		private static Dictionary<WorkItem, Thread> _threads = new Dictionary<WorkItem, Thread>();

		public static WorkItem QueueUserWorkItem(WaitCallback callback)
		{
			return QueueUserWorkItem(callback, null);
		}

		public static WorkItem QueueUserWorkItem(WaitCallback callback, object state)
		{
			if (callback == null)
				throw new ArgumentNullException("callback");
			WorkItem item = new WorkItem(callback, state, ExecutionContext.Capture());
			lock (_callbacks) _callbacks.AddLast(item);
			ThreadPool.QueueUserWorkItem(new WaitCallback(HandleItem));
			return item;
		}

		private static void HandleItem(object ignored)
		{
			WorkItem item = null;
			try
			{
				lock (_callbacks)
				{
					if (_callbacks.Count > 0)
					{
						item = _callbacks.First.Value;
						_callbacks.RemoveFirst();
					}
					if (item == null) return;
					_threads.Add(item, Thread.CurrentThread);
				}

				ExecutionContext.Run(item.Context,
				  delegate { item.Callback(item.State); }, null);
			}
			finally
			{
				lock (_callbacks)
				{
					if (item != null) _threads.Remove(item);
				}
			}
		}

		public static WorkItemStatus Cancel(WorkItem item, bool allowAbort)
		{
			if (item == null) 
				throw new ArgumentNullException("item");

			lock (_callbacks)
			{
				LinkedListNode<WorkItem> node = _callbacks.Find(item);
				if (node != null)
				{
					_callbacks.Remove(node);
					return WorkItemStatus.Queued;
				}
				else if (_threads.ContainsKey(item))
				{
					if (allowAbort)
					{
						_threads[item].Abort();
						_threads.Remove(item);
						return WorkItemStatus.Aborted;
					}
					else return WorkItemStatus.Executing;
				}
				else return WorkItemStatus.Completed;
			}
		}
	}

}
