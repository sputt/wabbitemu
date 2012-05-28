using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    interface IDeclarations
    {
		ISet<Declaration> GetModifiedDeclarations();
		ISet<Declaration> GetReferencedDeclarations();
    }

	interface IAllocationScheme
	{
		ISet<Declaration> GetAllocatableRegisters(Declaration decl);
	}

    class Statement : IEnumerable<Block>, IDeclarations, ICloneable, IAllocationScheme
    {
        public Block Block;
        public HashSet<String> Properties;

		public Module Module
		{
			get
			{
				return Block.Module;
			}
		}

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

		public virtual ISet<Declaration> GetModifiedDeclarations()
        {
            return new HashSet<Declaration>();
        }

		public virtual ISet<Declaration> GetReferencedDeclarations()
        {
			return new HashSet<Declaration>();
        }

        #endregion

		#region ICloneable Members

		public object Clone()
		{
			return this.MemberwiseClone();
		}

		#endregion

		#region IAllocationScheme

		public virtual ISet<Declaration> GetAllocatableRegisters(Declaration decl)
		{
			var set = GetModifiedDeclarations().Union(GetReferencedDeclarations());
			if (!set.Contains(decl))
			{
				throw new System.Exception("This declaration is not even in this statement");
			}
			return Module.GeneralPurposeRegisters;
		}

		#endregion

		public virtual string ToAssemblyString()
		{
			return String.Empty;
		}

        public void ReplaceDeclaration(Declaration declToReplace, Declaration newDecl)
        {
            System.Type type = this.GetType();
            //TOOD: remove this. we should not have public fields
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
            var properties = from p in type.GetProperties()
                             where p.PropertyType == typeof(Declaration) || p.PropertyType == typeof(Datum) || p.PropertyType == typeof(List<Declaration>)
                             select p;
            foreach (var property in properties)
            {
                if (property.PropertyType == typeof(List<Declaration>))
                {
                    var decls = property.GetValue(this, null) as List<Declaration>;
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
                    if (property.GetValue(this, null) == declToReplace)
                    {
                        if (property.CanWrite)
                        {
                            property.SetValue(this, newDecl, null);
                        }
                    }
                }
            }
        }
	}
}
