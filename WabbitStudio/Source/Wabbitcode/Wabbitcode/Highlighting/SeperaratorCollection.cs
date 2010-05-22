using System;
using System.Collections;

namespace Wabbitcode
{
	/// <summary>
	/// Summary description for SeperaratorCollection.
	/// </summary>
	public class SeperaratorCollection
	{
		private ArrayList mInnerList = new ArrayList();
		internal SeperaratorCollection()
		{
		}

		public void AddRange(ICollection c)
		{
			mInnerList.AddRange(c);
		}

		internal char[] GetAsCharArray()
		{
			return (char[])mInnerList.ToArray(typeof(char));
		}
		#region IList Members

		public bool IsReadOnly
		{
			get
			{
				return mInnerList.IsReadOnly;
			}
		}

		public char this[int index]
		{
			get
			{
				return (char)mInnerList[index];
			}
			set
			{
				mInnerList[index] = value;
			}
		}

		public void RemoveAt(int index)
		{
			mInnerList.RemoveAt(index);
		}

		public void Insert(int index, char value)
		{
			mInnerList.Insert(index, value);
		}

		public void Remove(char value)
		{
			mInnerList.Remove(value);
		}

		public bool Contains(char value)
		{
			return mInnerList.Contains(value);
		}

		public void Clear()
		{
			mInnerList.Clear();
		}

		public int IndexOf(char value)
		{
			return mInnerList.IndexOf(value);
		}

		public int Add(char value)
		{
			return mInnerList.Add(value);
		}

		public bool IsFixedSize
		{
			get
			{
				return mInnerList.IsFixedSize;
			}
		}

		#endregion

		#region ICollection Members

		public bool IsSynchronized
		{
			get
			{
				return mInnerList.IsSynchronized;
			}
		}

		public int Count
		{
			get
			{
				return mInnerList.Count;
			}
		}

		public void CopyTo(Array array, int index)
		{
			mInnerList.CopyTo(array, index);
		}

		public object SyncRoot
		{
			get
			{
				return mInnerList.SyncRoot;
			}
		}

		#endregion

		#region IEnumerable Members

		public IEnumerator GetEnumerator()
		{
			return mInnerList.GetEnumerator();
		}

		#endregion
	}
}
