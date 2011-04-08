using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Statements;

namespace WabbitC.Model
{
    class BlockEnumerator : IEnumerator<Block>
    {
        private Block baseBlock;
        private Block currentBlock;
        private IEnumerator<Statement> currentStatement;
        private IEnumerator<Block> currentSubblock;
        BlockEnumerator childEnum;

        public BlockEnumerator(Block start)
        {
            baseBlock = start;
            this.Reset();
        }

        #region IEnumerator<Block> Members

        public Block Current
        {
            get { return currentBlock; }
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            //throw new NotImplementedException();
        }

        #endregion

        #region IEnumerator Members

        object System.Collections.IEnumerator.Current
        {
			get { return currentStatement; }
        }

        public bool MoveNext()
        {
            if (currentBlock == null)
            {
                currentBlock = baseBlock;
                currentStatement = currentBlock.Statements.GetEnumerator();
                return true;
            }

            if (childEnum != null)
            {
                if (childEnum.MoveNext())
                {
                    currentBlock = childEnum.Current;
                    return true;
                }
                childEnum = null;
            }

            if (currentSubblock != null)
            {
                if (currentSubblock.MoveNext())
                {
                    childEnum = new BlockEnumerator(currentSubblock.Current);
                    return this.MoveNext();
                }
                currentSubblock = null;
            }

            while (currentStatement.MoveNext())
            {
                currentSubblock = currentStatement.Current.GetEnumerator();
                return this.MoveNext();
            }

            return false;
        }

        public void Reset()
        {
            currentBlock = null;
            currentStatement = null;
            currentSubblock = null;
            childEnum = null;
        }

        #endregion
    }
}
