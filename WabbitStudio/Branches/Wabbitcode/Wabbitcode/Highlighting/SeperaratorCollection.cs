namespace Wabbitcode
{
    using System;
    using System.Collections;

    /// <summary>
    /// Summary description for SeperaratorCollection.
    /// </summary>
    public class SeperaratorCollection
    {
        private ArrayList mInnerList = new ArrayList();

        internal SeperaratorCollection()
        {
        }

        public int Count
        {
            get
            {
                return this.mInnerList.Count;
            }
        }

        public bool IsFixedSize
        {
            get
            {
                return this.mInnerList.IsFixedSize;
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return this.mInnerList.IsReadOnly;
            }
        }

        public bool IsSynchronized
        {
            get
            {
                return this.mInnerList.IsSynchronized;
            }
        }

        public object SyncRoot
        {
            get
            {
                return this.mInnerList.SyncRoot;
            }
        }

        public char this[int index]
        {
            get
            {
                return (char)this.mInnerList[index];
            }

            set
            {
                this.mInnerList[index] = value;
            }
        }

        public int Add(char value)
        {
            return this.mInnerList.Add(value);
        }

        public void AddRange(ICollection c)
        {
            this.mInnerList.AddRange(c);
        }

        public void Clear()
        {
            this.mInnerList.Clear();
        }

        public bool Contains(char value)
        {
            return this.mInnerList.Contains(value);
        }

        public void CopyTo(Array array, int index)
        {
            this.mInnerList.CopyTo(array, index);
        }

        public IEnumerator GetEnumerator()
        {
            return this.mInnerList.GetEnumerator();
        }

        public int IndexOf(char value)
        {
            return this.mInnerList.IndexOf(value);
        }

        public void Insert(int index, char value)
        {
            this.mInnerList.Insert(index, value);
        }

        public void Remove(char value)
        {
            this.mInnerList.Remove(value);
        }

        public void RemoveAt(int index)
        {
            this.mInnerList.RemoveAt(index);
        }

        internal char[] GetAsCharArray()
        {
            return (char[])this.mInnerList.ToArray(typeof(char));
        }
    }
}