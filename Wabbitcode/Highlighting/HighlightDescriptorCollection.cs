using System;
using System.Collections;

namespace Wabbitcode
{
	/// <summary>
	/// Summary description for SeperaratorCollection.
	/// </summary>
	public class HighLightDescriptorCollection
	{
		private ArrayList mInnerList = new ArrayList();
		internal HighLightDescriptorCollection()
		{
		}

		public void AddRange(ICollection c)
		{
			mInnerList.AddRange(c);
		}


		#region IList Members

		public bool IsReadOnly
		{
			get
			{
				return mInnerList.IsReadOnly;
			}
		}

		public HighlightDescriptor this[int index]
		{
			get
			{
				return (HighlightDescriptor)mInnerList[index];
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

		public void Insert(int index, HighlightDescriptor value)
		{
			mInnerList.Insert(index, value);
		}

		public void Remove(HighlightDescriptor value)
		{
			mInnerList.Remove(value);
		}

		public bool Contains(HighlightDescriptor value)
		{
			return mInnerList.Contains(value);
		}

		public void Clear()
		{
			mInnerList.Clear();
		}

		public int IndexOf(HighlightDescriptor value)
		{
			return mInnerList.IndexOf(value);
		}

		public int Add(HighlightDescriptor value)
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
