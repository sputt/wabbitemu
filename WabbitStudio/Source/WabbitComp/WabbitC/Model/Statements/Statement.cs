using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    interface IDeclarations
    {
        List<Declaration> GetModifiedDeclarations();
        List<Declaration> GetReferencedDeclarations();
    }

    class Statement : IEnumerable<Block>, IDeclarations
    {
        public Block Block;
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

        #region IDeclarations Members

        public virtual List<Declaration> GetModifiedDeclarations()
        {
            return new List<Declaration>();
        }

        public virtual List<Declaration> GetReferencedDeclarations()
        {
            return new List<Declaration>();
        }

        #endregion

        public void ReplaceDeclaration(Declaration declToReplace, Declaration newDecl)
        {
            System.Type type = this.GetType();
            var fields = from f in type.GetFields() 
                         where f.FieldType == typeof(Declaration) || f.FieldType == typeof(Datum) || f.FieldType == typeof(List<Declaration>)
                         select f;
            foreach (var field in fields)
            {
                if (field.FieldType == typeof(List<Declaration>))
                {
                    var decls = field.GetValue(this) as List<Declaration>;
                    for (int i = 0; i < decls.Count; i++)
                    {
                        if (decls[i] == declToReplace)
                        {
                            decls[i] = newDecl;
                            break;
                        }
                    }
                }
                else
                {
					if (field.GetValue(this) == declToReplace)
                    {
						field.SetValue(this, newDecl);
                    }
                }
            }
        }
    }
}
