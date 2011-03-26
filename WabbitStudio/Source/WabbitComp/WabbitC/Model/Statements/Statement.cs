using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    abstract class Statement : IEnumerable<Block>
    {
        public HashSet<String> Properties;

        #region IEnumerable<Block> Members

        public virtual IEnumerator<Block> GetEnumerator()
        {
            return new List<Block>().GetEnumerator();
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
